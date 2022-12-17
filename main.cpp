#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <cstring>
#include <random>
#include <cstdint>

extern "C" {
#include "hll.h"
}

int main(int argc, char *argv[])
{
	if(argc != 2) {
		printf("Usage: %s {precision}\n", argv[0]);
		exit(-1);
	}

	int precision = strtoll(argv[1], NULL, 10);
	struct hll *hll = hll_create(precision);
	auto hasher = std::hash<std::string>{};
	std::string input;
	while (std::getline(std::cin, input))
		hll_add(hll, hasher(input));

	size_t estimation = hll_count_distinct(hll);
	std::cout << estimation << std::endl;

	hll_destroy(hll);

	return 0;
}
