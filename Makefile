estimator: main.cpp hll.c hll.h
	gcc -Wall -Wextra -Werror -o hll -c hll.c
	g++ -Wall -Wextra -Werror -o main -c main.cpp
	g++ -o estimator main hll
