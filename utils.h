#ifndef __UTILS_H__
#define __UTILS_H__

#include "list.h"
#include "payload.h"
#include <arpa/inet.h>
#include <string.h>

/**
* Parse a command
* \param cmd the command to parse
* \param args where will be stored arguments
* \return number of args in cmd
*/
int parseArgs(char * cmd, char * args[]);

/**
* Get IP from an interface name
* \param iface the interface name
* \param addr wher will be stored the ip
*/
void getIP(char* iface, struct in_addr *addr);

/**
* Check if the current process is root of a group
* \param parents_list parents of current group
* \return 1 if current group is a root else 0
*/
int isRoot(List * parents_list);

/**
* Add a payload to an history file
* \param p the payload to write
*/
void historilog(payload *p);

/**
* Retrieve a group from it's name in list l
* \param group_name the name of the oayload to find
* \param l the list to retrieve group_name
* \return the group if found else NULL
*/
group * getGroupFromName(char * group_name, List * l);

/**
* Retrieve a group from it's ip/port
* \param ip the ip of the oayload to find
* \param port the port of the oayload to find
* \param l the list to retrieve group_name
* \return the group if found else NULL
*/
group * getGroupFromIPPort(char * ip, int port, List * l);

/**
* Delete all useless \n in a string
* \param str the string where \n will be deleted
*/
void deleteLineBreaks(char * str);

void loadScreen(char** screen, FILE* fichier);

#endif
