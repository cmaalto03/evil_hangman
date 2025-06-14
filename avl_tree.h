#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "status.h"
#include "my_string.h"
#include "generic_vector.h"

typedef void* AVL_TREE;

// Precondition: None
// Postcondition: Allocate space for an avl_tree with root set to NULL
AVL_TREE avl_tree_init_default(void);

// Precondition: hAvl_tree is a valid AVL_TREE object, hKey is a valid MY_STRING object representing a word family's key from
// get_key_word_value, and hString is a valid MY_STRING object represented in dictionary.txt
// Postcondition: If hKey is not already in the AVL_TREE, allocate a new node with key hKey and a default GENERIC_VECTOR, and allocate
// a new copy of hString in the GENERIC_VECTOR. If hKey is already in the tree, find its place, and push the copy
// of hString into the GENERIC_VECTOR. Return SUCCESS on allocation success, and failure if malloc fails.
Status avl_tree_insert(AVL_TREE hAvl_tree,  MY_STRING hKey, MY_STRING hString);

// Precondition: hAvl_tree is a valid AVL_TREE, key is a valid MY_STRING object, and pStatus is either a Status* or NULL
// Postcondition: Traverses the AVL_TREE and returns the GENERIC_VECTOR assosiated with key. Changes pStatus to SUCCESS if found,
// if it is present, and FAILURE if not found
GENERIC_VECTOR avl_tree_search(AVL_TREE hAvl_tree, MY_STRING key, Status* pStatus);

// Precondition: phAvl_tree is a pointer to a valid AVL_TREE object
// Postcondition: Frees memory assosiated with the AVL_TREE object. Traverses the tree post-order, and frees each GENERIC_VECTOR assosiated
// with the key, as well as that node
void avl_tree_destroy(AVL_TREE* phAvl_tree);

// Precondition: hAvl_tree is a valid AVL_TREE object
// Postcondition: Prints the key assosiated with each node in the AVL_TREE, which is a valid MY_STRING object.
void avl_tree_traverse(AVL_TREE hAvl_tree);

// Precondition: hAvl_tree is a valid AVL_TREE object
// Postcondition: Prints the key assosiated with each node in the AVL_TREE, as well as the entire contents of each GENERIC_VECTOR
// in the respective node
void avl_tree_traverse_with_key(AVL_TREE hAvl_tree);

// Precondition: hAvl_tree is a valid AVL_TREE object, and key is a valid MY_STRING object that may or may not be in the tree as a key
// Postcondition: Frees memory assosiated with key by destroying that particular node, as well aas the data within that node as a GENERIC_VECTOR.
// Returns SUCCESS if the node is found and deleted, and FAILURE if that key is not assosiated with a key in the tree.
Status avl_tree_delete(AVL_TREE hAvl_tree, MY_STRING key);

// Precondition: hAvl_tree is a non-empty valid AVL_TREE object, hString_key is a key that is assosiated with a node within the AVL_TREE object
// Postcondition: Calls upon an interal function that traverses the tree, and stores a pointer to the largest GENERIC_VECTOR, as well as a pointer 
// to that particular word family key. Returns a deep copy of the largest word_family GENERIC_VECTOR, as well as a deep copy of the largest
// word family's key in hString_key
GENERIC_VECTOR avl_tree_find_largest(AVL_TREE hAvl_tree, MY_STRING hString_key);

#endif
