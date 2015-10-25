all:
	gcc -Wall main.c -o main -ggdb -lcurl
clean:
	rm main
