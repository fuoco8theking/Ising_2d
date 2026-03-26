set terminal wxt size 1024,768

dati = sprintf('risultati_fit/parametri_fit.txt')

stats dati using 1 name "Xstats"
stats dati using 2 name "Ystats"

set title sprintf("Grafico β_{pc} vs L lattice")
set xlabel 'L lattice'
set ylabel 'β_{pc}'
set key top left

set autoscale 

a0=1
b0=1
c0=1

f0(x)= a0 + b0*x**c0 	#y=beta_pc a=beta_c b=x0 c=-1/eta

a=a0
b=b0
c=c0

f(x)= a + b*x**c 	#y=beta_pc a=beta_c b=x0 c=-1/eta


fit f(x) dati using 1:4:5 via a,b,c

chin=FIT_WSSR / FIT_NDF
print sprintf("Chi quadro ridotto: %.4f", chin)

beta_c =a
err_beta_c =a_err
eta=-1/c
err_eta= abs(c_err/(eta*eta))

filename = sprintf("risultati_fit/risultati_fit_betapc.txt")
set print filename
print "#beta_c 	err_beta_c	eta	err_eta"
print beta_c,err_beta_c,eta,err_eta
set print 

set multiplot layout 2,1 title "Fit e residui"

plot \
    dati using 1:4:5 with yerrorbars pt 7 ps 1.2 title "dati", \
    f(x) title 'fit' lc rgb 'red' lw 2 , \
    NaN title sprintf("χ²_{rid}: %.2f", chin)
     #f0(x) title 'f(x) iniziale' lc rgb 'green' lw 2
     
# Calcolo residui e salvo in un file temporaneo
set table 'risultati_fit/residui.tmp'
	plot dati using 1:(($4 - f($1)) / $5) notitle #residui normalizzati
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







     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
