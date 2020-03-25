all:
	g++ -static -fno-optimize-sibling-calls -fno-strict-aliasing -lm -s -x c++ -O3 -std=c++11 -Wall -o dictionarysearch dictionarysearch.cc

clean:
	rm -f dictionarysearch
