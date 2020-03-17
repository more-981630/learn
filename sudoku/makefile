All:sudo.o main.o main clean

src = $(wildcard ./*.cpp)
obj = $(patsubst %.cpp, %.o, $(src))


main:$(obj)
	g++ $^ -lncursesw -o $@

%.o:%.cpp
	g++ $< -c -o $@


clean:
	rm -f $(obj)
