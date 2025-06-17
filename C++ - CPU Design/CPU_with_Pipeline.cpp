#include <bits/stdc++.h>
using namespace std;


//__________________________________________________________________________________________________________//

//************************************************* CPU START **********************************************//
//__________________________________________________________________________________________________________//


//********************************************** Global Variables ******************************************//

int PC;
int DM[1024] = {0};
vector<string> IM;
int GPR[32] = {0};
int N;

//**********************************************************************************************************//


//********************************************** Hazard handling *******************************************//
//Class and Variable to incorporate Pipeline

class Valid
{
public:
    bool PC, ifid, idex, exmo, mowb;
    Valid()
    {
        this->PC = true;
        this->ifid = false;
        this->idex = false;
        this->exmo = false;
        this->mowb = false;
    }
};
Valid valid;

bitset<32> inUse;

bool stopOnce = false;
bool data_bubble = false;
bool control_bubble= false;

//**************************************************************************************************************//


//************************************************ Initializations *********************************************//
class controlWord
{
public:
    int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
};

map<string, controlWord> controlUnit;

void initControlUnit()
{
    // R-type 
    controlUnit["0110011"] = {1, 1, 0, 10, 0, 0, 0, 0, 0};

    // I-type 
    controlUnit["0010011"] = {1, 1, 1, 11, 0, 0, 0, 0, 0};

    // I-type (Load)
    controlUnit["0000011"] = {1, 1, 1, 00, 0, 0, 1, 0, 1};

    // S-type 
    controlUnit["0100011"] = {1, 0, 1, 00, 0, 0, 0, 1, 2}; // Mem2Reg is X (Don't care)

    // B-type
    controlUnit["1100011"] = {1, 0, 0, 01, 1, 0, 0, 0, 2}; // Mem2Reg is X (Don't care)

    // U-type
    controlUnit["0110111"] = {0, 1, 1, 00, 0, 0, 0, 0, 0};

    // J-type
    controlUnit["1101111"] = {0, 1, 0, 00, 0, 1, 0, 0, 0};
}

void initDM()
{
    // DM[11] = 10; 
    // DM[1] = 1;
    
    DM[0] = 10; // N-Sum & Fibonacci
}

//***************************************************************************************************************************//


//****************************************** Instruction Fetch, Instruction Decode ******************************************//

class IFID
{
public:
    string IR;
    int DPC, NPC;
};

//***************************************************************************************************************************//


//***************************************** Instruction Decode, Instruction Execute *****************************************//
class IDEX
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;
        void controller(string str)
        {
            controlWord cow = controlUnit[str];
            this->RegRead = cow.RegRead;
            this->RegWrite = cow.RegWrite;
            this->ALUSrc = cow.ALUSrc;
            this->ALUOp = cow.ALUOp;
            this->Branch = cow.Branch;
            this->Jump = cow.Jump;
            this->MemRead = cow.MemRead;
            this->MemWrite = cow.MemWrite;
            this->Mem2Reg = cow.Mem2Reg;
        }
    };

public:
    string imm1, imm2, func, rdl, rs1, rs2, IR;
    int JPC, DPC;
    CW cw;
};

//***************************************************************************************************************************//


//****************************************** Instruction Execute, Memory Operation ******************************************//
class EXMO
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;

    public:
        void copyCW(IDEX &idex)
        {
            this->RegRead = idex.cw.RegRead;
            this->RegWrite = idex.cw.RegWrite;
            this->ALUSrc = idex.cw.ALUSrc;
            this->ALUOp = idex.cw.ALUOp;
            this->Branch = idex.cw.Branch;
            this->Jump = idex.cw.Jump;
            this->MemRead = idex.cw.MemRead;
            this->MemWrite = idex.cw.MemWrite;
            this->Mem2Reg = idex.cw.Mem2Reg;
        }
    };

public:
    string rdl, rs2;
    int ALUOUT;
    CW cw;
};

//******************************************************************************************************************//


//****************************************** Memory Operation, Write Back ******************************************//

class MOWB
{
public:
    class CW
    {
    public:
        int RegRead, RegWrite, ALUSrc, ALUOp, Branch, Jump, MemRead, MemWrite, Mem2Reg;

    public:
        void copyCW(EXMO exmo)
        {
            this->RegRead = exmo.cw.RegRead;
            this->RegWrite = exmo.cw.RegWrite;
            this->ALUSrc = exmo.cw.ALUSrc;
            this->ALUOp = exmo.cw.ALUOp;
            this->Branch = exmo.cw.Branch;
            this->Jump = exmo.cw.Jump;
            this->MemRead = exmo.cw.MemRead;
            this->MemWrite = exmo.cw.MemWrite;
            this->Mem2Reg = exmo.cw.Mem2Reg;
        }
    };

public:
    string rdl;
    int ALUOUT, LDOUT;
    CW cw;
};

//******************************************************************************************************************//


//****************************************** String to Int, Int to String ******************************************//

int to_int(string binaryStr)
{
    int decimalValue = 0;
    int length = binaryStr.length();
    bool isNegative = (binaryStr[0] == '1');

    if (!isNegative)
    {
        for (int i = 0; i < length; i++)
        {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1')
            {
                decimalValue += (1 << i);
            }
        }
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            char bit = binaryStr[length - 1 - i];

            if (bit == '1' && i != length - 1)
            {
                decimalValue += (1 << i);
            }
        }

        decimalValue -= (1 << (length - 1));
    }

    return decimalValue;
}

string to_str(int num)
{
    bitset<32> binary(num);
    return binary.to_string();
}

int SignedExtend(string imm)
{
    string extended_imm = imm;
    if (imm[0] == '0')
    {
        extended_imm = "000000000000" + extended_imm;
    }
    else
    {
        extended_imm = "111111111111" + extended_imm;
    }

    return to_int(extended_imm);
}

//********************************************************************************************************************//


//************************************************* Hazard Detector **************************************************//

void detectHazard(string cW_s3, string rd_s4, string rs1_s3, string rs2_s3)
{
    // R-Type and B-Type
    if (cW_s3 == "0110011" || cW_s3 == "1100011")
    {
        if (rd_s4 == rs1_s3 || rd_s4 == rs2_s3)
        {
            data_bubble = true;
            inUse[stoi(rd_s4, NULL, 2)] = 1;
            cout << "data bubble inserted due to R/B type" << endl;
        }
    }

    // I-Type and Load-Type
    if (cW_s3 == "0010011" || cW_s3 == "0000011")
    {
        if (rd_s4 == rs1_s3)
        {
            data_bubble = true;
            inUse[stoi(rd_s4, NULL, 2)] = 1;
            cout << "data bubble inserted due to load/store type" << endl;
        }
    }

    // B-Type and J-Type
    if (cW_s3 == "1100011" || cW_s3 == "1101111")
    {
        control_bubble= true;
        cout << "control bubble inserted due to J/B type" << endl;
    }
}

//***************************************************************************************************************//


//********************************************** Instruction Fetch **********************************************//

int IF(IFID &ifid)
{
    if (data_bubble || control_bubble)
    {
        cout << "IF stopped due to bubble insertion" << endl;
        return 0;
    }

    if (PC < N * 4)
    {
        int idx = PC / 4;
        ifid.IR = IM[idx];
        ifid.DPC = PC;
        PC = PC + 4;
        valid.ifid = true;
    }
    else
    {
        valid.PC = false;
    }

    return 1;
}

//****************************************************************************************************************//


//********************************************** Instruction Decode **********************************************//

int ID(IDEX &idex, IFID &ifid, EXMO &exmo)
{
    if (!valid.PC)
    {
        valid.ifid = false;
        cout << "Stopped due to invalid PC" << endl;
        return 0;
    }

    if (stopOnce)
    {
        stopOnce = false;
        return 0;
    }

    if (data_bubble || control_bubble)
    {
        cout << "ID stopped due to bubble insertion" << endl;
        return 0;
    }

    string ir = ifid.IR;
    idex.JPC = ifid.DPC + 4 * SignedExtend(ir.substr(0, 20));
    idex.DPC = ifid.DPC;
    idex.imm1 = ir.substr(0, 12);
    idex.imm2 = ir.substr(0, 7) + ir.substr(20, 5);
    idex.func = ir.substr(17, 3);
    idex.rdl = ir.substr(20, 5);
    idex.cw.controller(ir.substr(25, 7));
    idex.IR = ifid.IR;

    valid.idex = true;

    detectHazard(idex.IR.substr(25, 7), exmo.rdl, ir.substr(12, 5), ir.substr(7, 5));

    return 1;
}

//*************************************************************************************************//


//********************************************** ALU **********************************************//

string ALUControl(int ALUOp, string func, string func7)
{
    string ALUSelect;

    switch (ALUOp)
    {
    case 00:                    
        ALUSelect = "0010";         // Load/Store (for I-type, S-type), Perform ADD for address calculation
        break;
    case 01:                    
        if (func == "000")
        {                       
            ALUSelect = "0110";     // beq -> Perform SUB for comparison
        }
        else if (func == "001")
        {                       
            ALUSelect = "0110";     // bne -> SUB for comparison
        }
        else
        {
            ALUSelect = "1111";     // Undefined
        }
        break;
    case 10: 
        if (func == "000")
        {                                                     
            if (func7 == "0000000") 
            {
                ALUSelect = "0010"; // ADD
            }
            else 
            {
                ALUSelect = "0110"; // SUB
            }
        }
        else if (func == "001")
        {                       
            ALUSelect = "0011";     // SLL
        }
        else if (func == "111")
        { 
            ALUSelect = "0000";     // AND
        }
        else if (func == "110")
        { 
            ALUSelect = "0001";     // OR
        }
        else
        {
            ALUSelect = "1111";     // Undefined
        }
        break;
    case 11: 
        if (func == "000")
        {                       
            ALUSelect = "0010";     // ADDI
        }
        else if (func == "010")
        {                       
            ALUSelect = "0111";     // SLTI
        }
        else if (func == "100")
        {                       
            ALUSelect = "0011";     // XORI
        }
        else
        {
            ALUSelect = "1111";     // Undefined
        }
        break;
    default:
        ALUSelect = "1111";         // Undefined operation
        break;
    }

    return ALUSelect;
}

int ALU(string ALUSelect, string rs1, string rs2)
{
    int operand1 = stoi(rs1, NULL, 2);
    int operand2 = stoi(rs2, NULL, 2);
    int result = 0;

    if (ALUSelect == "0000")                      
    { 
        result = operand1 & operand2;   // AND
    }
    else if (ALUSelect == "0001")
    {       
        result = operand1 | operand2;   // OR
    }
    else if (ALUSelect == "0010")
    { 
        result = operand1 + operand2;   // ADD
    }
    else if (ALUSelect == "0110")
    { 
        result = operand1 - operand2;   // SUB
    }
    else if (ALUSelect == "0011")
    {                                           
        result = operand1 << (operand2 & 0x1F); // SLL
    }
    else if (ALUSelect == "0100")
    {                                           
        result = operand1 >> (operand2 & 0x1F); // SRL
    }
    else if (ALUSelect == "0101")
    {                                           
        result = operand1 >> (operand2 & 0x1F); // SRA
    }
    else if (ALUSelect == "0111")
    { 
        result = (operand1 < operand2) ? 1 : 0; // SLT
    }
    else if (ALUSelect == "1000")
    { 
        result = ((unsigned int)operand1 < (unsigned int)operand2) ? 1 : 0; // SLTU 
    }
    else if (ALUSelect == "1111")
    {
        std::cerr << "Error: Undefined ALU operation." << std::endl;
        return -1; 
    }

    return result;
}

//*****************************************************************************************************************//


//********************************************** Instruction Execute **********************************************//

int IE(EXMO &exmo, IDEX &idex)
{
    if (!valid.ifid)
    {
        cout << "Stopped due to invalid IFID" << endl;
        valid.idex = false;
        return 0;
    }

    if (data_bubble)
    {
        cout << "IE stopped due to bubble insertion" << endl;
        return 0;
    }

    if (stopOnce)
    {
        cout << "Stopped for one time" << endl;
        return 0;
    }

    string ir = idex.IR;

    if (idex.cw.RegRead)
    {
        cout << "rsl1 -> ir.substr(12, 5)" << " " << ir.substr(12, 5) << endl;
        cout << "GPR[to_int(rsl1)]" << " " << GPR[to_int(ir.substr(12, 5))] << endl;
        idex.rs1 = to_str(GPR[to_int(ir.substr(12, 5))]);
    }
    
    // For load or store
    if (idex.cw.ALUSrc && (ir.substr(25, 7) == "0010011" || ir.substr(25, 7) == "0000011"))
    {
        if (idex.cw.RegRead)
        {
            cout << "idex.imm1" << " " << idex.imm1 << endl;
            idex.rs2 = idex.imm1;
        }
    }
    else
    {
        if (idex.cw.RegRead)
        {
            cout << "rsl2 -> ir.substr(7, 5)" << " " << ir.substr(7, 5) << endl;
            cout << "GPR[to_int(rsl2)]" << " " << GPR[to_int(ir.substr(7, 5))] << endl;
            idex.rs2 = to_str(GPR[to_int(ir.substr(7, 5))]);
        }
    }

    string ALUSelect = ALUControl(idex.cw.ALUOp, idex.func, idex.IR.substr(0, 7));

    string opcode = idex.IR.substr(25, 7);
    if (opcode == "0100011" || opcode == "1100011")
    {
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.imm2);
    }
    else
    {
        exmo.ALUOUT = ALU(ALUSelect, idex.rs1, idex.rs2);
    }
    int ALUZeroFlag = (idex.rs1 == idex.rs2);
    exmo.cw.copyCW(idex);

    if (idex.cw.Branch)
    {
        if (ALUZeroFlag)
        {
            cout << "BPC = to_int(idex.imm2)*4 + idex.DPC " << to_int(idex.imm2) * 4 + idex.DPC << endl;
            PC = to_int(idex.imm2) * 4 + idex.DPC;
        }

        if (control_bubble)
        {
            stopOnce = true;
        }
            
        control_bubble= false;
    }

    if (idex.cw.Jump)
    {
        PC = idex.JPC;
        
        if (control_bubble)
        {
            stopOnce = true;
        }
            
        control_bubble= false;
    }

    exmo.rdl = idex.rdl;
    exmo.rs2 = idex.rs2;

    valid.exmo = true;

    return 1;
}

//**************************************************************************************************************//


//********************************************** Memory Operation **********************************************//

int MO(MOWB &mowb, EXMO &exmo)
{
    if (!valid.idex)
    {
        cout << "Stopped due to invalid IDEX" << endl;
        valid.exmo = false;
        return 0;
    }

    cout << "exmo.ALUOUT " << exmo.ALUOUT << endl;
    if (exmo.cw.MemWrite)
    {
        DM[exmo.ALUOUT] = to_int(exmo.rs2);
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }
    if (exmo.cw.MemRead)
    {
        mowb.LDOUT = DM[exmo.ALUOUT];
        cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
    }

    mowb.ALUOUT = exmo.ALUOUT;
    mowb.cw.copyCW(exmo);
    mowb.rdl = exmo.rdl;

    valid.mowb = true;

    return 1;
}

//********************************************************************************************************//


//********************************************** Write Back **********************************************//


int WB(MOWB &mowb)
{
    if (!valid.exmo)
    {
        cout << "Stopped due to invalid EXMO" << endl;
        valid.mowb = false;
        return 0;
    }

    bool shouldUnlock = false;

    if (mowb.cw.RegWrite)
    {
        if (mowb.cw.Mem2Reg)
        {
            GPR[to_int(mowb.rdl)] = mowb.LDOUT;
            cout << "GPR[to_int(mowb.rdl)] " << GPR[to_int(mowb.rdl)] << endl;

            if (inUse[stoi(mowb.rdl, NULL, 2)] == 1)
            {
                shouldUnlock = true;
            }
        }
        else
        {
            GPR[to_int(mowb.rdl)] = mowb.ALUOUT;
            cout << "GPR[to_int(mowb.rdl)] " << GPR[to_int(mowb.rdl)] << endl;

            if (inUse[stoi(mowb.rdl, NULL, 2)] == 1)
            {
                shouldUnlock = true;
            }
        }
    }

    if (shouldUnlock)
    {
        data_bubble = false;
        stopOnce = true;
        inUse[stoi(mowb.rdl, NULL, 2)] = 0;
    }

    return 1;
}

//***************************************************** CPU ENDS *****************************************************//

void printGPR()
{
    cout << "GPR values after the whole execution" << endl;
    for(int i = 0; i < 32; i++)
    {
        if(i <= 9){
            cout << "GPR[" << i << "]     " << GPR[i] << endl;
        }
        else 
        {
            cout << "GPR[" << i << "]    " << GPR[i] << endl;
        }
    }
}

//____________________________________________________________________________________________________________________//

//*************************************************** MAIN FUNCTION **************************************************//
//____________________________________________________________________________________________________________________//

int main() 
{
    //********************************************** CPU -> Machine Code to Output **********************************************//
    
    //********************* Initializations *********************//

    vector<string> machineCode = {
        "00000000101000001000000010010011",
        "00000000000000001000011011100011",
        "00000000000100011000000110010011",
        "00000000001100001000010111100011",
        "00000000000100010000000100010011",
        "00000000000100100000001000010011",
        "00000000000000101000001010010011",
        "00000000000100010000001101100011",
        "00000000010100100000000110110011",
        "00000000000000100000001010110011",
        "00000000000000011000001000110011",
        "00000000000100010000000100010011",
        "11111111111111111011001101101111",
        "00000000001100000010000010100011"
    };

    IFID ifid;
    IDEX idex;
    EXMO exmo;
    MOWB mowb;

    IM = machineCode;
    PC = 0;
    initControlUnit();
    initDM();
    int n = machineCode.size();
    N = n;

    //***********************************************************//

    //**************** Simulation of the Pipeline****************//

    int i = 0;

    while (PC < n * 4 || valid.PC || valid.ifid || valid.idex || valid.exmo || valid.mowb)
    {
        cout << endl;
        if(i <= 9){
            cout << "***********************Cycle " << i++ << " Starts***********************" << endl;
        }
        else if(i <= 99){
            cout << "***********************Cycle " << i++ << " Starts**********************" << endl;
        }
        else {
            cout << "**********************Cycle " << i++ << " Starts**********************" << endl;
        }

        cout << endl;
        if (valid.mowb)
        {   
            cout << endl;
            cout << "--------------------WriteBack (Stage-5)---------------------" << endl;
            if(WB(mowb))
            {
                cout << endl;
                cout << "DM[exmo.ALUOUT] " << DM[exmo.ALUOUT] << endl;
                cout << "to_int(mowb.rdl) " << to_int(mowb.rdl) << endl;
                cout << "GPR[to_int(mowb.rdl)] " << GPR[to_int(mowb.rdl)] << endl;
                cout << "PC " << PC << endl;
                cout << "GPR[0] " << GPR[0] << endl;
                cout << "GPR[1] " << GPR[1] << endl;
                cout << "GPR[2] " << GPR[2] << endl;
                cout << "GPR[3] " << GPR[3] << endl;
                cout << "GPR[4] " << GPR[4] << endl;
                cout << "GPR[5] " << GPR[5] << endl;
                cout << "DM[0] " << DM[0] << endl;
                cout << "DM[1] " << DM[1] << endl;
                cout << endl;
            }
            cout << "-----------------------WriteBack Ends-----------------------" << endl;
            cout << endl;
            cout << endl;
        }

        if (valid.exmo)
        {   
            cout << endl;
            cout << "-------------------Memory Operation (Stage-4)---------------" << endl;
            if(MO(mowb, exmo))
            {
                cout << endl;
                cout << "mowb.ALUOUT " << mowb.ALUOUT << endl;
                cout << "mowb.LDOUT " << mowb.LDOUT << endl;
                cout << "mowb.rdl " << mowb.rdl << endl;
                cout << endl;
            }
            cout << "---------------------Memory Operation Ends------------------" << endl;
            cout << endl;
            cout << endl;
        }

        if (valid.idex)
        {            
            cout << endl;
            cout << "------------------Instruction Execute (Stage-3)-------------" << endl;
            if(IE(exmo, idex))
            {
                cout << endl;
                cout << "ALUOUT " << exmo.ALUOUT << endl;
                cout << endl;
            }
            cout << "------------------Instruction Execution Ends----------------" << endl;
            cout << endl;
            cout << endl;
        }

        if (valid.ifid)
        {
            cout << endl;
            cout << "-----------------Instruction Decode (Stage-2)---------------" << endl;
            if(ID(idex, ifid, exmo))
            {
                cout << endl;
                cout << "imm1 " << idex.imm1 << endl;
                cout << "imm2 " << idex.imm2 << endl;
                cout << "func " << idex.func << endl;
                cout << "rdl " << idex.rdl << endl;
                cout << "rs1 " << idex.rs1 << endl;
                cout << "rs2 " << idex.rs2 << endl;
                cout << "JPC " << idex.JPC << endl;
                cout << "DPC " << idex.DPC << endl;
                cout << endl;
                cout << "-------------CONTROL WORD-------------" << endl;
                cout << "| RegRead   " << idex.cw.RegRead  << "                         |"  << endl;
                cout << "| RegWrite  " << idex.cw.RegWrite << "                         |"  << endl;
                cout << "| ALUSrc    " << idex.cw.ALUSrc   << "                         |"  << endl;

                if(idex.cw.ALUOp <= 9){
                    cout << "| ALUOp     0" << idex.cw.ALUOp    << "                        |"  << endl;
                }
                else {
                    cout << "| ALUOp     " << idex.cw.ALUOp    << "                        |"  << endl;
                }

                cout << "| Branch    " << idex.cw.Branch   << "                         |"  << endl;
                cout << "| Jump      " << idex.cw.Jump     << "                         |"  << endl;
                cout << "| MemRead   " << idex.cw.MemRead  << "                         |"  << endl;
                cout << "| MemWrite  " << idex.cw.MemWrite << "                         |"  << endl;
                cout << "| Mem2Reg   " << idex.cw.Mem2Reg  << "                         |"  << endl;
                cout << "--------------------------------------" << endl;
                cout << endl;
            }
            cout << "------------------Instruction Decode Ends-------------------" << endl;
            cout << endl;
            cout << endl;
        }

        if (valid.PC)
        {
            cout << endl;
            cout << "----------------Instruction Fetch (Stage-1)-----------------" << endl;
            cout << "Instruction: " << PC / 4 << endl;
            if(IF(ifid))
            {
                cout << endl;
                cout << "IR " << ifid.IR << endl;
                cout << "DPC " << ifid.DPC << endl;
                cout << "NPC " << ifid.NPC << endl;
                cout << endl;
            }
            cout << "-------------------Instruction Fetch Ends-------------------" << endl;
            cout << endl;
            cout << endl;
        }

        if(i <= 9){
            cout << "**********************Cycle " << i++ << " Finishes**********************" << endl;
        }
        else if(i <= 99){
            cout << "**********************Cycle " << i++ << " Finishes*********************" << endl;
        }
        else {
            cout << "*********************Cycle " << i++ << " Finishes*********************" << endl;
        }
        cout << endl;
        cout << endl;
    }

    printGPR();

    //***********************************************************//

    return 0;
    
//********************************************************************************************************************//

}

//*************************************************** MAIN FUNCTION ENDS *********************************************//


