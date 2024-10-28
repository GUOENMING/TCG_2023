CC:=g++
FLAG:= -std=c++17 -O3
exe:=test
obj:=src/my_agent.cpp src/ewn.cpp src/random_walk.cpp
head=src/ewn.h

all:$(obj) $(head)
	$(CC) $(FLAG) -o $(exe) $(obj)

clean:
	rm $(exe)
