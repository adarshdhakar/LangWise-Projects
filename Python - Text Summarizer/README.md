# Text Summarization using Seq2Seq with Attention

This project implements a deep learning model to automatically generate summaries for user reviews. It uses an encoder-decoder (Seq2Seq) architecture with stacked LSTMs and an attention mechanism, built with TensorFlow and Keras.

## Project Overview

The goal of this project is to take a lengthy product review and produce a concise, relevant summary. This is a classic example of abstractive text summarization, where the model generates new sentences that capture the essence of the original text, rather than just extracting existing ones.

The model is trained on the "Amazon Fine Food Reviews" dataset.

### Key Features:

  * **Encoder-Decoder Architecture:** An encoder processes the input text and a decoder generates the summary.
  * **Stacked LSTMs:** The encoder uses a 3-layer stacked LSTM to capture complex patterns in the input sequence.
  * **Attention Mechanism:** An attention layer is included to help the decoder focus on the most relevant parts of the input review while generating each word of the summary.
  * **Comprehensive Preprocessing:** The text data is thoroughly cleaned by removing HTML tags, expanding contractions, removing stopwords, and performing stemming.

## Technologies & Libraries

  * **Python 3.x**
  * **TensorFlow & Keras:** For building and training the deep learning model.
  * **NLTK:** For natural language processing tasks like tokenization, stemming, and stopword removal.
  * **Pandas:** For data manipulation and reading the CSV file.
  * **NumPy:** For numerical operations.
  * **Scikit-learn:** For splitting the data into training and testing sets.
  * **BeautifulSoup:** For stripping HTML tags from the text data.

## Dataset

The model is trained on the [Amazon Fine Food Reviews](https://www.kaggle.com/datasets/snap/amazon-fine-food-reviews) dataset from Kaggle. The script processes the first 100,000 reviews from `Reviews.csv`.

  * `Text`: The body of the review (input to the model).
  * `Summary`: The user-written summary of the review (target for the model).

## ⚙️ How to Run the Project

### 1\. Prerequisites

Make sure you have Python installed. Clone this repository and install the required libraries.

```bash
# Clone the repository
git clone text-summarizer
cd text-summarizer

# Install dependencies
pip install -r requirements.txt
```

Create a `requirements.txt` file with the following content:

```
numpy
pandas
nltk
tensorflow
scikit-learn
beautifulsoup4
lxml
```

### 2\. Setup

  * **Dataset:** Download the `Reviews.csv` file from the Kaggle link above and place it in the root directory of the project.
  * **Contractions File:** You will need the `contractions.pkl` file, which contains a dictionary for expanding contractions (e.g., "don't" -\> "do not"). Ensure this file is in the same directory as the script.
  * **NLTK Data:** The script will automatically download the necessary NLTK packages (`wordnet`, `stopwords`, `punkt`) on its first run.

### 3\. Execution

To run the project, simply execute the main Python script:

```bash
python text_summarizer.py
```

**What happens when you run the script:**

1.  **Preprocessing:** The script loads and cleans 100,000 reviews.
2.  **Training:** It builds and trains the Seq2Seq model. The training process might take a significant amount of time depending on your hardware. The trained model will be saved in a directory named `s2s`.
3.  **Inference:** After training is complete, the script loads the saved model and prompts you to enter a review.
4.  **Prediction:** Enter your review in the terminal, and the model will generate a predicted summary.

### Example Usage:

```
Enter : This is the best coffee I have ever tasted. It has a rich, bold flavor without being bitter. I drink it every single morning and it is a great way to start my day. Highly recommended for any coffee lover!

Review : This is the best coffee I have ever tasted. It has a rich, bold flavor without being bitter. I drink it every single morning and it is a great way to start my day. Highly recommended for any coffee lover!

Predicted summary: best coffee ever
```

## 🧠 Model Architecture

The model consists of two main parts: an Encoder and a Decoder.

### Encoder

1.  **Input Layer:** Takes sequences of integers representing the review text.
2.  **Embedding Layer:** Converts the integer sequences into dense vector embeddings.
3.  **LSTM Layers:** A stack of three LSTMs processes the embedded sequence. The final hidden state ($h$) and cell state ($c$) of the last LSTM are passed to the decoder as its initial state. The full output sequence from the final LSTM layer is also passed to the attention mechanism.

### Decoder

1.  **Input Layer:** Takes the target summary sequences, shifted by one timestep.
2.  **Embedding Layer:** Converts the target integer sequences into dense vectors.
3.  **LSTM Layer:** Its initial state is set to the final state of the encoder. It processes the target sequence.
4.  **Attention Layer:** This layer computes a context vector by assigning attention scores to the encoder's outputs. It helps the decoder weigh the importance of different words in the input review.
5.  **Concatenate Layer:** The output from the decoder's LSTM and the context vector from the attention layer are concatenated.
6.  **Dense Layer:** A final dense layer with a `softmax` activation function produces a probability distribution over the entire target vocabulary to predict the next word.

The model is compiled with the `rmsprop` optimizer and `sparse_categorical_crossentropy` loss function.
