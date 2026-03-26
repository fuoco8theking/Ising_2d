## File Analisi Dati 

<!--toc:start-->
  - [File Analisi Dati](#file-analisi-dati)
    - [Come utilizzare il programma](#come-utilizzare-il-programma)
- [Struttura salvataggio dati analisi](#struttura-salvataggio-dati-analisi)
    - [Termalizzazione](#termalizzazione)
    - [Errore in funzione di k](#errore-in-funzione-di-k)
    - [Iterazioni](#iterazioni)
    - [Questioni aperte analisi dati](#questioni-aperte-analisi-dati)
<!--toc:end-->

Nella nuova versione, analisi dati esporta un singolo file di testo per ogni L, in formato:
`      // "# beta[0]   e[1]  sigma_e[2]   m[3]  sigma_m[4]  susc[5] sigma_susc[6]
      // cu[7] sigma_cu[8] cal[9] sigma_cal[10]
`

### Come utilizzare il programma
- Da terminale in cartella Progmod1 eseguire `./analisi.sh    L   term    k   root-file-import    root-file-export  errore_k  b_errore_k`
  - `L`: lunghezza lati del reticolo quadrato 
  - `term`: dati iniziali da saltare, di "termalizzazione" (consultare [[#Termalizzazione]])
  - `k`: lunghezza blocco nel jackknife (consultare [[#Errore in funzione di k]])
  - `root-file-import`: ATTENZIONE: bisogna riportare la cartella madre in cui verranno esportati i file:
    esempio: voglio che i file analizzati siano: `~/generazione_mod1/L40/n#.txt`, dunque inserisco: `~/analisi_mod1` (senza nemmeno `/` finale) (differisce dal caso del modulo III)
  - root-file-export: analogo al caso di sopra:
    esempio: voglio che il file venga salvato in:  `~/analisi_mod1/L40.txt`, dunque inserisco: `~/analisi_mod1`
  - `errore_k`: boolean:
    - `0`: analisi dati normale
    - `1` analisi dati per errore in funzione di k
  - `b_errore_k`: scegliere quale file analizzare
    esempio: inserisco 10: il file analizzato sarà `n10.txt`

Nel caso di problemi in esecuzione, consultare la seguente sezione: [[#Questioni aperte analisi dati]]

# Struttura salvataggio dati analisi


La struttura è stata resa simile al caso del modIII, la differenza principale è che qua il singolo file itera direttamente su tutti i beta. 
Il file può essere salvato nella directory che si preferisce (vedere [[[#Come utilizzare il programma]]).
I dati vengono esportati come file di testo della forma `L#.txt`, all'interno sono disposti in colonne come segue:
```
# beta[0]   e[1]  sigma_e[2]   m[3]  sigma_m[4]  susc[5] sigma_susc[6] cu[7] sigma_cu[8] cal[9] sigma_cal[10]
```

### Termalizzazione
Implementata in modo che sia possibile decidere numero di righe da saltare direttamente da terminale.
Per vedere quante righe è opportuno saltare, andare nella cartella dei file ad L fissato che si vuole analizzare e prendere l'ultimo file creato (quello con beta più alta). Vedere in che zona si "stabilizzano" energia e magnetizzazione (scegliendo una riga all'inizio del "plateau").
- Esempio pratico per capire quante righe saltare (da eseguire all'interno della cartella con L desiderato (in `generazione_dati/L#` ), selezionando # n a piacere):
```
	gnuplot  
	plot "n10.txt" skip 1 using 0:1
```

### Errore in funzione di k 
Per plottare:
`
Per il plot fatto bene c'è il programma pyton: `plot_k.py`
Per eseguire un plot veloce con gnuplot dal server, eseguire (dopo aver aperto `gnuplot`):
```
plot 'L10.txt' using ($0*10):3 with lines
```
- ATTENZIONE: con gnuplot le colonne iniziano da `1`, la `0` rappresenta l'indice di riga
- `*1` indica che nella generazione di `errore_k` è stato scelto `k` = 1, scegliere a seconda di errore_k 
- `:3` = sigma_e, `:5` = sigma_m, `:7` = sigma_susc, `:9` = sigma_cumulante, `:11` = sigma_calore_spec
