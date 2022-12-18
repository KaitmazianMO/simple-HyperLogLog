#include <stdlib.h>
#include <math.h>

#include "hll.h"

struct hll {
	unsigned precision;
	uint8_t *registers;
};

size_t hll_size(unsigned precision)
{
	return (UINT64_C(1) << precision);
}

struct hll *hll_create(unsigned precision)
{
	struct hll *hll = malloc(sizeof(*hll));
	hll->precision = precision;
	hll->registers = calloc(hll_size(precision), sizeof(*hll->registers));
	return hll;
}

void hll_destroy(struct hll *hll)
{
	free(hll->registers);
	free(hll);
}

static size_t hll_register_index(uint64_t hash, unsigned precision)
{
	return hash >> (64 - precision);
}

static uint8_t hll_register_rank(uint64_t hash)
{
	return __builtin_ctzll(hash) + 1;
}

void hll_add(struct hll *hll, uint64_t hash)
{
	size_t index = hll_register_index(hash, hll->precision);
	uint8_t rank = hll_register_rank(hash);
	if (hll->registers[index] < rank)
		hll->registers [index] = rank;
}

static double hll_alpha(size_t size)
{
	switch (size) {
	case 16:
		return 0.673;
	case 32:
		return 0.697;
	case 64:
		return 0.709;
	default:
		return  0.7213 / (1 + 1.079 / size);
	}
}

size_t hll_raw_estimate(const struct hll *hll)
{
	size_t size = hll_size(hll->precision);
	double sum = 0;
	for (size_t i = 0; i < size; ++i)
		sum += pow(2, -hll->registers[i]);

	double alpha_m_m = hll_alpha(size) * size * size;
	return alpha_m_m / sum;
}

static size_t linear_counting(double size, double nzeros)
{
	return size * log(size / nzeros);
}

static size_t hll_count_zeros(const struct hll *hll)
{
	size_t count = 0;
	for (size_t i = 0, size = hll_size(hll->precision); i < size; ++i)
		if (hll->registers[i] == 0)
			count += 1;
	return count;
}

size_t hll_count_distinct(const struct hll *hll,
			  const struct polynomial *correction)
{
	size_t nzeros = hll_count_zeros(hll);
	if (nzeros != 0) {
		size_t size = hll_size(hll->precision);
		double lc_estimate = linear_counting(size, nzeros);
		if (lc_estimate < 2.5 * size)
			return lc_estimate;
	}
	double estimate = hll_raw_estimate(hll);
	if (correction != NULL && estimate < 5 * hll_size(hll->precision))
		estimate -= correction->calc(estimate, correction);
	return estimate;
}
