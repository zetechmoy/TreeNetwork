#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "group.h"
#include <string.h>

List * makeList(){
	List *temp = (List *) malloc(sizeof(List));
	temp->g = NULL;
	temp->next = NULL;
	temp->size = 0;
	return temp;
}

void add(group *g, List * l){
	// On augmente la taille
	l->size++;

	// Puis on ajoute
	if (l->next != NULL){
		add(g,l->next);
	}else{
		l->next = (List *) malloc(sizeof(List));
		l->next->g = g;
		l->next->next = NULL;
	}
}

group* delete(group *g, List * L){

	List * current_g_cell = L->next;
	List * previous_g_cell = NULL;
	while (current_g_cell->g != NULL) {
		if(current_g_cell->g->descriptor == g->descriptor && current_g_cell->g->port == g->port){
			break;
		}else{
			previous_g_cell = current_g_cell;
			current_g_cell = current_g_cell->next;
		}
	}

	if(previous_g_cell == NULL){
		return remove_first(L);
	}

	if(current_g_cell != NULL && previous_g_cell != NULL){
		previous_g_cell->next = current_g_cell->next;
		L->size--;
		return current_g_cell->g;
	}

	return NULL;
}

void add_head(group *g, List *l){
	List *nouv = (List *) malloc(sizeof(List));
	nouv->g = g;
	nouv->next = l->next;
	l->next = nouv;
	l->size++;
}

group * remove_first(List * l){
	group *g;
	g = l->next->g;
	l->next = l->next->next;
	l->size--;
	return g;
}

group * get_first(List * l){
	if (l->next != NULL){
		return l->next->g;
	}else{
		return NULL;
	}
}

group * get_last(List * l){
	if (l->next != NULL){
		return get_last(l->next);
	}else{
		return l->g;
	}
}

void destroy(List * l){
	if(l->next != NULL){
		destroy(l->next);
	}

	if(l->g != NULL) {
		destroyGroup(l->g);
	}else {
		l->size = 0;
	}
	free(l);
}

int size(List * l){
	return l->size;
}

group* search_by_desc(int descriptor, List * L){

	List * current_g_cell = L->next;
	while (current_g_cell != NULL) {
		if(current_g_cell->g->descriptor == descriptor){
			return current_g_cell->g;
		}
		current_g_cell = current_g_cell->next;
	}

	return NULL;

}

group* search_by_port(int port, List * L){
	List * current_g_cell = L->next;
	while (current_g_cell != NULL) {
		if(current_g_cell->g->port == port){
			return current_g_cell->g;
		}
		current_g_cell = current_g_cell->next;
	}

	return NULL;

}

void display(List * L){
	List * current_g_cell = L->next;
	while (current_g_cell != NULL) {
		displayGroup(current_g_cell->g);
		current_g_cell = current_g_cell->next;
	}
}

int check_is_in(group * g, List * L){
	List * current_g_cell = L->next;
	while (current_g_cell != NULL) {
		if(compareGroup(current_g_cell->g, g) == 1){
			return 1;
		}
		current_g_cell = current_g_cell->next;
	}
	return 0;
}

List * list_concat(List * l1, List * l2){
	List * nl = makeList();

	List * current_g_cell = l1->next;
	while (current_g_cell != NULL) {
		add(current_g_cell->g, nl);
		current_g_cell = current_g_cell->next;
	}

	current_g_cell = l2->next;
	while (current_g_cell != NULL) {
		add(current_g_cell->g, nl);
		current_g_cell = current_g_cell->next;
	}

	return nl;
}

fd_set * listToFDSET(List * l){

	fd_set * readset = calloc(1, sizeof(fd_set));

	FD_ZERO(readset);

	List * current_g_cell = l->next;
	while (current_g_cell != NULL) {
		FD_SET(current_g_cell->g->descriptor, readset);
		current_g_cell = current_g_cell->next;
	}

	return readset;
}
