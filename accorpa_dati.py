import os

# Input numero di L
N = int(input("Numero di L analizzati: "))
print()

# Input valori di L
vett_L = []
for y in range(N):
    L = int(input(f"Valore {y} di L: "))
    print()
    vett_L.append(L)

# Dati: L, chi_peak, err_chi, beta_pc, err_beta
dati = []

# Directory output/input
output_dir = "risultati_fit"
os.makedirs(output_dir, exist_ok=True)

# File finale
output_file = os.path.join(output_dir, "parametri_fit.txt")

with open(output_file, "w") as fp1:
    fp1.write("#L\tChi_peak\terr_chi\tbeta_pc\terr_beta\n")

    for L in vett_L:
        input_file = os.path.join(output_dir, f"parametri_fit_L{L}.txt")

        try:
            with open(input_file, "r") as fp:
                for line in fp:
                    line = line.strip()

                    if not line or line.startswith("#"):
                        continue

                    values = list(map(float, line.split()))
                    values = values[:5]

                    dati.append(values)
                    fp1.write("\t".join(map(str, values)) + "\n")

                    break

        except FileNotFoundError:
            print(f"File non trovato: {input_file}")
