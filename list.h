
#ifndef __LISTE_H__
#define __LISTE_H__

#include "group.h"

//Basic linkedlist structure customed for groups (process)
typedef struct node {
	group *g;
	struct node *next;
	int size;
} List;

/**
* Init List
* \return a pointer to the initiated List
*/
List * makeList();

/**
* Add a group to the end of the List l
* \param g the group to add
* \param l the List where g will be added
*/
void add(group *g, List * l);

/**
* Delete a group from the List L
* \param g the group to delete
* \param L the List where g will be removed
* \return NULL if not found else the deleted group
*/
group* delete(group *g, List * L);

/**
* Add a group to the head of the List l
* \param g the group to add
* \param l the List where g will be added
*/
void add_head(group *g, List *l);

/**
* Remove the first group of the List l
* \param l the List where the first group will be removed
* \return the removed group if found else NULL
*/
group * remove_first(List * l);

/**
* What about having the first group of the list ?
* \param l the List to get the first group
* \return the first group of List l
*/
group * get_first(List * l);

/**
* What about having the last group of the list ?
* \param l the List to get the last group
* \return the last group of List l
*/
group * get_last(List * l);

/**
* Destroy a list and all it's element
* \param l the List to get destroy
*/
void destroy(List * l);

/**
* What about having the size of the list ?
* \param l the List to get size
* \return the size of the list (number of elements in l)
*/
int size(List * l);

/**
* Check if a descriptor is in the list L
* \param descriptor the descriptor to search
* \param L the List to search descriptor
* \return NULL if not found else the group with corresponding descriptor
*/
group* search_by_desc(int descriptor, List * L);

/**
* Check if a port is in the list L
* \param port the port to search
* \param L the List to search port
* \return NULL if not found else the group with corresponding port
*/
group* search_by_port(int port, List * L);

/**
* Check if a group is in the list
* \param group the group to check
* \param L the List to search group
* \return 0 if not found else 1
*/
int check_is_in(group * g, List * L);

/**
* Display all elements of the List L
* \param L the List to display
*/
void display(List * L);

/**
* Concatenate two lists next to each other
* \param L the List to search port
* \return a file descriptor set
*/
List * list_concat(List * l1, List * l2);

/**
* Convert a list of group to a file descriptor set
* \param L the List to search port
* \return a file descriptor set
*/
fd_set * listToFDSET(List * l);

#endif
