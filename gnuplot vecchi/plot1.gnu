set terminal wxt size 1024,768

print "Valore di L per fit:"
L = real(system("read L; echo $L"))


dati = sprintf('analisi_suscettivita/susc_L%d.txt',L)

N = system(sprintf("wc -l < %s", dati))
print "Numero dati:"
print N

print "Numero dati da togliere all'inizio:"
inizio = real(system("read inizio; echo $inizio"))
start=inizio

print "Nunero dati da togliere alla fine:"
fine= real(system("read fine; echo $fine"))

end=N-fine-1

stats dati using 1 every ::start::end name "Xstats" 
stats dati using 2 every ::start::end name "Ystats" 

set title sprintf("Grafico χ' vs β L=%d", L)
set xlabel 'β [J]'
set ylabel 'χ''
set key top left

dx = (Xstats_max - Xstats_min)*0.05
dy = (Ystats_max - Ystats_min)*0.15

set xrange [Xstats_min - dx : Xstats_max + dx]
set yrange [Ystats_min - dy : Ystats_max + dy]

a0 = 140
b0 = -10e5
c0 = 0.437

f0(x) = a0 + b0 * ((x - c0)**2)

a = a0
b = b0
c = c0
f(x) = a + b * ((x - c)**2)


fit f(x) dati using 1:2:3 every ::start::end via a,b,c

chin=FIT_WSSR / FIT_NDF
chi= FIT_WSSR
dof=FIT_NDF
var_chi= sqrt(2*dof)


filename = sprintf("risultati_fit/parametri_fit_L%d.txt", L)
set print filename
#print "#L	Chi_peak	err_chi		Beta_pc		err_beta"
print L,a, a_err, c ,c_err
set print 



print sprintf("\n\n\nL=%d \n\ nchi2n = %.2f  \n chi2=%.2f +/- %.2f \n dof=%d\n y_{pk}= %f +/- %f \n x_{pk} = %f +/- %f\n\n\n\n\n",L,chin,chi,var_chi,dof,a, a_err, c ,c_err)


filename = sprintf("risultati_fit/statistica_fit_L%d.txt", L)
set print filename
print "#L	Chi_peak	err_chi		Beta_pc		err_beta 	chi2n	chi2	var_chi2	dof"
print L,a, a_err, c ,c_err,chin,chi,var_chi,dof
set print 



set multiplot layout 2,1 title "Fit e residui"

plot \
    dati using 1:2:3 every ::start::end with yerrorbars pt 7 ps 1.2 title sprintf("dati L=%d", L), \
    f(x) title 'Fit finale' lc rgb 'red' lw 2 , \
    NaN title sprintf("χ²_{rid}: %.2f", chin)
     #f0(x) title 'f(x) iniziale' lc rgb 'green' lw 2
    
    

# Calcolo residui e salvo in un file temporaneo
set table 'risultati_fit/residui.tmp'
	plot dati every ::start::end using 1:(($2 - f($1)) / $3) notitle #residui normalizzati
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
set xlabel 'Beta [J]'
set ylabel 'Residui normalizzati'

# Plot residui con linea zero
plot \
    'risultati_fit/residui.tmp' using 1:2 with points pt 7 ps 1.2 title '', \
    0 lc rgb 'black' lw 1 title ''

unset multiplot


pause -1







