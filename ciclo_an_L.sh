#!/bin/bash
# dati da inserire: L (auto), term, k, import, export, errore_k (=0 per analisi normale), b_errore_k (=0 per analisi normale)
# Ciclo da 10 a 180 con passi di 10
for i in {10..180..10}; do
  # Esegui il programma ./programma passando il valore corrente come input
  ./analisi "$i" 50000 200 ~/generazione_mod1 ~/analisi_mod1 0 0
done
