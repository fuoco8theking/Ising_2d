# PLOT DATI DA ANALISI
# ultima modifica 23/03/26 0re 21:27 Andrea
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import glob

if len(sys.argv) < 2:
    print(
        "Errore: devi inserire la variabile da plottare:\n"
        "python3 esempio.py 3\n"
        "1 = energia\n"
        "2 = magnetizzazione\n"
        "3 = suscettività\n"
        "4 = cumulante\n"
        "5 = calore specifico\n"
    )
    sys.exit(1)

s = int(sys.argv[1])

# Cartella contenente i file
cartella = os.path.expanduser("~/analisi_mod1/per_plot_chic")

# Trova tutti i file L*.txt
lista_file = sorted(glob.glob(os.path.join(cartella, "L*.txt")))

if len(lista_file) == 0:
    print("Nessun file L*.txt trovato.")
    sys.exit(1)


# Funzione per selezionare colonne
def seleziona_colonne(s, dati):
    if s == 1:
        return "energia", dati[:, 1], dati[:, 2]
    elif s == 2:
        return "magnetizzazione", dati[:, 3], dati[:, 4]
    elif s == 3:
        return "suscettività", dati[:, 5], dati[:, 6]
    elif s == 4:
        return "cumulante", dati[:, 7], dati[:, 8]
    elif s == 5:
        return "calore specifico", dati[:, 9], dati[:, 10]
    else:
        print("ERRORE: scelta variabile errata")
        sys.exit(1)


X_all = []
Y_all = []

plt.figure()

for file_path in lista_file:
    try:
        dati = np.loadtxt(file_path)
    except Exception as e:
        print(f"Errore nel file {file_path}: {e}")
        continue

    x = dati[:, 0]
    variabile, y, dy = seleziona_colonne(s, dati)

    # Estrai L dal nome file (es: L16.txt → 16)
    nome_file = os.path.basename(file_path)
    L = nome_file.replace("L", "").replace(".txt", "")

    X_all.extend(x)
    Y_all.extend(y)

    plt.errorbar(x, y, yerr=dy, fmt=".", label=f"L = {L}")

if len(X_all) == 0:
    print("Nessun dato valido trovato.")
    sys.exit(1)

# Limiti globali
X_all = np.array(X_all)
Y_all = np.array(Y_all)

X_min, X_max = np.min(X_all), np.max(X_all)
Y_min, Y_max = np.min(Y_all), np.max(Y_all)

ddx = (X_max - X_min) * 0.05
ddy = (Y_max - Y_min) * 0.15

plt.title(f"Grafico {variabile} vs β")
plt.xlabel("β [J]")
plt.ylabel(f"{variabile}")

plt.xlim(X_min - ddx, X_max + ddx)
plt.ylim(Y_min - ddy, Y_max + ddy)

plt.legend()
plt.grid()

plt.show()
