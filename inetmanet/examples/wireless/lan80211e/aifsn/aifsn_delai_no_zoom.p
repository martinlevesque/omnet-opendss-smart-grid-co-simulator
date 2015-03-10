set term postscript eps enhanced
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set encoding iso_8859_15
set output "aifsn_delai_no_zoom.eps"
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title ""
set xlabel "Nombre de stations Voip"
set ylabel "Délai (en secondes)"

# set xr [0:0.002]
# set yr [0:0.015]

set size ratio 0.5

set key left top noinvert box spacing 3
set style line 1 lw 1 pt 2
set style line 2 lw 1 pt 4
set style line 3 lw 1 pt 6
set style line 4 lw 1 pt 8
set style line 5 lw 1 pt 10
set style line 6 lw 1 pt 12
set style line 7 lw 1 pt 14
set style line 8 lw 1 pt 16
plot    "aifsn=0/voipStats.txt" using ($3):($8) ls 1 title 'Voip, AIFSN Voip = 0' with linespoints, \
        "aifsn=2/voipStats.txt" using ($3):($8) ls 2 title 'Voip, AIFSN Voip = 2' with linespoints, \
        "aifsn=4/voipStats.txt" using ($3):($8) ls 3 title 'Voip, AIFSN Voip = 4' with linespoints, \
        "aifsn=6/voipStats.txt" using ($3):($8) ls 4 title 'Voip, AIFSN Voip = 6' with linespoints, \
        "aifsn=0/bestEffortStats.txt" using ($3):($8) ls 5 title 'Best effort AIFSN Voip = 0' with linespoints, \
        "aifsn=2/bestEffortStats.txt" using ($3):($8) ls 6 title 'Best effort AIFSN Voip = 2' with linespoints,\
        "aifsn=4/bestEffortStats.txt" using ($3):($8) ls 7 title 'Best effort AIFSN Voip = 4' with linespoints,\
        "aifsn=6/bestEffortStats.txt" using ($3):($8) ls 8 title 'Best effort AIFSN Voip = 6' with linespoints

