CPPC=g++

all: StringWordIterator.o HighlighterC.o main.o
	$(CPPC) -o vimuserdef main.o HighlighterC.o StringWordIterator.o -lpcrecpp

StringWordIterator.o: StringWordIterator.cpp
	$(CPPC) -g -o StringWordIterator.o -c StringWordIterator.cpp

HighlighterC.o: HighlighterC.cpp
	$(CPPC) -g -o HighlighterC.o -c HighlighterC.cpp

main.o:
	$(CPPC) -g -o main.o -c main.cpp

clean:
	rm *.o vimuserdef

install: all
	- mkdir -p ~/.bin/
	- mkdir -p ~/.vim/plugin
	cp vimuserdef ~/.bin/
	cp UserdefHighlight.vim ~/.vim/plugin/
