#pragma once

#include <stdint.h>
#include <stddef.h>

struct hll;

struct hll *hll_create(unsigned precision);

void hll_add(struct hll *hll, uint64_t hash);

struct polynomial {
	double (*calc)(double x, const struct polynomial *poly);
	/* data */
};

size_t hll_raw_estimate(const struct hll *hll);

size_t hll_count_distinct(const struct hll *hll,
			  const struct polynomial *bias_correction);

void hll_destroy(struct hll *hll);

size_t hll_size(unsigned precision);
