#!/bin/bash
# One-Liner that compiles bind.c with the user being prompted for the port number.
read -p 'Desired port number[Default 6969]: ' p && echo User entered: $p; case $p in ''|*[!0-9]*) cat ./bind.c | gcc -xc -o bind - && echo connect via: netcat 127.0.0.1 6969 ;;  *) cat ./bind.c | sed "s/DEFAULT_PORT\s6969/DEFAULT_PORT $p/" | gcc -xc -o bind - && echo connect via: netcat 127.0.0.1 $p ;; esac
