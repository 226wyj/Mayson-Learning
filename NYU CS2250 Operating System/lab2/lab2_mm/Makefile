main: main.o DES.o Event.o Process.o Scheduler.o Utils.o
	g++ -std=c++11 main.o DES.o Event.o Process.o Scheduler.o Utils.o -o scheduler

main.o: main.cpp DES.h Event.h Process.h Scheduler.h Utils.h
	g++ -std=c++11 -c main.cpp

DES.o: DES.cpp DES.h
	g++ -std=c++11 -c DES.cpp

Event.o: Event.cpp Event.h
	g++ -std=c++11 -c Event.cpp

Process.o: Process.cpp Process.h
	g++ -std=c++11 -c Process.cpp

Scheduler.o: Scheduler.cpp Scheduler.h
	g++ -std=c++11 -c Scheduler.cpp

Utils.o: Utils.cpp Utils.h
	g++ -std=c++11 -c Utils.cpp

clean:
	rm -f scheduler *.o