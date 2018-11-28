/**GIVEN*CODE*******************************************************************
 *
 * DA3 - Ukkonen voorbeeldcode
 *
 * Je mag deze code gebruiken binnen jouw project en zoveel aanpassen als je
 * zelf wilt. Bij vragen of problemen kun je de auteur contacteren.
 *
 * Door: Robbert Gurdeep Singh <Robbert.GurdeepSingh@UGent.be>
 */

/**
 * Ukkonen tree
 *
 * Each node of the tree has a childs array that may be NULL (in that case you
 * can use ukkonen_node_ensure_childs() to make sure that there is a childs
 * array)
 *
 * These arrays can be indexed by chars using the CHILD() macro in operations.c
 * (This will cast it to an unsigned char and subtract one). Do not pass <=0 to
 * it.
 *
 **/

#pragma once
#include <stdlib.h>
// Not 128 because we do not include NUL '\0'
#define BRANCHSIZE 127

// We will describe the leaf_it as -1
#define UEND -1
typedef enum { false = 0, true = 1 } bool;
typedef struct UkkonenNode UkkonenNode;

/**
 * A node in the tree and the edge *above* it.
 * A node is a leaf if its ending is UEND
 */
typedef struct UkkonenNode {       /* and edge above */
  int begin;                       /* negative for empty (root node) */
  int ending;                      /* UEND for leaf_it */
  struct UkkonenNode *suffix_link; /* NULL for none*/
  struct UkkonenNode **childs; /* Array of childs (may be NULL) */
  int rep_start;
  UkkonenNode* parent;
} UkkonenNode;

/**
 * The status of the Ukkonen algorithm:
 * - Pointer to where we are in the UkkonenTree (σ form the lecture notes)
     (point, edge, offset)
 * - Suffix-link that needs to be filled
 * - The first non-leaf position
 */
typedef struct UkkonenStatus {
  UkkonenNode *point;  // node below which we are
  const char *edge;    // pointer to the edge to take
  size_t offset;       // offset on the edge to take

  UkkonenNode *
      *suffix_hole;   // A pointer to the previous suffix-link that needs to be
                      // filled (a pointer to a UkkonenNode*)
  size_t start_from;  // The first none leaf suffix
} UkkonenStatus;

/**
 * The UkkonenTree struct describes a root node and the leaf_it.
 * A pointer to the original data (not malloced by ukkonen_make)
 *
 */
typedef struct UkkonenTree {
  const char *data;   // the text of the tree (not freed)
  UkkonenNode *root;  // The root node
  UkkonenStatus ap;   // A point in
  unsigned int leaf_it;
} UkkonenTree;

/** Malloc a Ukkonen node and set fields */
UkkonenNode *ukkonen_node_make(int begin, int end, UkkonenNode* parent, UkkonenTree* tree);

/** Free the space taken by the node and all its children */
UkkonenNode *ukkonen_node_free(UkkonenNode *node);

/** Malloc a Ukkonen tree and its root. Set fields */
UkkonenTree *ukkonen_make();

/** Free the space taken by the tree and all its nodes (data is not freed!) */
int ukkonen_free(UkkonenTree *tree);

/** Ensure that the children array of node is not empty */
UkkonenNode *ukkonen_node_ensure_childs(UkkonenNode *node);

/** Print the ukkonen tree in the correct format */
void ukkonen_print(UkkonenTree *tree);

/** Get the length of the contents of an edge (above the given node)*/
size_t ukkonen_node_len(UkkonenTree *tree, UkkonenNode *node);

/** let ukkonen ingest the char at this->data[this->leaf_it++]*/
int ukkonen_ingest_next(UkkonenTree *tree);
