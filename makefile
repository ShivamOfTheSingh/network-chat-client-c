all:
	gcc -o client -lpthread client.c; gcc -o server -lpthread server.c
clean:
	rm server client logs.txt
