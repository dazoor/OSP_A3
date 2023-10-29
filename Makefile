.default: all

all: firstfit bestfit

clean:
	find . -name '*.o' -type f -delete
	find . -name 'firstfit' -type f -delete
	find . -name 'bestfit' -type f -delete

firstfit: fitstfit.cpp dealloc.cpp
	g++ -Wall -Werror -std=c++20 -O -o $@ $^

bestfit: bestfit.cpp dealloc.cpp
	g++ -Wall -Werror -std=c++20 -O -o $@ $^
