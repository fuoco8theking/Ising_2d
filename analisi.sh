#!/bin/bash

#compilazione file analisi
gcc ising_analisi_dati.c -o analisi -lm -O3 -march=native -Wall -Wextra -Wconversion -fsanitize=address

#esecuzione analisi
echo "script.sh: ESECUZIONE analisi dati:"
./analisi
