all: StringWordIterator.o HighlighterC.o
	g++ -o vimuserdef HighlighterC.o StringWordIterator.o -lpcrecpp

StringWordIterator.o: StringWordIterator.cpp
	g++ -g -o StringWordIterator.o -c StringWordIterator.cpp

HighlighterC.o: HighlighterC.cpp
	g++ -g -o HighlighterC.o -c HighlighterC.cpp

clean:
	rm *.o vimuserdef

install: all
	- mkdir -p ~/.bin/
	- mkdir -p ~/.vim/plugin
	cp vimuserdef ~/.bin/
	cp UserdefHighlight.vim ~/.vim/plugin/
