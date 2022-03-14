Q1: Q1.c
	gcc Q1.c -o Q1
Q2: Q2.c
	gcc -pthread Q2.c -o Q2
clean:
	rm -f *.o Q1 Q2
