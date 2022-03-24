CC=gcc
THREAD_TOOL_ARGS=--tool=helgrind --verbose --log-file=valgrind-result.log
MEMCHECK_TOOL_ARGS=--leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.log
# WARNING: Don't combine 'valgrind' with ASAN/TSAN !
# ASAN_LIB=-fsanitize=address -fno-omit-frame-pointer
# TSAN_LIB=-fsanitize=thread
.PHONY: Q1 Q2 Q3 Q4 Q5 Q6
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
Q6: Q6.c
	gcc -g3 -pthread Q6.c -o Q6
Q4_debug: Q4
	valgrind $(THREAD_TOOL_ARGS) ./Q4 10 4 1
Q5_debug: Q5
	valgrind $(THREAD_TOOL_ARGS) ./Q5
Q6_debug: Q6
	valgrind $(THREAD_TOOL_ARGS) ./Q6
clean:
	rm -f *.o *.log Q[1-6]
