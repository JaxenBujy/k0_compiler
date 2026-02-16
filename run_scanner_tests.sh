#!/bin/bash
make clean
make
./scanner test_program1.kt >> scanner_output.txt
./scanner test_program2.kt >> scanner_output.txt
./scanner test_program3.kt >> scanner_output.txt
./scanner test_program4.kt >> scanner_output.txt
python3 int_code_counter.py
> scanner_output.txt