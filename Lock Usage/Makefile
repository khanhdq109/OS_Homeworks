CC=gcc
ANALYSIS_TOOL=valgrind --tool=helgrind --verbose --log-file=valgrind-result.log
# ASAN_LIB=-fsanitize=address -fno-omit-frame-pointer
.PHONY: Q1 Q2 Q3 Q4 Q5
Q1: Q1.c
	gcc Q1.c -o Q1
Q2: Q2.c
	gcc -pthread Q2.c -o Q2
Q3: Q3.c
	gcc -pthread Q3.c -o Q3
Q4: Q4.c
	gcc -g3 -pthread Q4.c -o Q4 
Q5: Q5.c
	gcc -g3 -pthread Q5.c -o Q5
Q4_debug: Q4
	$(ANALYSIS_TOOL) ./Q4 10 4 1
Q5_debug: Q5
	$(ANALYSIS_TOOL) ./Q5
clean:
	rm -f *.o *.log Q[1-5]
