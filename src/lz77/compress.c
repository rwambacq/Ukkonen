#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "ukkonen.h"
#include "compress.h"

#define CHILD(node, x) ((node)->childs[(unsigned char)((x)-1)])


int compress(){
	int buffersize = 64;
	int size = 0;
	char* stringsje = (char*)malloc(buffersize * sizeof(char));
	char lees;

	UkkonenTree* tree = ukkonen_make();

	while ((lees = (char)fgetc(stdin)) != EOF) {
		if (size == buffersize) {
			stringsje = realloc(stringsje, sizeof(char) * (buffersize * 2));
			buffersize *= 2;
		}
		stringsje[size] = lees;
		size++;
	}

	if (size == buffersize) {
		stringsje = realloc(stringsje, sizeof(char) * (buffersize + 1));
		buffersize += 1;
	}

	stringsje[size] = '\0';
	tree->data = stringsje;

	compress_tree(tree, size);
	ukkonen_free(tree);

    return 0;
}

int compress_tree (UkkonenTree* tree, int size) {

	if (size > 0) {
		uint32_t p = (uint32_t)0, l = (uint32_t)0;
		uint8_t next = (uint8_t)tree->data[0];

		uint32_t vierbytes[2];
		uint8_t eenbyte[1];
		vierbytes[0] = p;
		vierbytes[1] = l;
		eenbyte[0] = next;

		fwrite(vierbytes, 4, 2, stdout);
		fwrite(eenbyte, 1, 1, stdout);

		if (tree->data[0] != '\0') {
			ukkonen_ingest_next(tree);

			uint32_t next_one = 1;

			while (tree->data[next_one] != '\0') {
				LongestMatch* lm = find_longest_match(tree, next_one);

				vierbytes[0] = (uint32_t)(lm->p);
				vierbytes[1] = (uint32_t)(lm->l);
				eenbyte[0] = (uint8_t)(tree->data[next_one + lm->l]);

				fwrite(vierbytes, 4, 2, stdout);
				fwrite(eenbyte, 1, 1, stdout);

				if (tree->data[next_one + lm->l + 1] != '\0') {
					ukkonen_ingest_next(tree);
				}
				if (tree->data[next_one + lm->l] != '\0') {
					next_one += (lm->l + 1);
				}
				else {
					next_one += (lm->l);
				}
				free(lm);
			}
		}
	}
	return 0;
}

LongestMatch* find_longest_match(UkkonenTree* tree, uint32_t start) {
	CurrentPosition* curr_pos = malloc(sizeof(CurrentPosition));
	curr_pos->curr_node = tree->root;
	curr_pos->curr_len = 0;

	uint32_t curr_index = start;

	while ( tree->data[curr_index] != '\0' && 
		((curr_pos->curr_len == 0 && CHILD(curr_pos->curr_node, tree->data[curr_index]) != NULL) // current position is at the root, check if next char is a child of current node
		|| ((curr_pos->curr_len == (ukkonen_node_end(tree, curr_pos->curr_node) - curr_pos->curr_node->begin + 1)) && CHILD(curr_pos->curr_node, tree->data[curr_index]) != NULL) // current position is at the end of a node, check if next char is a child of current node 
		|| ((curr_pos->curr_len != (ukkonen_node_end(tree, curr_pos->curr_node) - curr_pos->curr_node->begin + 1)) && (tree->data[curr_index] == tree->data[curr_pos->curr_node->begin + curr_pos->curr_len])))) { // current position is on an edge, check if next char on edge is next char to find
		
		//printf("Match zoeken loop afgelopen\n");

		if (curr_pos->curr_len == 0 || (curr_pos->curr_len == (ukkonen_node_end(tree, curr_pos->curr_node) - curr_pos->curr_node->begin + 1))) {
			curr_pos->curr_node = CHILD(curr_pos->curr_node, tree->data[curr_index]);
			curr_pos->curr_len = 1;
		}
		else {
			curr_pos->curr_len++;
		}
		if (tree->data[curr_index + 1] != '\0') {
			ukkonen_ingest_next(tree);
		}
		curr_index++;
	}

	LongestMatch* to_return = malloc(sizeof(LongestMatch));

	uint32_t len = curr_index - start;

	if (len == 0) {
		to_return->l = 0;
		to_return->p = 0;
	}
	else {
		to_return->p = curr_pos->curr_node->rep_start;
		to_return->l = len;
	}
	free(curr_pos);
	return to_return;
}

int compress_optimized () {
	char lees = 'a';
	int size = 0;

	while(lees != EOF) {

		int buffersize = 64;
		char *stringsje = (char *) malloc(buffersize * sizeof(char));

		if(size != 0){
			stringsje[0] = lees;
			size = 1;
		}

		UkkonenTree *tree = ukkonen_make();

		while (((lees = (char) fgetc(stdin)) != EOF) && size < MAX_BUFFER) {
			stringsje[size] = lees;
			size++;
			if ((size == buffersize) && (buffersize != MAX_BUFFER)) {
				stringsje = realloc(stringsje, sizeof(char) * (buffersize * 2));
				buffersize *= 2;
			}
		}

		if (size == buffersize) {
			stringsje = realloc(stringsje, sizeof(char) * (buffersize + 1));
			buffersize += 1;
		}

		stringsje[size] = '\0';
		tree->data = stringsje;

		compress_tree(tree, size);
		ukkonen_free(tree);
		free(stringsje);
	}
	return 0;
}

#undef CHILD