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

uint64_t rand64() {
	static std::mt19937 generator (123);
	static std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);
	return dis(generator);
}

struct point {
	double x, y;
};

constexpr int npoints = 3;

struct newtons_polynomial : polynomial {
	point points[npoints];
};

double calculate_newtons_polynomial(double x, newtons_polynomial *polynomial)
{
	point *p = polynomial->points;
	double f0 = p[0].y;
	double f01 = (p[1].y - p[0].y) / (p[1].x - p[0].x);
	double f12 = (p[2].y - p[1].y) / (p[2].x - p[1].x);
	double f012 = (f12 - f01) / (p[2].x - p[0].x);
	return f0 + f01 * (x - p[0].x) + f012 * (x - p[0].x) * (x - p[1].x);
}

double calculate_bias_correction(double raw_estimate,
				 const struct polynomial *cardinality_polynomial_)
{
	struct newtons_polynomial *cardinality_polynomial =
		(struct newtons_polynomial *)cardinality_polynomial_;
	double cardinality =
		calculate_newtons_polynomial(raw_estimate, cardinality_polynomial);
	return raw_estimate - cardinality;
}

polynomial *new_bias_correction(unsigned precision)
{
	double m = hll_size(precision);
	struct point points[npoints] = { {0, 2*m}, {0, 3*m}, {0, 5*m} };
	for (int i = 0; i < npoints; ++i) {
		struct hll *hll = hll_create(precision);
		for (size_t n = 0; n < points[i].y; ++n)
			hll_add(hll, rand64());

		points[i].x = hll_raw_estimate(hll);
		hll_destroy(hll);
	}

	auto ret = new newtons_polynomial;
	ret->calc = calculate_bias_correction;
	std::memcpy(ret->points, points, sizeof(points));
	return ret;
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
	auto bias_correction = new_bias_correction(precision);

	std::string input;
	while (std::getline(std::cin, input))
		hll_add(hll, hasher(input));

	size_t estimation = hll_count_distinct(hll, bias_correction);
	std::cout << estimation << std::endl;

	hll_destroy(hll);
	delete bias_correction;

	return 0;
}
