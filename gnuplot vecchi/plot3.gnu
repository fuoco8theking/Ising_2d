set terminal wxt size 1024,768

dati = sprintf('risultati_fit/parametri_fit.txt')

stats dati using 1 name "Xstats"
stats dati using 2 name "Ystats"

set title sprintf("Grafico X_{peak} vs L lattice")
set xlabel 'L lattice'
set ylabel 'X_{peak}'
set key top left

dx = (Xstats_max - Xstats_min)*0.05
dy = (Ystats_max - Ystats_min)*0.05

#set xrange [Xstats_min - dx : Xstats_max + dx]
#set yrange [Ystats_min - dy : Ystats_max + dy]

set autoscale 


a0=1
b0=1

f0(x)= a0*x**(b0)	#y=χ'_{peak} a=y0 b=gamma/nu

a=a0
b=b0


f(x)= a*x**b	#y=χ'_{peak} a=y0 b=gamma/nu


fit f(x) dati using 1:2:3 via a,b

chin= FIT_WSSR / FIT_NDF
print sprintf("Chi quadro ridotto: %.4f", chin)


nu_str = system("sed -n 2p risultati_fit/risultati_fit_betapc.txt | awk '{print $3}'")
nu = real(nu_str)


err_nu_str = system("sed -n 2p risultati_fit/risultati_fit_betapc.txt | awk '{print $4}'")
err_nu = real(err_nu_str)


gamma= b*nu
err_gamma= gamma*sqrt((err_nu/nu)**2+(b_err/b)**2)


file_output = sprintf("risultati_fit/risultati_fit_chipeak.txt")
set print file_output
print "#gamma	 err_gamma"
print gamma, err_gamma
set print 

set multiplot layout 2,1 title "Fit e residui"

plot \
    dati using 1:2:3 with yerrorbars pt 7 ps 1.2 title "dati", \
    f(x) title 'fit' lc rgb 'red' lw 2 , \
    NaN title sprintf("χ²_{rid}: %.2f", chin)
     #f0(x) title 'f(x) iniziale' lc rgb 'green' lw 2
     
# Calcolo residui e salvo in un file temporaneo
set table 'risultati_fit/residui.tmp'
	plot dati using 1:(($2 - f($1)) / $3) notitle #residui normalizzati
unset table


unset xrange
unset yrange


# Calcolo statistiche sui residui
stats 'risultati_fit/residui.tmp' using 2 name "Rstats"

# Calcolo margine
dy_res = (Rstats_max - Rstats_min)*0.5

# Set range in base ai residui

set yrange [Rstats_min - dy_res : Rstats_max + dy_res]

# Etichette assi
set xlabel 'L lattice'
set ylabel 'Residui normalizzati'

# Plot residui con linea zero
plot \
    'risultati_fit/residui.tmp' using 1:2 with points pt 7 ps 1.2 title '', \
    0 lc rgb 'black' lw 1 title ''

unset multiplot

pause -1







