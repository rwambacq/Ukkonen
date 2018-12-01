#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ukkonen.h"

typedef struct CurrentPosition {
	UkkonenNode* curr_node;
	int curr_len;
} CurrentPosition;

typedef struct LongestMatch {
	uint32_t p;
	uint32_t l;
} LongestMatch;

typedef struct DecompInput {
	uint32_t p;
	uint32_t l;
	uint32_t karakter;
} DecompInput ;

void compress(UkkonenTree* tree, int size);

void compress_optimized(UkkonenTree* tree, int size);

void decompress();

LongestMatch* find_longest_match(UkkonenTree* tree, uint32_t start);