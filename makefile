#
# PROGRAM: ASSIGNMENT #4
# AUTHOR:  Leonart Jaos
#

CXXFLAGS = -Wall -Werror -std=c++14

z1911688_project4: z1911688_project4.o
	g++ $(CXXFLAGS) -o z1911688_project4 z1911688_project4.o

z1911688_project4.o: z1911688_project4.cpp
	g++ $(CXXFLAGS) -c z1911688_project4.cpp

clean:
	rm -f *.o z1911688_project4
