#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define MAX_BUFFER 1048576

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
	uint8_t karakter;
} DecompInput ;

int compress();

int compress_optimized();

int decompress();

int decompress_optimized();

LongestMatch* find_longest_match(UkkonenTree* tree, uint32_t start);

int compress_tree (UkkonenTree* tree, int size);