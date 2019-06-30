
#ifndef __GROUP_H__
#define __GROUP_H__

#include "gconst.h"
//Group structure
typedef struct {
	int descriptor;
	int port;
	char addip[BUFFSIZE];
	char name[NAMESIZE];
} group;

/**
* *Display all attributes of a group
* \param g the group to display
*/
void displayGroup(group *g);

/**
* Create a group
* \param descriptor the descriptor associated to the group
* \param port the port associated to the group
* \param addip the addip associated to the group
* \param name the name associated to the group
* \return the created group
*/
group *makeGroup(int descriptor, int port, char* addip, char * name);

/**
* Create a group with no arguments (used when the group is created in another function)
* \return the created group with initialized attributes
*/
group *makeNullGroup();

/**
* Destroy a group
* \param g the group to destroy
*/
void destroyGroup(group *g);

/**
* Compare two groups
* \param g1 the first group to compare
* \param g2 the second group to compare
* \return 1 if they are the same alse 0
*/
int compareGroup(group * g1, group * g2);

#endif
