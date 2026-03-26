set terminal wxt size 1024,768

print "Valore di L per plot:"
L = real(system("read L; echo $L"))


dati = sprintf('susc_L%d.txt',L)


stats dati using 1 name "Xstats"
stats dati using 2 name "Ystats"

set title sprintf("Grafico χ' vs β L=%d", L)
set xlabel 'β [J]'
set ylabel 'χ''
set key top left

dx = (Xstats_max - Xstats_min)*0.05
dy = (Ystats_max - Ystats_min)*0.15

set xrange [Xstats_min - dx : Xstats_max + dx]
set yrange [Ystats_min - dy : Ystats_max + dy]

plot dati using 1:2 pt 7 ps 1.2 title sprintf("dati L=%d", L)

pause -1
