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

/** MACRO to get the node at the end of the edge with x
 * The -1 saves a space in every internal node because NUL will never be in the
 * input.
 *
 * x must be > 0
 */
#define CHILD(node, x) ((node)->childs[(unsigned char)((x)-1)])

/** Set the previous suffix-link to point to target */
void ukkonen_fill_suffix_hole(UkkonenNode*** suffix_hole, UkkonenNode* target) {
  if (*suffix_hole != NULL) {
    **suffix_hole = target;
    *suffix_hole = NULL;
  }
}

/** Normalise the active point (move point until offset fits in the branch) */
void ukkonen_fix_ap(UkkonenTree* tree) {
  UkkonenStatus* ap = &(tree->ap);
  while (ap->offset &&
         ukkonen_node_len(tree, CHILD(ap->point, *ap->edge)) <= ap->offset) {
    size_t len = ukkonen_node_len(tree, CHILD(ap->point, *ap->edge));
    ap->offset -= len;
    ap->point = CHILD(ap->point, *ap->edge);
    ap->edge += len;
  }
}

/**
 * Split the current edge at position of AP and make a new leaf branch for
 * text[c..c]
 *
 * This case occurs when the active point has offset != 0 and edge != null
 * Note that the AP is the parent
 *
 *                      AP                             AP
 *                      /       =>       new_split_up /
 *           cur_node /              new_split_down /\ new_spit_branch
 */
void ukkonen_split_edge(UkkonenTree* tree, UkkonenNode* cur_node) {
  UkkonenStatus* ap = &(tree->ap);
  UkkonenNode*** suffix_hole = &(tree->ap.suffix_hole);

  UkkonenNode* new_split_up =
      ukkonen_node_make(cur_node->begin, cur_node->begin + ap->offset - 1, cur_node->parent, tree);
  UkkonenNode* new_spit_branch =
      ukkonen_node_make(tree->leaf_it /*cur_end*/, UEND, new_split_up, tree);
  UkkonenNode* new_split_down = cur_node;
  new_split_down->parent = new_split_up;
  new_split_down->begin = cur_node->begin + ap->offset;
  // new_split_down->ending = cur_node->ending; // NOP

  // Replace cur_node with new_split_up as child of AP
  CHILD(ap->point, *ap->edge) = new_split_up;

  // Add a children array to new_split_up, it will have two children
  ukkonen_node_ensure_childs(new_split_up);
  CHILD(new_split_up, tree->data[new_split_down->begin]) = new_split_down;
  CHILD(new_split_up, tree->data[new_spit_branch->begin]) = new_spit_branch;

  ukkonen_fill_suffix_hole(suffix_hole, new_split_up);
  *suffix_hole = &(new_split_up->suffix_link);
}

/**
 * Add a child to the current node
 *
 * cur_end : the index of the last ingested character
 * tree    : tree to perform action on
 * cur_node: the node to whom a child is added
 *
 */
void ukkonen_add_child(int cur_end, UkkonenTree* tree, UkkonenNode* cur_node) {
  CHILD(cur_node, tree->data[cur_end]) = ukkonen_node_make(cur_end, UEND, cur_node, tree);
  ukkonen_fill_suffix_hole(&(tree->ap.suffix_hole), tree->ap.point);
}

/** read in next character */
int ukkonen_ingest_next(UkkonenTree* tree) {
  tree->leaf_it++;
  int cur_end = tree->leaf_it;  // index of the last ingested character

  UkkonenStatus* ap = &(tree->ap);
  int cur_start = tree->ap.start_from;

  while (cur_start <= cur_end) {
    ukkonen_node_ensure_childs(ap->point);
    ukkonen_fix_ap(tree);

    // add tree->data[cur_start..cur_end] starting form ap
    UkkonenNode* cur_node = (ap->offset == 0 || ap->edge == NULL)
                                ? ap->point
                                : CHILD(ap->point, *ap->edge);
    if (ap->offset == 0) {
      ukkonen_node_ensure_childs(cur_node);
      // look at children (at node)
      if (NULL == CHILD(cur_node, tree->data[cur_end])) {
        // child does not exist -> make it
        ukkonen_add_child(cur_end, tree, cur_node);

        tree->ap.start_from = cur_start + 1;

        ap->point = ap->point->suffix_link;
        ap->edge = tree->data + cur_start;  // next char
      } else {
        ukkonen_fill_suffix_hole(&(tree->ap.suffix_hole), ap->point);
        ap->point = cur_node;
        ap->edge =
            tree->data +
            cur_end;     // We will continue with cur_end+1 as target next itter
        ap->offset = 1;  // was 0 (so same as ap->offset++)

        cur_start = cur_end;  // We are done with this one (break)
      }
    } else {
      // look at edge contents (on edge) not at node (ap->offset > 0)

      int cur_text_offset = cur_node->begin + ap->offset;
      if (tree->data[cur_text_offset] == tree->data[cur_end]) {
        // found on edge
        ap->point = ap->point;        // Keep same point
        ap->edge = ap->edge;          // and edge
        ap->offset = ap->offset + 1;  // but progress one char

        cur_start = cur_end;  // done

      } else {
        // mismatch on edge
        ukkonen_split_edge(tree, cur_node);
        tree->ap.start_from = cur_start + 1;

        if (ap->point == tree->root) {
          ap->point = ap->point;  // keep root as point
          ap->offset--;           // Offset reduced if in root
          ap->edge = tree->data +
                     tree->ap.start_from;  // index of next letter to start form
        } else {
          ap->point = ap->point->suffix_link;
          ap->edge = ap->edge;      // ap->edge remains
          ap->offset = ap->offset;  // With sane offset
        }
      }
    }

    cur_start++;
  }
  return 0;
}

#undef CHILD
