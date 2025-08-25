#!/bin/bash
gcc ./example.c ../build/libsi_cnet.a ../../si_data/build/libsi_data.a ../../si_core/build/libsi_core.a -I../include -I../../si_data/include -I../../si_core/include -o example -lm
valgrind -s ./example
rm -f ./example
