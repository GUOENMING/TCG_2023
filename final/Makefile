CC:=g++
FLAG:= -std=c++17 -O3
exe:=agent
obj:=src/main.cpp src/Mystar.cpp src/MyAI.cpp
head=src/Mystar.h src/MyAI.h

all:$(obj) $(head)
	$(CC) $(FLAG) -o $(exe) $(obj)

clean:
	rm $(exe)
