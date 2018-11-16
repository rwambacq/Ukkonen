#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SuffixTree.h"

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
	TreeNode* root = create_node(-1, -1);
	SuffixTree* tree = (SuffixTree*)malloc(sizeof(SuffixTree));
	tree->root = root;
	tree->string = allocate_string(len);
	strcpy(tree->string, string);
	tree->len = len;
	return tree;
}

TreeEdge* create_edge(int start, int end, TreeNode* parent, SuffixTree* tree) {
	TreeEdge* edge = (TreeEdge*)malloc(sizeof(TreeEdge));
	edge->rep_start = start;
	edge->rep_end = end;
	edge->parent = parent;
	int childStart;
	if (parent->rep_end == -1) {
		childStart = start;
		// Indien de boog van de root komt, stelt de eerstvolgende node gewoon start -> end van de boog voor
	} else {
		childStart = start - (parent->rep_end - parent->rep_start + 1); 
		// Indien de boog van een niet-root node komt, stelt de child node de string voor van de vorige node + de boog
	}
	parent->edges[(int)(tree->string[start])] = edge;
	TreeNode* childNode = create_node(childStart, end);
	edge->child = childNode;
	return edge;
}

TreeNode* create_node(int start, int end) {
	TreeNode* node = (TreeNode*) malloc(sizeof(TreeNode));
	node->rep_start = start;
	node->rep_end = end;
	node->edges = malloc(128 * sizeof(TreeEdge*));
	for (int i = 0; i < 128; i++) {
		node->edges[i] = NULL;
	}	
	return node;
}

UkkonenInfo* create_info(TreeNode* root) {
	UkkonenInfo* info = (UkkonenInfo*)malloc(sizeof(UkkonenInfo));
	info->curr = 0;
	info->remaining = 1;
	info->active_pointer = create_active_pointer(root);
	return info;
}

ActivePointer* create_active_pointer(TreeNode* root) {
	ActivePointer* ap = (ActivePointer*)malloc(sizeof(ActivePointer));
	ap->active_node = root;
	ap->active_edge = '\0x';
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
}

void print_tree(SuffixTree* tree) {
	SimpleDFSLinkedList* wortel = create_dfs_ll(tree->root);
	SimpleDFSLinkedList* laatste = dfs_indexation(wortel);
	// Nu staat in "wortel" het eerste element van een singly linked list van alle nodes in dfs volgorde.
	
	SimpleDFSLinkedList* current = wortel;
	int index = 0;
	while (current) {
		if (current->node->rep_end != -1) {
			printf("%d @ %d-%d", index, current->node->rep_start, current->node->rep_end);
		}
		else {
			printf("%d @ - ", index);
		}
		int teller = 0;
		for (int i = 0; i < 128; i++) {
			if (current->node->edges[i]) {
				int ascii_start, dfs_index, edge_start, edge_end;
				edge_start = current->node->edges[i]->rep_start;
				edge_end = current->node->edges[i]->rep_end;
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
		printf("\n");
		index++;
		current = current->next;
	}
}

int main(int argc, char* argv[]) {
	// TODO: Read string from stdin, make ukkonen tree with that string, print
	SuffixTree* test_boom = create_tree("abcad", 3);
	TreeEdge* a = create_edge(0, 0, test_boom->root, test_boom);
	create_edge(4, 4, test_boom->root, test_boom);
	create_edge(2, 4, test_boom->root, test_boom);
	create_edge(1, 4, test_boom->root, test_boom);

	create_edge(1, 4, a->child, test_boom);
	create_edge(4, 4, a->child, test_boom);
	print_tree(test_boom);
	getchar();
}