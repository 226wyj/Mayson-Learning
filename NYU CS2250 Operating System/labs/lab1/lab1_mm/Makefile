main: main.o Tokenizer.o Parser.o
	g++ -std=c++11 main.o Tokenizer.o Parser.o -o linker

main.o: main.cpp Tokenizer.h Parser.h
	g++ -std=c++11 -c main.cpp

Tokenizer.o: Tokenizer.cpp Tokenizer.h
	g++ -std=c++11 -c Tokenizer.cpp

Parser.o: Parser.cpp Parser.h
	g++ -std=c++11 -c Parser.cpp

clean:
	rm -f linker *.o