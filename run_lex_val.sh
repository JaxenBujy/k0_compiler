#!/bin/bash
make clean
make
valgrind ./scanner test1.kt