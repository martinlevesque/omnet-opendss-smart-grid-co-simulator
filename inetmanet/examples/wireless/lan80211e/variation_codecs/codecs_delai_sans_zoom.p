set term postscript eps enhanced
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set encoding iso_8859_15
set output "codecs_delai_sans_zoom.eps"
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title ""
set xlabel "Nombre de stations Voip"
set ylabel "Délai (en secondes)"

# set xr [0:0.002]
# set yr [0:0.025]

set size ratio 0.5

set key left top noinvert box spacing 3
set style line 1 lw 1 pt 4
set style line 2 lw 1 pt 6
set style line 3 lw 1 pt 8
set style line 4 lw 1 pt 3
plot    "G.711/voipStats.txt" using ($3):($8) ls 1 title 'G.711' with linespoints, \
        "G.729A/voipStats.txt" using ($3):($8) ls 2 title 'G.729A' with linespoints,\
        "G.723.1/voipStats.txt" using ($3):($8) ls 3 title 'G.723.1' with linespoints

