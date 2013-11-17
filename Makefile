CPPC=g++
CFLAGS+=-g3 -Wall

all: StringWordIterator.o HighlighterJava.o HighlighterC.o main.o
	$(CPPC) -o vimuserdef main.o HighlighterC.o HighlighterJava.o StringWordIterator.o -lpcrecpp

StringWordIterator.o: StringWordIterator.cpp
	$(CPPC) $(CFLAGS) -o StringWordIterator.o -c StringWordIterator.cpp

HighlighterC.o: HighlighterC.cpp
	$(CPPC) $(CFLAGS) -o HighlighterC.o -c HighlighterC.cpp

HighlighterJava.o: HighlighterJava.cpp
	$(CPPC) $(CFLAGS) -o HighlighterJava.o -c HighlighterJava.cpp

main.o:
	$(CPPC) $(CFLAGS) -o main.o -c main.cpp

clean:
	- rm *.o vimuserdef

install: all
	- mkdir -p ~/.bin/
	- mkdir -p ~/.vim/plugin
	cp vimuserdef ~/.bin/
	cp UserdefHighlight.vim ~/.vim/plugin/
	(cp java.vim ~/.vim/syntax/ && \
	 ln -sf `find $$VIM/*/syntax -name html.vim | head -1` ~/.vim/syntax)
