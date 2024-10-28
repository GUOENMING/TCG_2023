CC:=g++
FLAG:= -std=c++17 -O2
exe:=test
obj:=src/astar.cpp
head=src/ewn.h


all:$(obj) $(head) 
	$(CC) $(FLAG) -o $(exe) $(obj)

clean:
	rm $(exe)
