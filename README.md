# Indistinguishability
Readme for code for 'The Power of Two: Simplified User Interaction for the Indistinguishability Query' (Do, Kim, Chai, and Lall)

Base code by Min Xie (first author of Strongly Truthful Interactive Regret Minimization, SIGMOD 2019).
Code for "The Power of Two: Simplified User Interaction for the Indistinguishability Query" (TT-Breakpoint and Strongly Truthful Breakpoint) by Lam Do, Oghap Kim, and Chloe Chai

This repository has the following algorithms:
1. Squeeze-u
2. Squeeze-u2
3. MinR
4. MinD
5. Breakpoint (Do, Kim, Chai, 2024)
6. TT_Breakpoint (Do, Kim, Chai, 2024)

This package also provides the datasets used.

INSTALLATION:

1. Make sure there is a folder called "input/" and a folder "output/" under the working directory. They will be used for storing the input/output files and some intermediate results.
2.  You will need to install the GLPK package (for solving LPs) at first.
        See GLPK webpage <http://www.gnu.org/software/glpk/glpk.html>.
    The Makefile should handle all compilation (once you get GLPK installed correctly).
        Note that "-lglpk" in the Makefile is the GLPK package and you may need to change it to the location where you installed the GLPK package.
3. Run 'make' to compile code.
4. You will need to install the gnuplot package for plotting the experimental results. Make sure you are in the output folder, then use the command "python makeplot.py" to generate plots as pdfs.

RUNNING:

To run the code please consult EXPERIMENTS.txt for exact command line options. For example,

./run Q a3-100000.txt 0.05 0.0 2 9 B 0

will run the code for varying the number of questions asked of the user (q) on the data set a3-100000.txt (must be in the input/ folder) with epsilon=0.5, delta=0.0, s=2 tuples per round, running (B)oth real and artificial tuple queries, and a random seed of 0. While any random seed may be used, the experiments consistently used a seed value of 0 for reproducibility.

REPRODUCIBILITY (Do, Kim, Chai, Lall):

To reproduce the experiments in the paper 'The Power of Two: Simplified User Interaction for the Indistinguishability Query' use the following command line options:

./run T nba.txt 0.05 0.0 2 20 B 0; ./run E island.txt 0.05 0.0 2 10 B 0; ./run E nba.txt 0.05 0.0 2 20 B 0; ./run E house2.txt 0.05 0.0 2 30 B 0; ./run t island.txt 0.05 0.0 2 10 B 0; ./run t nba.txt 0.05 0.0 2 20 B 0; ./run t house2.txt 0.05 0.0 2 30 B 0; ./run Q island.txt 0.05 0.0 2 10 B 0; ./run Q nba.txt 0.05 0.0 2 20 B 0; ./run Q house2.txt 0.05 0.0 2 30 B 0; ./run d LAM 0.05 0.0 2 10 B 0; ./run n LAM 0.05 0.0 2 9 B 0

After running these commands, go to the output folder and type in:

python makeplot

to generate all the plots found in the paper 'The Power of Two: Simplified User Interaction for the Indistinguishability Query'.
