# PLOT DATI DA ANALISI
# ultima modifica 23/03/26 0re 21:27 Andrea
# from typing import ChainMap
import numpy as np
import matplotlib.pyplot as plt
import os
import sys


if len(sys.argv) < 3:
    print(
        "Errore: devi inserire L e la variabile da plottare : python3 esempio.py 4 3 \n 1 = energia \n 2 = magnetizzazione \n 3 = suscettività \n 4 = cumulante \n 5 = calore specifico \n "
    )
    sys.exit(1)  # Esci con codice errore

L = int(sys.argv[1])
s = int(sys.argv[2])

nome_file_import = os.path.expanduser(f"~/analisi_mod1/L{L}.txt")
dati = np.loadtxt(nome_file_import, skiprows=0)  # ATTENZIONE A SKIPROWS
# Caricamento dati
x = dati[:, 0]

if s == 1:
    variabile = "energia"
    y = dati[:, 1]
    dy = dati[:, 2]

elif s == 2:
    variabile = "magnetizzazione"
    y = dati[:, 3]
    dy = dati[:, 4]

elif s == 3:
    variabile = "suscettività"
    y = dati[:, 5]
    dy = dati[:, 6]

elif s == 4:
    variabile = "cumulante"
    y = dati[:, 7]
    dy = dati[:, 8]

elif s == 5:
    variabile = "calore specifico"
    y = dati[:, 9]
    dy = dati[:, 10]

else:
    print("ERRORE: scelta variabile errata")
    y = dati[:, 0]
    dy = dati[:, 0]
    variabile = "hai scavolato numero"

print(f"Dati inseriti: L = {L}, variabile = {variabile}")

# margini
X_min, X_max = np.min(x), np.max(x)
Y_min, Y_max = np.min(y), np.max(y)

ddx = (X_max - X_min) * 0.05
ddy = (Y_max - Y_min) * 0.15

# Plot
plt.errorbar(x, y, yerr=dy, fmt=".", label=f"dati L={L}")

plt.title(f"Grafico {variabile} vs β L={L}")
plt.xlabel("β [J]")
plt.ylabel(f"{variabile}")

plt.xlim(X_min - ddx, X_max + ddx)
plt.ylim(Y_min - ddy, Y_max + ddy)

plt.legend(loc="upper left")

plt.show()
