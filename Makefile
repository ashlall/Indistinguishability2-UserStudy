all:
	g++ -w *.c *.cpp -lglpk -lm -Ofast -o run

valgrind:
	g++ -w *.c *.cpp -g -O0 -lglpk -lm -o run

gdb:
	g++ -g -w *.c *.cpp -lglpk -lm -o run

countslopes:
	g++ -w test_count_slopes.cpp utility.cpp data_utility.cpp read_write.cpp -lglpk -lm -Ofast -o run_countslopes

clean:
	rm run