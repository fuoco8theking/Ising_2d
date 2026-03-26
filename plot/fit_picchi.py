import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import sys
import os

# ---------------- INPUT ----------------
if len(sys.argv) > 3:
    L = int(sys.argv[1])
    inizio = int(sys.argv[2])
    fine = int(sys.argv[3])
else:
    L = int(input("Valore di L per fit: "))
    inizio = int(input("Numero dati da togliere all'inizio: "))
    fine = int(input("Numero dati da togliere alla fine: "))

nome_file_import = os.path.expanduser(f"~/analisi_mod1/L{L}.txt")

# ---------------- LETTURA DATI ----------------
dati = np.loadtxt(nome_file_import, skiprows=0)  # ATTENZIONE A SKIPROWS
N = len(dati)


start = inizio
end = N - fine

data_cut = dati[start:end]

x = data_cut[:, 0]
y = data_cut[:, 5]
yerr = data_cut[:, 6]

# ---------------- STATISTICHE ----------------
X_min, X_max = np.min(x), np.max(x)
Y_min, Y_max = np.min(y), np.max(y)

dx = (X_max - X_min) * 0.15
dy = (Y_max - Y_min) * 0.15


# ---------------- MODELLO ----------------
def f(x, a, b, c):  # a= Ypeak c= Xpeak
    return a + b * (x - c) ** 2


# Parametri iniziali
a0 = np.max(y)
c0 = x[np.argmax(y)]
b0 = -2e5
# ---------------- FIT ----------------
popt, pcov = curve_fit(
    f, x, y, sigma=yerr, p0=[a0, b0, c0], absolute_sigma=True, maxfev=8000
)

a, b, c = popt
a_err, b_err, c_err = np.sqrt(np.diag(pcov))

# ---------------- CHI QUADRO ----------------
residui = (y - f(x, *popt)) / yerr

chi2 = np.sum(residui**2)
dof = len(x) - len(popt)
chi2n = chi2 / dof
var_chi = np.sqrt(2 * dof)

# ---------------- OUTPUT FILE ----------------
output_dir = "../risultati_fit"
file_path = os.path.join(output_dir, f"parametri_fit_L{L}.txt")

with open(file_path, "w") as f_out:
    f_out.write("#L Chi_peak err_chi Beta_pc err_beta chi2n chi2 var_chi2 dof\n")
    f_out.write(f"{L} {a} {a_err} {c} {c_err} {chi2n} {chi2} {var_chi} {dof}\n")

# ---------------- PRINT ----------------
print(f"""
L={L}

chi2 ridotto = {chi2n:.2f}
chi2 = {chi2:.2f} +/- {var_chi:.2f}
dof = {dof}

y_peak = {a} +/- {a_err}
x_peak = {c} +/- {c_err}
parametro_b = {b}
""")

# ---------------- FIGURE COMBINATE ----------------
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10.24, 7.68))

x_fit = np.linspace(X_min, X_max, 500)

# ---------------- GRAFICO FIT ----------------
ax1.errorbar(x, y, yerr=yerr, fmt="o", label=f"dati L={L}")
ax1.plot(
    x_fit, f(x_fit, *popt), "r-", label=rf"Fit finale ($\chi^2_{{rid}} = {chi2n:.2f}$)"
)

ax1.set_title(f"Fit χ' vs β L={L}")
ax1.set_xlabel("β [J]")
ax1.set_ylabel("χ'")

ax1.set_xlim(X_min - dx, X_max + dx)
ax1.set_ylim(Y_min - dy, Y_max + dy)

ax1.legend(loc="upper left")

# ---------------- RESIDUI ----------------
res_norm = (y - f(x, *popt)) / yerr

R_min, R_max = np.min(res_norm), np.max(res_norm)
dy_res = (R_max - R_min) * 0.5

ax2.plot(x, res_norm, "o")
ax2.axhline(0, color="black")

ax2.set_xlabel("β [J]")
ax2.set_ylabel("Residui normalizzati")
ax2.set_ylim(R_min - dy_res, R_max + dy_res)

ax2.set_title("Residui")

plt.tight_layout()
plt.show()
