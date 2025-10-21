#!/bin/bash

gcc ./chat_server.c \
../build/libsi_cnet.a ../../si_data/build/libsi_data.a ../../si_core/build/libsi_core.a ../../si_thread/build/libsi_thread.a \
-I../include -I../../si_data/include -I../../si_core/include -I../../si_thread/include \
-o chat_server -lm -lacl || exit 1

valgrind -s --leak-check=full ./chat_server
rm -f ./chat_server
