# Steganography
A C-based project to hide secret data into image and decode that data

## Project Overview
This project implements **image steganography** using the **C programming language**.
Steganography is the technique of hiding secret information inside a file without
visibly altering it. In this project, secret data is embedded into an image file
and can later be extracted securely.

---

## Objectives
- Hide secret data inside an image file
- Retrieve the hidden data without data loss
- Understand low-level file handling and bit manipulation in C

---

## Features
- Encode secret text into an image file
- Decode and extract hidden text from the image
- Works on bitmap (`.bmp`) image format
- Uses Least Significant Bit (LSB) technique
- Command-line based application

---

## Technologies Used
- C Programming
- File Handling
- Bit Manipulation
- Structures
- Header Files

---

## How to Compile and Run

### Compile
gcc *.c -o steganography

### Encode
./steganography -e beautiful.bmp secret.txt output.bmp

### Decode
./steganography -d output.bmp extracted.txt

