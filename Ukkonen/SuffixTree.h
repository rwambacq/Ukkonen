#ifndef _SUFFIXTREE_HEADER_
#define _SUFFIXTREE_HEADER_

typedef struct TreeNode TreeNode;
typedef struct ActivePointer ActivePointer;
typedef struct SimpleDFSLinkedList SimpleDFSLinkedList;

/*****************
*  Edge struct  *
*****************/
typedef struct TreeEdge {
	int rep_start;
	int rep_end;
	TreeNode* parent;
	TreeNode* child;
} TreeEdge;

/*****************
*  Node struct  *
*****************/
typedef struct TreeNode {
	int rep_start;
	int rep_end;
	/* De voorstelling wordt ook in de nodes bijgehouden om het makkelijker te maken bij de print functie.
	Dit kan ook uit de edges worden afgeleid, maar dat zorgt voor meer werk in de print functie.*/
	TreeEdge** edges;
	/* array voor edges, lengte is 128 voor alle standaard ascii karakters 
	(veronderstelling dat extended ascii niet wordt gebruikt)*/
} TreeNode;

/*****************
*  Tree struct  *
*****************/
typedef struct SuffixTree {
	TreeNode* root;
	char* string;
	int len;
} SuffixTree;

/* Information struct, needed for ukkonen */
typedef struct UkkonenInfo {
	int curr;
	ActivePointer* active_pointer;
	int remaining;
} UkkonenInfo;

/* Active pointer, maakt deel uit van het ukkonen info struct */
typedef struct ActivePointer {
	TreeNode* active_node;
	char active_edge;
	int active_length;
} ActivePointer;

// Linked list struct om te helpen bij indexering in print DFS.
typedef struct SimpleDFSLinkedList {
	TreeNode* node;
	SimpleDFSLinkedList* next;
} SimpleDFSLinkedList;

/*****************
* Help functions *
*****************/

char* allocate_string(int strlen);

SimpleDFSLinkedList* dfs_indexation(SimpleDFSLinkedList* node); 
// Maakt singly linked list van alle nodes in de boom in dfs volgorde

/*****************
* Main functies *
*****************/

/* Maak nieuwe boom aan */
SuffixTree* create_tree(char string[], int len);

/* Maak nieuwe edge aan */
TreeEdge* create_edge(int start, int end, TreeNode* parent, SuffixTree* tree);

/* Maak nieuwe node aan */
TreeNode* create_node(int start, int end);

/* Maak nieuwe info struct aan */
UkkonenInfo* create_info(TreeNode* root);

/* Maak nieuwe active pointer aan */
ActivePointer* create_active_pointer(TreeNode* root);

/* Maak LinkedList item aan met node als waarde */
SimpleDFSLinkedList* create_dfs_ll(TreeNode* node);

/* Verwijder de gegeven boom */
void free_tree(SuffixTree*);

/* Verwijder de gegeven edge */
void free_edge(TreeEdge*);

/* Verwijder de gegeven node */
void free_node(TreeNode*);

/* Verwijder de gegeven active pointer */
void free_active_pointer(ActivePointer*);

/* Verwijder het gegeven info struct */
void free_info(UkkonenInfo*);

/* Maak nieuwe suffixboom aan voor een gegeven string */
SuffixTree* ukkonen(char s[], int len);

/* Print de gegeven boom */
void print_tree(SuffixTree*);

#endif