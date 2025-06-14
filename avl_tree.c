
#include "avl_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include "generic_vector.h"
#include "my_string.h"

struct node;    

typedef struct node Node;

struct node
{
    MY_STRING key;
    GENERIC_VECTOR data;
    Node* left;
    Node* right;
    int height;
};

struct avl_tree
{
    Node* root;
};

typedef struct avl_tree Avl_tree;


ITEM vector_my_string_init_copy2(ITEM hMy_string)
{

  MY_STRING hCopy = my_string_init_copy(hMy_string);

  return hCopy;
}

void vector_my_string_destroy2(ITEM* phMy_string)
{
  MY_STRING hMy_string = (MY_STRING)*phMy_string;

  my_string_destroy(&hMy_string);
}

// utils for insertion
int get_max(int x, int y)
{
    return x > y ? x : y;
}

int get_subtree_height(Node* node)
{
    if (node == NULL)
    {
        return 0;
    }

   return get_max(get_subtree_height(node->right), get_subtree_height(node->left) )+ 1;
}

int get_height(Node* node) {
    return node ? node->height : -1;
}

int get_balance(Node* node) {
    return node ? get_height(node->right) - get_height(node->left) : 0;
}

void update_height(Node* node) {
    
    node->height = get_max(get_height(node->left), get_height(node->right)) + 1;
}

Node* make_node(MY_STRING key)
{
    Node* pNew = (Node*)malloc(sizeof(Node));

    if (pNew != NULL)
    {
        pNew->key = my_string_init_copy(key);
        pNew->data = generic_vector_init_default(vector_my_string_init_copy2, vector_my_string_destroy2);
        pNew->left = NULL;
        pNew->right = NULL;
        pNew->height = 0;
    }

    return pNew;
}

AVL_TREE avl_tree_init_default(void)
{
    Avl_tree* pAvl_tree = (Avl_tree*)malloc(sizeof(Avl_tree));

    if (pAvl_tree != NULL)
    {
        pAvl_tree->root = NULL;
    }

    return pAvl_tree;
}

void right_rotation(Node** root)
{
    Node* prior_root = *root;
    Node* new_root = prior_root->left;

    prior_root->left = new_root->right;
    new_root->right = prior_root;

    *root = new_root;
    update_height(prior_root);
    update_height(new_root);
}

void left_rotation(Node** root)
{
    Node* prior_root = *root;
    Node* new_root = prior_root->right;

    prior_root->right = new_root->left;
    new_root->left = prior_root;

    *root = new_root;

    update_height(prior_root);
    update_height(new_root);
}

void right_left_rotation(Node** root)
{
    right_rotation(&(*root)->right);
    left_rotation(root);
}

void left_right_rotation(Node** root)
{
    left_rotation(&(*root)->left);
    right_rotation(root);
}

void avl_tree_node_insert(Node** root, MY_STRING key, MY_STRING data)
{
    if(!(*root))
    {
        *root = (Node*)make_node(key);
        generic_vector_push_back((*root)->data, data);

        // make node with VECTOR_GENERIC *
        return;
    }

    if (my_string_compare(key, (*root)->key) == 0)
    {        
        generic_vector_push_back((*root)->data, data);

        return;
        // insert into the array
    }

    if (my_string_compare(key, (*root)->key) > 0)
    {

        avl_tree_node_insert(&(*root)->right, key, data);

    }
    if (my_string_compare(key, (*root)->key) < 0)

    {
        avl_tree_node_insert(&(*root)->left, key, data);
    }
    update_height(*root);

    int balance = get_balance(*root);

    // LR Double Rotation + RR
    if (balance == -2)
    {

        int sub_balance = get_balance((*root)->left);
        if (sub_balance == 1)
            left_right_rotation(root);
        
        else
            // Right Rotation
            right_rotation(root);
    }

    // RL Double Rotation + LL
    if (balance == 2)
    {
        int sub_balance = get_balance((*root)->right);

        if (sub_balance == -1)
        {
            right_left_rotation(root);
        }

        else
        {
            // Left Rotation
            left_rotation(root);
        }
    }
}

Status avl_tree_insert(AVL_TREE hAvl_tree, MY_STRING hKey, MY_STRING hString)
{

    Avl_tree* pAvl_tree = (Avl_tree*)hAvl_tree;

    avl_tree_node_insert(&pAvl_tree->root, hKey, hString);

    
    return SUCCESS;
}

GENERIC_VECTOR avl_tree_node_search(Node* root, MY_STRING key, Status* pStatus)
{
    if (!root)
    {
        if(pStatus)
            *pStatus = FAILURE;

        return NULL;
    }

    if (my_string_compare(root->key, key) == 0)
    {
        if(pStatus)
            *pStatus = SUCCESS;

        return root->data;
    }

    if (my_string_compare(key, root->key) > 0)
    {
        return avl_tree_node_search(root->right, key, pStatus);
    }

    if (my_string_compare(key, root->key)< 0)
    {
        return avl_tree_node_search(root->left, key, pStatus);
    }

    // catch all
    else
    {
        *pStatus = FAILURE;
        return NULL;

    }
}

GENERIC_VECTOR avl_tree_search(AVL_TREE hAvl_tree, MY_STRING key, Status* pStatus)
{
    Avl_tree* pAvl_tree = (Avl_tree*)hAvl_tree;
    

    return avl_tree_node_search(pAvl_tree->root, key, pStatus);
}


Status avl_tree_node_delete(Node** root, MY_STRING key)
{
    if(!(*root))
    {
        return FAILURE;
    }

    // found, so free the memory, and fix the tree
    if ( my_string_compare((*root)->key, key) == 0)
    {
        Node* delete_me = *root;
        // can just delete it
        if (delete_me->right == NULL && delete_me->left == NULL)
        {
            free(*root);
            *root = NULL;   // don't forget about dangling pointer !
        }
        // singular child
        else if (!(*root)->left || !(*root)->right) 
        {

            Node* temp = (*root)->left ? (*root)->left : (*root)->right;
            free(*root);
            *root = temp;
        }
        else
        {
            Node* temp = *root; 
            Node* parent = temp;
            delete_me = temp->right;
        
            while (delete_me->left != NULL)
            {
                parent = delete_me;
                delete_me = delete_me->left;
            }
            
            (*root)->key = delete_me->key;
            (*root)->data = delete_me->data;
        
            if (parent->left == delete_me)
            {
                parent->left = delete_me->right;  
            }

            else
            {
              
                parent->right = delete_me->right;  
            }
            free(delete_me);
        }

        update_height(*root);


        int balance = get_balance(*root);

        // LR Double Rotation + RR
        if (balance == -2)
        {
    
            int sub_balance = get_balance((*root)->left);
            if (sub_balance == 1)
                left_right_rotation(root);
            
            else
                // Right Rotation
                right_rotation(root);
        }
    
        // RL Double Rotation + LL
        if (balance == 2)
        {
            int sub_balance = get_balance((*root)->right);
    
            if (sub_balance == -1)
            {
                right_left_rotation(root);
            }
    
            else
            {
                // Left Rotation
                left_rotation(root);
            }
        }

        return SUCCESS;
    }
    if (my_string_compare(key, (*root)->key) > 0)
    {
        avl_tree_node_delete(&(*root)->right, key);
    }
   if (my_string_compare(key, (*root)->key) <0)

    {
        avl_tree_node_delete(&(*root)->left, key);
    }
}

Status avl_tree_delete(AVL_TREE hAvl_tree, MY_STRING key)
{
    Avl_tree* pAvl_tree = (Avl_tree*)hAvl_tree;

    return avl_tree_node_delete(&pAvl_tree->root, key);
}
void avl_tree_node_traverse(Node* root)
{
    if (!root)
        return;
            
    avl_tree_node_traverse(root->left);
    printf("%s with size of %d\n", my_string_c_str(root->key), generic_vector_get_size(root->data));
    avl_tree_node_traverse(root->right);
    
}

void avl_tree_traverse(AVL_TREE hAvl_tree)
{
    Avl_tree* pAvl_tree = (Avl_tree*)hAvl_tree;
    avl_tree_node_traverse(pAvl_tree->root);
}

//prints all of the possible words
void avl_tree_node_traverse_with_key(Node* root)
{
    if (!root)
        return;
            
    avl_tree_node_traverse_with_key(root->left);

   //printf("%s\n", my_string_c_str(root->key));



    for(int i = 0; i < generic_vector_get_size(root->data); i++)
    {
        printf("%s: %s\n", my_string_c_str(*generic_vector_at(root->data, i)), my_string_c_str(root->key));
    }
    avl_tree_node_traverse_with_key(root->right);
    
}
void avl_tree_traverse_with_key(AVL_TREE hAvl_tree)
{
    Avl_tree* pAvl_tree = (Avl_tree*)hAvl_tree;
    avl_tree_node_traverse_with_key(pAvl_tree->root);
}

void avl_tree_node_destroy(Node* root)
{
    if (!root)
        return;
    avl_tree_node_destroy(root->left);
    avl_tree_node_destroy(root->right);

    generic_vector_destroy(&(root->data));
    my_string_destroy(&(root->key));
    free(root);

}

void avl_tree_destroy(AVL_TREE* phAvl_tree)
{
    Avl_tree* pAvl_tree = (Avl_tree*)*phAvl_tree;
    avl_tree_node_destroy(pAvl_tree->root);
    free(pAvl_tree);
}


void node_find_largest(Node* root, int* largestSize, GENERIC_VECTOR* largestVector, MY_STRING hString_key)
{
    if (!root) 
        return;


    node_find_largest(root->left, largestSize, largestVector, hString_key);

    int currentSize = generic_vector_get_size(root->data);
 if (currentSize > *largestSize)
{
    *largestSize = currentSize;

    *largestVector = root->data;

    my_string_assignment(hString_key, root->key);

}

    node_find_largest(root->right, largestSize, largestVector, hString_key);
}


GENERIC_VECTOR avl_tree_find_largest(AVL_TREE hAvl_tree, MY_STRING hString_key)
{
    Avl_tree* pAvl = (Avl_tree*)hAvl_tree;

    int largestSize = 0;
    GENERIC_VECTOR largestVector = NULL;

    node_find_largest(pAvl->root, &largestSize, &largestVector, hString_key);


    // return a copy of the largest

    GENERIC_VECTOR deep_copy =  generic_vector_init_default(vector_my_string_init_copy2, vector_my_string_destroy2);

    for(int i = 0; i < generic_vector_get_size(largestVector); i++)
    {
        generic_vector_push_back(deep_copy, *generic_vector_at(largestVector, i));
    }

    return deep_copy;
} 

