#ifndef __RUNNERS_H__
#define __RUNNERS_H__

/**
* Read command from terminal (std_in)
* \param cmd where will we stored to command
* \param args where will be stored arguments
* \return number of args in cmd
*/
int readTermCommand(char * cmdIn, char ** args);

/**
* Run an attach command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param routing_list subtree of current process
*/
void runAttachCommand(char ** args, int nb_arg, group * localGroup, List * parents_list, List * routing_list);

/**
* Run an attach command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param routing_list subtree of current process
*/
void runAttachCommandName(char ** args, int nb_arg, group * localGroup, List * parents_list, List * routing_list);

/**
* Run an detach command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param children_list children of current process
*/
void runDetachCommand(char ** args, int nb_arg, group * localGroup, List * parents_list, List * children_list, List * routing_list);

/**
* Run an detach command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param children_list children of current process
*/
void runDetachCommandName(char ** args, int nb_arg, group * localGroup, List * parents_list, List * children_list, List * routing_list);

/**
* Run an broadcast command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
*/
void runBroadcastCommandName(char ** args, int nb_arg, group * localGroup, List * children_list);

/**
* Run an msg command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runMsgCommand(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list);

/**
* Run an msg command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runMsgCommandName(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list);

/**
* Run an info command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runInfoCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list);

/**
* Run an children command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runChildrenCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list);

/**
* Run an parents command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runParentsCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list);

/**
* Run an routes command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runRoutingCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list);

void runFileCommand(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list);
void runFileCommandName(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list);

/**
* Run an help command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runHelpCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list);

#endif
