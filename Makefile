CXX = g++
FLAGS = -g -W -Wall -Wextra -Wpedantic -Werror -std=c++11
LIBRARIES = -lpthread

.PHONY: default run

default: run

run:
	${CXX} ${FLAGS} *.cpp ${LIBRARIES} -o program

clean:
	-@rm -rf *.o program core
