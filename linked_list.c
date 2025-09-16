#include "linked_list.h"
#include <stddef.h>
#include <stdint.h>

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

// Register malloc function
bool linked_list_register_malloc(void * (*malloc)(size_t)) {
    if (malloc == NULL) {
        return false;
    }
    malloc_fptr = malloc;
    return true;
}

// Register free function  
bool linked_list_register_free(void (*free)(void*)) {
    if (free == NULL) {
        return false;
    }
    free_fptr = free;
    return true;
}

// Creates a new linked_list
struct linked_list * linked_list_create(void) {
    if (malloc_fptr == NULL) {
        return NULL;
    }
    
    struct linked_list * ll = malloc_fptr(sizeof(struct linked_list));
    if (ll == NULL) {
        return NULL;
    }
    
    ll->head = NULL;
    return ll;
}

// Deletes a linked_list and frees all memory associated with it
bool linked_list_delete(struct linked_list * ll) {
    if (ll == NULL || free_fptr == NULL) {
        return false;
    }
    
    struct node * current = ll->head;
    while (current != NULL) {
        struct node * next = current->next;
        free_fptr(current);
        current = next;
    }
    
    free_fptr(ll);
    return true;
}

// Returns the size of a linked_list
size_t linked_list_size(struct linked_list * ll) {
    if (ll == NULL) {
        return SIZE_MAX;
    }
    
    size_t size = 0;
    struct node * current = ll->head;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    
    return size;
}

// Inserts an element at the end of the linked_list
bool linked_list_insert_end(struct linked_list * ll, unsigned int data) {
    if (ll == NULL || malloc_fptr == NULL) {
        return false;
    }
    
    struct node * new_node = malloc_fptr(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }
    
    new_node->data = data;
    new_node->next = NULL;
    
    if (ll->head == NULL) {
        ll->head = new_node;
    } else {
        struct node * current = ll->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    
    return true;
}

// Inserts an element at the front of the linked_list
bool linked_list_insert_front(struct linked_list * ll, unsigned int data) {
    if (ll == NULL || malloc_fptr == NULL) {
        return false;
    }
    
    struct node * new_node = malloc_fptr(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }
    
    new_node->data = data;
    new_node->next = ll->head;
    ll->head = new_node;
    
    return true;
}

// Inserts an element at a specified index in the linked_list
bool linked_list_insert(struct linked_list * ll, size_t index, unsigned int data) {
    if (ll == NULL || malloc_fptr == NULL) {
        return false;
    }
    
    // Special case: insert at front
    if (index == 0) {
        return linked_list_insert_front(ll, data);
    }
    
    // Find the node before the insertion point
    struct node * current = ll->head;
    for (size_t i = 0; i < index - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    // If index is out of bounds
    if (current == NULL) {
        return false;
    }
    
    struct node * new_node = malloc_fptr(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }
    
    new_node->data = data;
    new_node->next = current->next;
    current->next = new_node;
    
    return true;
}

// Finds the first occurrence of data and returns its index
size_t linked_list_find(struct linked_list * ll, unsigned int data) {
    if (ll == NULL) {
        return SIZE_MAX;
    }
    
    struct node * current = ll->head;
    size_t index = 0;
    
    while (current != NULL) {
        if (current->data == data) {
            return index;
        }
        current = current->next;
        index++;
    }
    
    return SIZE_MAX;
}

// Removes a node from the linked_list at a specific index
bool linked_list_remove(struct linked_list * ll, size_t index) {
    if (ll == NULL || ll->head == NULL || free_fptr == NULL) {
        return false;
    }
    
    // Special case: remove first node
    if (index == 0) {
        struct node * to_remove = ll->head;
        ll->head = ll->head->next;
        free_fptr(to_remove);
        return true;
    }
    
    // Find the node before the one to remove
    struct node * current = ll->head;
    for (size_t i = 0; i < index - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    // If index is out of bounds or no node to remove
    if (current == NULL || current->next == NULL) {
        return false;
    }
    
    struct node * to_remove = current->next;
    current->next = to_remove->next;
    free_fptr(to_remove);
    
    return true;
}

// Creates an iterator struct at a particular index
struct iterator * linked_list_create_iterator(struct linked_list * ll, size_t index) {
    if (ll == NULL || malloc_fptr == NULL) {
        return NULL;
    }
    
    // Find the node at the specified index
    struct node * current = ll->head;
    size_t current_index = 0;
    
    while (current != NULL && current_index < index) {
        current = current->next;
        current_index++;
    }
    
    // If index is out of bounds
    if (current == NULL) {
        return NULL;
    }
    
    struct iterator * iter = malloc_fptr(sizeof(struct iterator));
    if (iter == NULL) {
        return NULL;
    }
    
    iter->ll = ll;
    iter->current_node = current;
    iter->current_index = index;
    iter->data = current->data;
    
    return iter;
}

// Deletes an iterator struct
bool linked_list_delete_iterator(struct iterator * iter) {
    if (iter == NULL || free_fptr == NULL) {
        return false;
    }
    
    free_fptr(iter);
    return true;
}

// Iterates to the next node in the linked_list
bool linked_list_iterate(struct iterator * iter) {
    if (iter == NULL || iter->current_node == NULL) {
        return false;
    }
    
    iter->current_node = iter->current_node->next;
    if (iter->current_node == NULL) {
        return false;
    }
    
    iter->current_index++;
    iter->data = iter->current_node->data;
    
    return true;
}