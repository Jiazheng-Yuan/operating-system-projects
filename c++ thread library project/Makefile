main.o: main.cpp thread.h cv.h impl.h
	g++ main.cpp -c -g -std=c++11 

thread.o: thread.cpp thread.h impl.h
	g++ thread.cpp -c -g -std=c++11 

cv.o: cv.cpp cv.h thread.h impl.h
	g++ cv.cpp -c -g -std=c++11 

mutex.o: mutex.cpp mutex.h impl.h
	g++ mutex.cpp -c -g -std=c++11 

impl.o: impl.cpp impl.h
	g++ impl.cpp -c -g -std=c++11 

main: main.o thread.o cv.o mutex.o cpu.o
	g++ -g -Wall main.o thread.o mutex.o cv.o libcpu.o -ldl -pthread -std=c++11 -o main

clean:
	rm -rf impl.o
	rm -rf cv.o
	rm -rf thread.o
	rm -rf mutex.o
	rm -rf cpu.o
	rm -rf main.o
	rm -rf main
