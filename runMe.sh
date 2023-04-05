#!/bin/bash
	make
    ./main
	cd CPU
    make
    echo "Your tree in the asmeq.asm\n"
    echo "Enter: asmeq.asm\n asmeq"
    ./run_coder
    echo "Enter: asmeq.bin\n 1000"
	./run_stack
    exit