# RISC-V Pipeline Simulator

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Running the Simulator](#running-the-simulator)
- [Project Structure](#project-structure)
- [Implementation Details](#implementation-details)
  - [Instruction Set](#instruction-set)
  - [Pipeline Stages](#pipeline-stages)
  - [Hazard Handling](#hazard-handling)
  - [Memory Organization](#memory-organization)
  - [Control Unit](#control-unit)
  - [ALU Operations](#alu-operations)
- [Output and Debugging](#output-and-debugging)
- [Limitations and Future Work](#limitations-and-future-work)

## Introduction
This project is a **RISC-V Pipeline Simulator** written in C++. It simulates the execution of RISC-V assembly instructions through a 5-stage pipelined processor model. The simulator features comprehensive hazard detection, pipeline stalling, and detailed cycle-by-cycle execution tracking.

## Features
- Complete five-stage pipelined processor implementation (IF, ID, EX, MEM, WB)
- Support for major RISC-V instruction types (R, I, S, B, U, J)
- Advanced hazard detection and handling:
  - Data hazards with pipeline stalling
  - Control hazards for branch and jump instructions
- Detailed cycle-by-cycle execution tracking
- Register file (32 general-purpose registers)
- 1024-word data memory
- Comprehensive ALU supporting multiple operations
- Pipeline state visualization for debugging

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/adarshdhakar/CPU_Design.git
    cd CPU_Design
    ```

2. Compile using g++ with C++11 support:
    ```bash
    g++ -o simulator CPU_with_Pipelined_Assembler.cpp -std=c++11
    ```

## Running the Simulator

1. Execute the compiled simulator:
    ```bash
    ./simulator
    ```

2. The simulator will:
   - Execute the predefined machine code instructions
   - Display detailed cycle-by-cycle pipeline state
   - Show register and memory updates
   - Print final register values after execution

## Project Structure
```
risc-v-pipeline-simulator/
├── CPU_Design.cpp                    # Main simulator implementation
├── AssemblyCode.txt                  # Assembly code for testing
├── MachineCode.txt                   # Binary machine code
├── CPU.cpp                           # CPU without Assembler and Pipeline
├── CPU_with_Pipeline.cpp             # CPU with Pipeline
├── CPU_with_Pipelined_Assembler.cpp  # CPU with Pipeline and Assembler
└── README.md           
```

## Implementation Details

### Instruction Set
Supported instruction types and their control signals:

- **R-type (0110011)**
  - Includes: ADD, SUB, AND, OR, SLL
  - Uses ALUOp 10 for operation selection
  
- **I-type (0010011, 0000011)**
  - Includes: ADDI, SLTI, XORI, LW
  - Uses ALUOp 11 for immediate operations
  
- **S-type (0100011)**
  - Includes: SW
  - Uses ALUOp 00 for address calculation
  
- **B-type (1100011)**
  - Includes: BEQ, BNE
  - Uses ALUOp 01 for branch comparison
  
- **U-type (0110111)**
  - Includes: LUI
  
- **J-type (1101111)**
  - Includes: JAL

### Pipeline Stages

1. **Instruction Fetch (IF)**
   - Fetches instruction from IM using PC
   - Updates PC for next instruction
   - Handles stalling conditions

2. **Instruction Decode (ID)**
   - Decodes instruction fields
   - Generates control signals
   - Reads register values
   - Calculates immediate values and jump addresses

3. **Execute (EX)**
   - Performs ALU operations
   - Handles branch resolution
   - Implements data forwarding logic

4. **Memory (MEM)**
   - Performs memory read/write operations
   - Manages data memory access

5. **Write Back (WB)**
   - Writes results back to register file
   - Handles register locking/unlocking for hazards

### Hazard Handling

The simulator implements sophisticated hazard detection and handling:

1. **Data Hazards**
   - Detection through register tracking
   - Pipeline stalling using data_bubble
   - Register locking mechanism using inUse bitset

2. **Control Hazards**
   - Branch and jump instruction handling
   - Control bubble insertion
   - PC update management

### Memory Organization

- **Instruction Memory (IM)**
  - Vector of 32-bit instruction strings
  - Accessed during IF stage

- **Data Memory (DM)**
  - 1024-word array (DM)
  - Word-addressable
  - Supports load/store operations

### Control Unit

The control unit generates 9 control signals:
- RegRead: Enable register read
- RegWrite: Enable register write
- ALUSrc: Select ALU second operand
- ALUOp: Select ALU operation
- Branch: Enable branch operation
- Jump: Enable jump operation
- MemRead: Enable memory read
- MemWrite: Enable memory write
- Mem2Reg: Select write-back data source

### ALU Operations

Supported ALU operations (4-bit control):
- 0000: AND
- 0001: OR
- 0010: ADD
- 0011: SLL (Shift Left Logical)
- 0110: SUB
- 0111: SLT (Set Less Than)

## Output and Debugging

The simulator provides detailed cycle-by-cycle output:
- Current pipeline stage states
- Register and memory updates
- Control signals
- ALU operations and results
- Hazard detection events
- Final register state

## Limitations and Future Work

1. Potential Enhancements:
   - Support for floating-point operations
   - Advanced branch prediction
   - Cache implementation
   - More comprehensive instruction set support
   - GUI interface for visualization

2. Current Limitations:
   - Basic hazard handling without forwarding
   - Limited memory size
   - No support for system calls or interrupts
