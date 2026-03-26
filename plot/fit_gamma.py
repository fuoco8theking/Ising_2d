import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import os

# -----------------------------

# CARICAMENTO DATI

# -----------------------------

os.chdir("..")
os.chdir("risultati_fit")
dati = "parametri_fit.txt"
data = np.loadtxt(dati, comments="#")

N = len(data)

inizio = int(input("Numero dati da togliere all'inizio: "))
fine = int(input("Numero dati da togliere alla fine: "))

start = inizio
end = N - fine

data_cut = data[start:end]

L = data_cut[:, 0]
X_peak = data_cut[:, 1]
err_X = data_cut[:, 2]

# -----------------------------

# MODELLO: f(x) = a * x^b

# -----------------------------

def f(x, a, b):
	return a * x**b

# guess iniziali

p0 = [1, 1]

# -----------------------------

# FIT PESATO

# -----------------------------

popt, pcov = curve_fit(
f,
L,
X_peak,
sigma=err_X,
absolute_sigma=True,
p0=p0,
maxfev=10000
)

a, b = popt
a_err, b_err = np.sqrt(np.diag(pcov))

# -----------------------------

# CHI QUADRO RIDOTTO

# -----------------------------

res = X_peak - f(L, *popt)
chi2 = np.sum((res / err_X) ** 2)
ndf = len(L) - len(popt)
chi_red = chi2 / ndf

print(f"Chi quadro ridotto: {chi_red:.4f}")

# -----------------------------

# LETTURA nu DA FILE

# -----------------------------

file_nu = "risultati_fit_betapc.txt"

with open(file_nu, "r") as infile:
	lines = infile.readlines()
	valori = lines[1].split()
	nu = float(valori[2])
	err_nu = float(valori[3])

# -----------------------------

# CALCOLO gamma

# -----------------------------

gamma = b * nu
err_gamma = gamma * np.sqrt((err_nu / nu)**2 + (b_err / b)**2)

print(f"gamma = {gamma} ± {err_gamma}")

# -----------------------------

# SALVATAGGIO RISULTATI

# -----------------------------

out_file = "risultati_fit_chipeak.txt"

with open(out_file, "w") as outfile:
	outfile.write("#gamma\terr_gamma\n")
	outfile.write(f"{gamma}\t{err_gamma}\n")

# -----------------------------

# GRAFICO FIT + RESIDUI

# -----------------------------

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10), sharex=True)

# FIT

xfit = np.linspace(min(L), max(L), 200)

ax1.errorbar(L, X_peak, yerr=err_X, fmt='o', label="dati")
ax1.plot(xfit, f(xfit, a, b), 'r-', lw=2, label=f"fit $\chi^2_{{rid}} = {chi_red:.2f}$")

ax1.set_title("Fit X_peak vs L lattice")
ax1.set_ylabel("X_peak")
ax1.legend(loc="upper left")


# -----------------------------

# RESIDUI NORMALIZZATI

# -----------------------------

res_norm = res / err_X

ax2.axhline(0, color='black', lw=1)
ax2.plot(L, res_norm, 'o')

ax2.set_xlabel("L lattice")
ax2.set_ylabel("Residui normalizzati")
ax2.set_title("Residui")

plt.tight_layout()
plt.show()
