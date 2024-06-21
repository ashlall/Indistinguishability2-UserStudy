all:
	g++ -w *.c *.cpp -lglpk -lm -Ofast -o run

valgrind:
	g++ -w *.c *.cpp -g -O0 -lglpk -lm -o run

gdb:
	g++ -g -w *.c *.cpp -lglpk -lm -o run

utility:
	g++ -w test_utility.cpp utility.cpp data_utility.cpp -lglpk -lm -Ofast -o run_utility

clean:
	rm run