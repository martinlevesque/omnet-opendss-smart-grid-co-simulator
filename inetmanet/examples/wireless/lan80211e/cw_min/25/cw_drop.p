set term postscript eps enhanced
set   autoscale                        # scale axes automatically
set encoding iso_8859_15
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set output "cw_drop.eps"
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title ""
set xlabel "CW min"
set ylabel "\% paquets rejet�s"

# set xr [0:0.002]
# set yr [0:0.015]

set size ratio 0.5

set key top right noinvert box spacing 3
set style line 1 lw 1 pt 2
set style line 2 lw 1 pt 4
set style line 3 lw 1 pt 5
set style line 4 lw 1 pt 6
set style line 5 lw 1 pt 8
set style line 6 lw 1 pt 10
set style line 7 lw 1 pt 12
set style line 8 lw 1 pt 14
plot    "voipStats.txt" using ($12):($11) ls 1 title 'Voip, CW max = 16' with linespoints
