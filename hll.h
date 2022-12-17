#pragma once

#include <stdint.h>
#include <stddef.h>

struct hll;

struct hll *hll_create(unsigned precision);

void hll_add(struct hll *hll, uint64_t hash);

size_t hll_count_distinct(const struct hll *hll);

void hll_destroy(struct hll *hll);

size_t hll_size(unsigned precision);
