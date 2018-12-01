/**GIVEN*CODE*******************************************************************
 *
 * DA3 - Ukkonen voorbeeldcode
 *
 * Je mag deze code gebruiken binnen jouw project en zoveel aanpassen als je
 * zelf wilt. Bij vragen of problemen kun je de auteur contacteren.
 *
 * Door: Robbert Gurdeep Singh <Robbert.GurdeepSingh@UGent.be>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ukkonen.h"
#define ERROR_MALLOC -2

UkkonenTree* ukkonen_make() {
  UkkonenTree* tree = malloc(sizeof(UkkonenTree));
  if (NULL == tree) {
    fprintf(stderr, "Could not malloc space for tree\n");
    exit(ERROR_MALLOC);
  }

  tree->data = NULL;

  tree->root = ukkonen_node_make(-1, 0, NULL, tree);
  tree->root->suffix_link = tree->root;
  ukkonen_node_ensure_childs(tree->root);

  tree->ap.point = tree->root;
  tree->ap.edge = '\0';
  tree->ap.offset = 0;
  tree->ap.suffix_hole = NULL;
  tree->ap.start_from = 0;

  tree->leaf_it = -1;
  return tree;
}

int ukkonen_free(UkkonenTree* tree) {
  if (NULL == tree) return 0;
  if (NULL != tree->root) ukkonen_node_free(tree->root);
  free(tree);
  return 0;
}

/** Malloc a Ukkonen node and set fields */
UkkonenNode* ukkonen_node_make(int begin, int end, UkkonenNode* parent, UkkonenTree* tree) {
  UkkonenNode* node = calloc(1, sizeof(UkkonenNode));
  if (NULL == node) {
    fprintf(stderr, "Could not malloc space for tree node\n");
    exit(ERROR_MALLOC);
  }
  if (begin < 0 || parent->rep_start < 0) {
	  node->rep_start = begin;
  }
  else {
	  size_t parent_einde = ukkonen_node_end(tree, parent);
	  node->rep_start = begin - (parent_einde - parent->rep_start + 1);
  }
  node->parent = parent;
  node->begin = begin;
  node->ending = end;
  node->suffix_link = NULL;
  return node;
}

/** Ensure that the children array of node is not empty */
UkkonenNode* ukkonen_node_ensure_childs(UkkonenNode* node) {
  if (node->childs == NULL) {
    UkkonenNode** childsArr = calloc(BRANCHSIZE, sizeof(UkkonenNode*));
    if (NULL == childsArr) {
      fprintf(stderr, "Could not malloc space for tree node children array\n");
      exit(ERROR_MALLOC);
    }
    node->childs = childsArr;
  }
  return node;
}

/** Free the space taken by the node and all its children */
UkkonenNode* ukkonen_node_free(UkkonenNode* node) {
  if (NULL == node) return 0;
  if (node->childs != NULL) {
    for (int i = 0; i < BRANCHSIZE; i++) {
      if (NULL != node->childs[i]) {
        ukkonen_node_free(node->childs[i]);
      }
    }
    free(node->childs);
  }
  free(node);
  return 0;
}

/** Get the end position of a node*/
size_t ukkonen_node_end(UkkonenTree* tree, UkkonenNode* node) {
  return node->ending == UEND ? tree->leaf_it : (size_t)node->ending;
}

/** Get the length of the contents of an edge (above the given node)*/
size_t ukkonen_node_len(UkkonenTree* tree, UkkonenNode* node) {
  return 1 + ukkonen_node_end(tree, node) - node->begin;
}

/** Print a node of the ukkonen tree in the correct format
 *
 * last_index is a pointer to a shared piece of memory that keeps track of the
 * last assigned DFS index.
 *
 * The range a node describes starts at its end position minus the sum of the
 * lengths of all edges above that node (and ends at its end position).
 */
size_t ukkonen_node_print(UkkonenNode* node, UkkonenTree* tree, size_t len,
                          size_t dfs_index, size_t* last_index) {
  size_t ids[BRANCHSIZE] = {0};
  if (node->childs != NULL) {
    for (int i = 0; i < BRANCHSIZE; i++) {
      UkkonenNode* cur_child = node->childs[i];
      if (cur_child != NULL) {
        (*last_index)++;  // This child gets the next DFS index
        size_t ending = ukkonen_node_end(tree, cur_child);
        size_t new_len = len + ending - cur_child->begin + 1;
        ids[i] = ukkonen_node_print(node->childs[i], tree, new_len, *last_index,
                                    last_index);
      }
    }
  }

  if (dfs_index == 0) {
    printf("%3lu @  - ", dfs_index);  // the root represents the empty string
  } else {
    size_t ending = ukkonen_node_end(tree, node);
    printf("%3lu @ %lu-%lu", dfs_index, node->rep_start, ending);
  }
  bool first = true;
  if (node->childs != NULL) {
    for (int i = 0; i < BRANCHSIZE; i++) {
      UkkonenNode* cur_child = node->childs[i];
      if (cur_child != NULL) {
        if (!first) {
          printf(" | ");
        } else {
          printf(" = ");
          first = false;
        }

        // plus one because we do not include NUL
        printf("%d:%lu,%d-", i + 1, ids[i], cur_child->begin);
        if (cur_child->ending != UEND) {
          printf("%d", cur_child->ending);
        } else {
          printf("%u", tree->leaf_it);
        }
      }
    }
  }
  printf("\n");
  return dfs_index;
}

/** Print the ukkonen tree in the correct format */
void ukkonen_print(UkkonenTree* tree) {
  size_t accumulator = 0;
  ukkonen_node_print(tree->root, tree, /* len */ 0, accumulator, &accumulator);
}
