# FIT BETA, dati da fit_picchi.py
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import os

# -----------------------------
# CARICAMENTO DATI
# -----------------------------

os.chdir("..")
os.chdir("risultati_fit")

file_path = "parametri_fit.txt"
data = np.loadtxt(file_path, comments="#", skiprows=5)  # ATTENZIONE A SKIPROWS

L = data[:, 0]
beta_pc = data[:, 1]
err_beta = data[:, 2]


# -----------------------------
# MODELLO: beta = a + b * L^c
# -----------------------------
def f(x, a, b, c):
    return a + b * x ** (-1 / c)


p0 = [0.44, 0.1, -1]  # a = 0.44 (beta_c), c = -1 (1/nu)

# -----------------------------
# FIT PESATO
# -----------------------------
popt, pcov = curve_fit(
    f, L, beta_pc, sigma=err_beta, absolute_sigma=True, p0=p0, maxfev=10000
)

a, b, c = popt
a_err, b_err, c_err = np.sqrt(np.diag(pcov))

# -----------------------------
# CHI QUADRO RIDOTTO
# -----------------------------
res = beta_pc - f(L, *popt)
chi2 = np.sum((res / err_beta) ** 2)
ndf = len(L) - len(popt)
chi_red = chi2 / ndf

print(f"Chi quadro ridotto: {chi_red:.4f}")

# -----------------------------
# PARAMETRI DERIVATI
# -----------------------------
beta_c = a
err_beta_c = a_err

nu = -1 / c
err_nu = abs(c_err / (nu**2))

print("Parametri Ottimali:", popt)
print(f"beta_c = {beta_c} ± {err_beta_c}\n")
print(f"nu = {nu} ± {err_nu}\n")


out_file = "risultati_fit_betapc.txt"

with open(out_file, "w") as f1:
    f1.write("#beta_c\terr_beta_c\tnu\terr_nu\n")
    f1.write(f"{beta_c}\t{err_beta_c}\t{nu}\t{err_nu}\n")

# -----------------------------
# GRAFICO FIT
# -----------------------------
plt.figure(figsize=(10, 8))

plt.errorbar(L, beta_pc, yerr=err_beta, fmt="o", label="dati")

xfit = np.linspace(min(L), max(L), 200)
yfit = f(xfit, a, b, c)
plt.plot(xfit, yfit, "r-", lw=2, label=f"fit $\\chi^2_{{rid}} = {chi_red:.2f}$")

plt.title("Grafico β_pc vs L lattice")
plt.xlabel("L lattice")
plt.ylabel("β_pc")
plt.legend(loc="upper left")

plt.tight_layout()
plt.show()

# -----------------------------
# RESIDUI NORMALIZZATI
# -----------------------------
res_norm = res / err_beta

plt.figure(figsize=(10, 5))

plt.axhline(0, color="black", lw=1)
plt.plot(L, res_norm, "o")

plt.xlabel("L lattice")
plt.ylabel("Residui normalizzati")
plt.title("Residui del fit")

plt.tight_layout()
plt.show()
