CFLAGS = -g -c -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline \
		 -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default \
 		 -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy \
		 -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers \
		 -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
		 -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

all: run_bintree

main.o: main.cpp
	g++ $(CFLAGS) main.cpp -o main.o

bintree.o: bintree.cpp
	g++ $(CFLAGS) bintree.cpp -o bintree.o

differenciator.o: differenciator.cpp
	g++ $(CFLAGS) differenciator.cpp -o differenciator.o

run_bintree: main.o bintree.o differenciator.o
	g++ main.o bintree.o differenciator.o -o main

run_programm:
	./main
	cd CPU
	echo ./run_coder
	echo ./run_stack
	
clean:
	rm *.o *.exe

del_txt_png:
	rm *.txt *.png