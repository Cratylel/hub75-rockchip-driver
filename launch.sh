#!/bin/bash
./main_x86_64 | tee >(python3 ./debug_decoder/test.py)