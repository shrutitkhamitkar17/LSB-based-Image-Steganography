# 🖼️ Image Steganography in C

A C-based image steganography application that hides and extracts secret data inside **BMP images** using the **Least Significant Bit (LSB)** technique.

## 📌 Overview

Steganography is the technique of concealing information within another medium so that the presence of the hidden information is not obvious.

This project implements LSB-based image steganography to encode secret data into a BMP image and retrieve it later through a command-line interface.

## ✨ Features

- Encode secret text or file data into a BMP image.
- Decode and extract hidden data from an encoded BMP image.
- Use the Least Significant Bit (LSB) technique for data embedding.
- Support 24-bit BMP images.
- Validate image capacity before encoding.
- Perform encoding and decoding through command-line arguments.
- Organize the application into modular C source and header files.

## 🛠️ Technologies Used

- C Programming
- File Handling (File I/O)
- Bitwise Operations
- Pointers and Structures
- Command-Line Arguments
- GCC Compiler
- Linux

## 📂 Project Structure

```text
.
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── common.h
├── types.h
├── beautiful.bmp
├── steged_img.bmp
├── secret.txt
└── README.md
```

## 🚀 Usage

### Encode Secret Data

```bash
./steganography -e beautiful.bmp secret.txt output.bmp
```

- `-e`: Encoding mode
- `beautiful.bmp`: Source BMP image
- `secret.txt`: File containing the secret message
- `output.bmp`: Generated image containing the hidden data

### Decode Secret Data

```bash
./steganography -d output.bmp decoded.txt
```

- `-d`: Decoding mode
- `output.bmp`: Encoded BMP image
- `decoded.txt`: Destination for the extracted data

*Note: These commands assume your program accepts the argument format shown above.*

## 🔍 Working Principle

### Encoding Process

1. Read the source BMP image and secret file.
2. Validate the input files and available image capacity.
3. Embed the secret data into the image's pixel bytes by modifying their least significant bits.
4. Copy the remaining image data as required by the implementation.
5. Generate the output stego-image.

### Decoding Process

1. Open the encoded BMP image.
2. Extract the embedded bits from the image data.
3. Reconstruct the original secret data.
4. Write the recovered data to the output file.

## 📷 Sample Output

### Encoding

```text
Encoding Successful!
Output Image: output.bmp
```

### Decoding

```text
Decoding Successful!
Recovered File: decoded.txt
```

*The output shown above is illustrative. Replace it with actual output from your program if it differs.*

## 📚 Concepts Used

- Least Significant Bit (LSB) Steganography
- Bitwise Manipulation
- Binary File I/O
- Pointers and Structures
- Memory Management
- Command-Line Argument Processing
- Modular Programming

## 🎯 Learning Outcomes

Through this project, I gained practical experience in:

- Implementing bit-level data manipulation in C.
- Reading and writing binary files.
- Developing command-line applications.
- Understanding the fundamentals of image data processing.
- Organizing programs using multiple source and header files.
- Implementing data validation and encoding/decoding workflows.

## 👩‍💻 Author

**Shruti Tushar Khamitkar**


## 📄 License

This project is intended for educational purposes. Add a specific open-source license if you intend to distribute the code under one.
