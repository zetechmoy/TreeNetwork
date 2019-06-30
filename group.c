
#include "group.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

/*
* *Display all attributes of a group
* \param g the group to display
*/
void displayGroup(group *g){
	printf("group : name = %s, desc = %i, port = %i, ip = %s\n", g->name, g->descriptor, g->port,g->addip);
}

/**
* Create a group
* \param descriptor the descriptor associated to the group
* \param port the port associated to the group
* \param addip the addip associated to the group
* \param name the name associated to the group
* \return the created group
*/
group *makeGroup(int descriptor, int port, char* addip, char * name){
	group *temp = (group *) malloc(sizeof(group));
	temp->descriptor = descriptor;
	temp->port = port;
	strcpy(temp->addip, addip);
	strcpy(temp->name, name);
	return temp;
}

/**
* Create a group with no arguments (used when the group is created in another function)
* \return the created group with initialized attributes
*/
group *makeNullGroup(){
	group *temp = (group *) malloc(sizeof(group));
	return temp;
}

/**
* Destroy a group
* \param g the group to destroy
*/
void destroyGroup(group *g){
	free(g);
}

/**
* Compare two groups
* \param g1 the first group to compare
* \param g2 the second group to compare
* \return 1 if they are the same alse 0
*/
int compareGroup(group * g1, group * g2){
	if(g1->port == g2->port && strcmp(g1->addip, g2->addip) == 0){
		return 1;
	}
	return 0;
}
