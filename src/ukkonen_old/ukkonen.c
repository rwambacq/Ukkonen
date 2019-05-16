#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ukkonen.h"

char* allocate_string(int strlen) {
	char* allocated = (char*)malloc((strlen + 1) * sizeof(char));
	if (!allocated) {
		printf("ERROR: Allocation failed, insufficient memory?\n");
		exit(1);
	}
	allocated[strlen] = '\0';
	return allocated;
}

SimpleDFSLinkedList * dfs_indexation(SimpleDFSLinkedList* node) {
	SimpleDFSLinkedList* current = node;
	for (int i = 0; i < 128; i++) {
		if (node->node->edges[i]) {
			SimpleDFSLinkedList* volgende = create_dfs_ll(node->node->edges[i]->child);
			current->next = volgende;
			current = dfs_indexation(volgende);
		}
	}
	return current;
}

SuffixTree* create_tree(char string[], int len) {
	int temp = -1;
	TreeNode* root = create_node(-1, &temp);
	SuffixTree* tree = (SuffixTree*)malloc(sizeof(SuffixTree));
	tree->root = root;
	tree->string = string;
	tree->len = len;
	return tree;
}

TreeEdge* create_edge(int start, int* end, TreeNode* parent, SuffixTree* tree) {
	TreeEdge* edge = (TreeEdge*)malloc(sizeof(TreeEdge));
	edge->rep_start = start;
	edge->rep_end = end;
	edge->parent = parent;
	int childStart;
	if (parent->rep_start == -1) {
		childStart = start;
		// Indien de boog van de root komt, stelt de eerstvolgende node gewoon start -> end van de boog voor
	} else {
		childStart = start - (*(parent->rep_end) - parent->rep_start + 1); 
		// Indien de boog van een niet-root node komt, stelt de child node de string voor van de vorige node + de boog
	}
	parent->edges[(int)(tree->string[start])] = edge;
	TreeNode* childNode = create_node(childStart, end);
	edge->child = childNode;
	return edge;
}

TreeNode* create_node(int start, int* end) {
	TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
	node->rep_start = start;
	if (start == -1) {
		node->rep_end = &(node->rep_start);
	} else {
		node->rep_end = end;
	}
	node->edges = malloc(128 * sizeof(TreeEdge*));
	for (int i = 0; i < 128; i++) {
		node->edges[i] = NULL;
	}
	node->suffix_link = NULL;
	return node;
}

UkkonenInfo* create_info(TreeNode* root) {
	UkkonenInfo* info = (UkkonenInfo*)malloc(sizeof(UkkonenInfo));
	info->curr = -1;
	info->remaining = 1;
	info->active_pointer = create_active_pointer(root);
	return info;
}

ActivePointer* create_active_pointer(TreeNode* root) {
	ActivePointer* ap = (ActivePointer*)malloc(sizeof(ActivePointer));
	ap->active_node = root;
	ap->active_edge = '\0';
	ap->active_length = 0;
	return ap;
}

SimpleDFSLinkedList* create_dfs_ll(TreeNode* node) {
	SimpleDFSLinkedList* toReturn = malloc(sizeof(SimpleDFSLinkedList));
	toReturn->node = node;
	toReturn->next = NULL;
	return toReturn;
}

void free_tree(SuffixTree* tree) {
	free(tree->string);
	free_node(tree->root);
	free(tree);
}

void free_edge(TreeEdge* edge) {
	free_node(edge->child);
	free(edge);
}

void free_node(TreeNode* node) {
	for (int i = 0; i < 128; i++) {
		if (node->edges[i]) {
			free_edge(node->edges[i]);
		}
	}
	free(node);
}

void free_active_pointer(ActivePointer* ap) {
	free(ap);
}

void free_info(UkkonenInfo* info) {
	free_active_pointer(info->active_pointer);
	free(info);
}

int find_index(TreeNode* te_vinden, SimpleDFSLinkedList* huidige_node, int huidige_index) {
	SimpleDFSLinkedList* curr = huidige_node;
	int toReturn = huidige_index;
	int gevonden = 0;
	while ((!gevonden) && (curr)) {
		if (curr->node == te_vinden) {
			gevonden = 1;
		}
		else {
			toReturn++;
			curr = curr->next;
		}
	}
	if (curr == NULL) {
		return -1;
	} else {
		return toReturn;
	}
}

SuffixTree* ukkonen(char s[], int len) {
	// TODO: Create suffix tree from given tree with ukkonen
	SuffixTree* boom = create_tree(s, len);
	UkkonenInfo* info = create_info(boom->root);
	int done;
	TreeNode* prev_added = NULL;
	TreeEdge* prev_added_edge = NULL;

	// 1 iteratie voor elke character in de in te voegen string
	for (int i = 0; i < len; i++) {
		if (info->remaining == 0) {
			info->remaining = 1;
		}
		info->curr++;
		done = 0;
		char in_te_voegen = s[i];
		prev_added = NULL;
		prev_added_edge = NULL;
		while ((!done) && (info->remaining > 0)) {
			// insert remaining # suffixes, startend met de grootste
			// dit komt door het bijhouden van het active point neer op het remaining maal invoegen van de laatste index op active point
			int	al_aanwezig = 0;
			if (info->active_pointer->active_length == 0) {
				al_aanwezig = (info->active_pointer->active_node->edges[(int)in_te_voegen] != NULL);
			} else {
				TreeEdge* active_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
				int zoek_index = active_edge->rep_start + info->active_pointer->active_length;
				al_aanwezig = (s[zoek_index] == in_te_voegen);
			}
			if (al_aanwezig) {
				done = 1;
				info->active_pointer->active_length++;
				info->remaining++;
				if (info->active_pointer->active_edge == '\0') {
					info->active_pointer->active_edge = (int)in_te_voegen;
				}
				TreeEdge* active_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];					
				if ((active_edge->rep_start + info->active_pointer->active_length) > *(active_edge->rep_end)) {
					info->active_pointer->active_edge = '\0';
					info->active_pointer->active_length = 0;
					info->active_pointer->active_node = active_edge->child;
				}
			} else {
				if (info->active_pointer->active_length == 0) {
					create_edge(i, &(info->curr), info->active_pointer->active_node, boom);
				}
				else {
					// Huidige edge opsplitsen
					TreeEdge* original = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
					int og_start = original->rep_start;
					original->rep_start = original->rep_start + info->active_pointer->active_length;
					TreeEdge* nieuw = create_edge(og_start, &(info->curr), info->active_pointer->active_node, boom);
					original->parent = nieuw->child;
					nieuw->child->edges[(int)s[original->rep_start]] = original;
					nieuw->internal_node_end = original->rep_start - 1;
					nieuw->rep_end = &(nieuw->internal_node_end);
					nieuw->child->internal_node_end = original->rep_start - 1;
					nieuw->child->rep_end = &(nieuw->child->internal_node_end);

					// Aan nieuwe node in midden van edge nieuwe edge toevoegen
					create_edge(i, &(info->curr), original->parent, boom);

					if (prev_added == NULL) {
						prev_added = nieuw->child;
						if (info->active_pointer->active_edge != '\0') {
							prev_added_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
						}
					} else {
						prev_added->suffix_link = nieuw->child;
						prev_added = nieuw->child;
						if (info->active_pointer->active_edge != '\0') {
							prev_added_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
						}
					}
				}
				info->remaining--;
				if (info->active_pointer->active_node == boom->root) {
					info->active_pointer->active_edge = s[i - info->remaining + 1];
					if (info->active_pointer->active_length > 0) {
						info->active_pointer->active_length--;
					}
				} else {
					if (info->active_pointer->active_node->suffix_link) {
						info->active_pointer->active_node = info->active_pointer->active_node->suffix_link;
					} else {
						info->active_pointer->active_node = boom->root;
						info->active_pointer->active_length = info->remaining - 1;
						info->active_pointer->active_edge = s[i - info->remaining + 1];
					}

					// Volg pahdt huidige edge tot active length < lengte edge
					if (info->active_pointer->active_edge != '\0') {
						TreeEdge* active_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
						int range = *(active_edge->rep_end) - active_edge->rep_start + 1;
						int totalrange = range;
						while (info->active_pointer->active_length >= range) {
							info->active_pointer->active_node = active_edge->child;
							info->active_pointer->active_length -= range;
							if (info->active_pointer->active_length == 0) {
								info->active_pointer->active_edge = '\0';
							}
							else {
								info->active_pointer->active_edge = (int)s[prev_added_edge->rep_start + totalrange];
							}
							active_edge = info->active_pointer->active_node->edges[(int)info->active_pointer->active_edge];
							range = *(active_edge->rep_end) - active_edge->rep_start + 1;
							totalrange += range;
						}
					}
				}
			}
		}
	}
	return boom;
}

void print_tree(SuffixTree* tree) {
	SimpleDFSLinkedList* wortel = create_dfs_ll(tree->root);
	dfs_indexation(wortel);
	// Nu staat in "wortel" het eerste element van een singly linked list van alle nodes in dfs volgorde.
	
	SimpleDFSLinkedList* current = wortel;
	int index = 0;
	while (current) {
		if (index != 0) {
			printf("%d @ %d-%d", index, current->node->rep_start, *(current->node->rep_end));
		}
		else {
			printf("%d @ - ", index);
		}
		int teller = 0;
		for (int i = 0; i < 128; i++) {
			if (current->node->edges[i]) {
				int ascii_start, dfs_index, edge_start, edge_end;
				edge_start = current->node->edges[i]->rep_start;
				edge_end = *(current->node->edges[i]->rep_end);
				ascii_start = (int)(tree->string[edge_start]);
				dfs_index = find_index(current->node->edges[i]->child, current, index);
				if (teller != 0) {
					printf(" | %d:%d,%d-%d", ascii_start, dfs_index, edge_start, edge_end);
				} else {
					printf(" = %d:%d,%d-%d", ascii_start, dfs_index, edge_start, edge_end);
					teller++;
				}
			}
		}
		current = current->next;
		printf("\n");
		index++;
	}
}

unsigned long get_input_length(FILE* stream) {
	unsigned long size;
	if (!stream) {
		printf("Stream bestaat niet\n");
		exit(1);
	}
	fseek(stream, 0, SEEK_END);
	size = ftell(stream);
	rewind(stream);
	return size;
}

int main(int argc, char* argv[]) {
		
	/*char* string = malloc(20 * sizeof(char));
	int size = get_input_length(stdin);
	string = allocate_string(size);
	fread(string, 1, size, stdin);*/

	int buffersize = 64;
	int size = 0;
	char* stringsje = (char*) malloc(buffersize * sizeof(char));
	char lees;

	while ((lees = (char)fgetc(stdin)) != EOF) {
		if (size == buffersize) {
			stringsje = realloc(stringsje, sizeof(char) * (buffersize * 2));
			buffersize *= 2;
		}
		stringsje[size] = lees;
		size++;
	}
	//SuffixTree* test_boom = ukkonen("abacabc_", 8);
	SuffixTree* test_boom = ukkonen(stringsje, size);
	print_tree(test_boom);
	//getchar();
}