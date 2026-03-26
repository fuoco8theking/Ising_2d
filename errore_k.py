# ANALISI ERRORE IN FUNZIONE DI K PER MODULO I
import os
import subprocess
import logging
import sys
# from datetime import datetime
# import time

## per vedere i risultati eseguire poi plot_k.py

# File di log
log_file = "exe_errore_k.log"

# verifica se è stato passato un argomento
if len(sys.argv) < 6:
    print(
        "Errore: devi inserire L n termalizzazione k_max dk, esempio: python3 errore_k.py 40 10 10000 1000 10"
        "Ricordarsi prima di compilare con ./comp_analisi.sh"
    )
    sys.exit(1)  # Esci con codice errore 1

L = int(sys.argv[1])  # Nt va inserito da terminale come primo argomento
n = float(sys.argv[2])  # beta va inserito da terminale come secondo argomento
term = int(sys.argv[3])
k_max = int(sys.argv[4])
dk = int(sys.argv[5])

# Variabili
k_min = 1
errore_k = 1
nome_file_export = os.path.expanduser("~/analisi_mod1/errore_k")  ## SISTEMARE

with open(f"{nome_file_export}/L{L}.txt", "w") as f_export:
    f_export.write(
        f"# Inizio ciclo errore su k, con parametri: L = {L}, n = {n}, term = {term}, k_max = {k_max}, dk = {dk}\n"
    )

# Creazione della cartella se non esiste
cartella_analisi = os.path.expanduser("~/analisi_mod1")
os.makedirs(cartella_analisi, exist_ok=True)

# Imposta il logging
logging.basicConfig(
    filename=log_file, level=logging.INFO, format="%(asctime)s - %(message)s"
)


def errore(msg, iterazione):
    logging.error(f"Errore: {msg} - Iterazione: {iterazione}")
    print(f"Errore: {msg}")
    exit(1)


# Inizia a scrivere nel log
logging.info("Esecuzione script analisi iniziata")


# Ciclo per eseguire l'analisi
for k in range(k_min, k_max, dk):
    # Dati variabili per questa iterazione: k
    nome_file_import = os.path.expanduser("~/generazione_mod1")  # File da analizzare

    # Log della configurazione
    logging.info(f"Esecuzione iterazione {k} con: n={n}, L={L}")
    # logging.info(f"nome_file_import = {nome_file_import}")
    # logging.info(f"nome_file_export = {nome_file_export}")

    # Esegui il programma con i dati variabili
    try:
        result = subprocess.run(
            [
                "./analisi",
                str(L),
                str(term),
                str(k),
                nome_file_import,
                nome_file_export,
                str(errore_k),
                str(n),
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
        )
        logging.info(result.stdout.decode())
    except subprocess.CalledProcessError as e:
        errore(f"Il programma ha fallito nell'iterazione {k}", k)

    # Pausa tra le iterazioni (opzionale)
    # time.sleep(1)

logging.info("Fine esecuzione script analisi")
