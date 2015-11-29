cc = g++
std = c++11
exe = main
objects = main.o

$(exe) : $(objects)
			$(cc) -o $(exe) $(objects)

main.o : 123\\b.h a.h
	$(cc) -std=$(std) -c main.cpp

clean : 
		rm $(objects) $(exe)
