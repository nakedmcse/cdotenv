all: test

test: cdotenv-stb-test.c cdotenv-stb.h
	gcc -O3 -o cdotenv-stb-test cdotenv-stb-test.c

clean:
	rm -f *.o
	rm -f *.a
	rm -f cdotenv-stb-test