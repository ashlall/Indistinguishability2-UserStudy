import os


def main():
    datfiles = [filename for filename in os.listdir() if filename[-3:] == "dat" and filename[:4] != "time"]

    for filename in datfiles:
        out = open("temp.p", "w")

        # version for alpha
        out.write("print '" + filename[:-15] + "'\n")
        out.write("set terminal pdf size 4,3 enhanced font 'Arial,14' butt\n")
        out.write("set size 1, 1\n")
        out.write("set output '" + filename[:-15] + ".pdf'\n")
        out.write("set key inside top right\n")
        if filename[:6] == "vary-T":
            out.write("set xlabel 'T'\n")
        elif filename[:6] == "vary-q":
            out.write("set xlabel 'q'\n")
        elif filename[:6] == "vary-s":
            out.write("set xlabel 's'\n")
        elif filename[:3] == "fix":
            out.write("set xlabel 's'\n")
        elif filename[:6] == "vary-e":   # for epsilon and epsilondelta
            out.write("set xlabel 'epsilon'\n")
        elif filename[:8] == "vary-dim" or filename.startswith("scalability-vary-d-alpha"):
            out.write("set xlabel 'd'\n")
        elif filename[:10] == "vary-delta":
            out.write("set xlabel 'delta'\n")
        elif filename[:6] == "vary-n" or filename.startswith("scalability-vary-n-alpha"):
            out.write("set logscale x\n")
            out.write("set xlabel 'n'\n")
        out.write("set ylabel 'alpha'\n")

        if filename[:8] == "vary-dim" or filename[:3] == "fix":
            out.write("set xtics 1\n")

        if filename[:6] == "vary-e" or filename[:10] == "vary-delta":
            out.write("set logscale x\n")
            out.write("set xrange [0.001:0.1]\n")

        if filename[:6] == "vary-T":
            out.write("plot  '" + filename + "' using 1:2 title 'MinD' with linespoints, '" + filename + "' using 1:3 title 'MinR' with linespoints\n")
        else:
            out.write("plot  '" + filename + "' using 1:2 title 'Squeeze-U' with linespoints, '" + filename + "' using 1:3 title 'TT-Breakpoint' with linespoints, '" + filename + "' using 1:4 title 'UH_Random' with linespoints, '" + filename + "' using 1:5 title 'MinD' with linespoints, '" + filename + "' using 1:6 title 'MinR' with linespoints, '" + filename + "' using 1:7 title 'Breakpoint' with linespoints\n")
            
        out.close()
        os.system("gnuplot temp.p")
    


if __name__ == "__main__":
    main()
