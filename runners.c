
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <ifaddrs.h>
#include <netdb.h>


#include "gconst.h"
#include "gnetwork.h"
#include "list.h"
#include "group.h"
#include "utils.h"

//##############################################################################
//Functions to read and run commands from terminal
//##############################################################################

/**
* Read command from terminal (std_in)
* \param cmd where will we stored to command
* \param args where will be stored arguments
* \return number of args in cmd
*/
int readTermCommand(char * cmdIn, char ** args){
	memset(cmdIn, '\0', BUFFSIZE);

	//Reading terminal commands
	if(read(stdin_desc, cmdIn, BUFFSIZE) < 0){
		perror("readTermCommand read()");
		exit(EXIT_SUCCESS);
	}
	return parseArgs(cmdIn, args);
}

/**
* Run an attach command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param routing_list subtree of current process
*/
void runAttachCommand(char ** args, int nb_arg, group * localGroup, List * parents_list, List * routing_list){
	int nbPort = 0;

	if(nb_arg == 3){//check params
		nbPort = atoi(args[1]);

		//delete last character '\n' from ip address
		char *addip = args[2];
		deleteLineBreaks(addip);

		group * pgroup = getGroupFromIPPort(addip, nbPort, parents_list);
		if(pgroup == NULL){

			group * incoming_group = makeGroup(-1, nbPort, addip, "");
			if(compareGroup(localGroup, incoming_group) == 1){//client is trying to cnnect to himself
				printf("%sFailed to attach on %s:%i because you try to connect yourself\n%s",RED, addip, nbPort,RESET);

				return;
			}
			if(nbPort >= 0 || nbPort <= 65535){
				//run attach command on given params
				group * new_parent_group = makeNullGroup();
				if(attach(nbPort, addip, localGroup, new_parent_group, routing_list) == 1){
					//Add parent to list of parents
					add(new_parent_group, parents_list);
				}else{
					printf("%sFailed to attach on %s:%i\n%s",RED, addip, nbPort,RESET);
				}
			}

		}else{
			printf("%sYou are already connected to this group %s:%i\n%s",RED, addip, nbPort,RESET);
		}


	}else{
		printf("%sError args in /attach, use \'/attach [PARENT_PORT] [PARENT_IP]\'\n%s",RED,RESET);

	}
}

/**
* Run an attach command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param routing_list subtree of current process
*/
void runAttachCommandName(char ** args, int nb_arg, group * localGroup, List * parents_list, List * routing_list){
	//check if a name corresponding to a group is given
	//get the correspnding group
	//rebuilt the corresponding command
	//run the method like if ip:port was passed

	//It doesn't really have sens for attach since we don't know the name of the parent and can't attach to a child
	//It means we have to get the list of parents of the group => Amelioration
	//if(nb_arg == 2){
		//int nb_arg = 3; //cmd, port, ip
		//char cmd_args[nb_arg][255];

		//char * group_name = args[1];

		//find the corresponding group in list of parents of group (ones where the current group is not attached => not parents_list)
		//group * parent_group = //search here
		//cmd_args[0] = "/attach";
		//cmd_args[1] = parent_group->port; //to convert with sprintf
		//cmd_args[2] = parent_group->addip;

		//runAttachCommand(cmd_args, nb_arg, localGroup, parents_list, routing_list);
	//}
	printf("runAttachCommandName() Not done yet !\n");
}

/**
* Run an detach command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param children_list children of current process
*/
void runDetachCommand(char ** args, int nb_arg, group * localGroup, List * parents_list, List * children_list, List * routing_list){
	int nbPort = 0;
	char * ip;
	if(nb_arg==3){//check params
		nbPort = atoi(args[1]);
		ip = args[2];

		if(nbPort >= 0 || nbPort <= 65535){
			//run detach command on given params
			group * old_parent_group = search_by_port(nbPort, parents_list);
			if(old_parent_group != NULL && strcmp(old_parent_group->addip, ip) == 0) {
				if(detachFromParent(localGroup, old_parent_group, parents_list) == 1){
					//delete parent from list of parents
					delete(old_parent_group, parents_list);
				}else{
					printf("%sFailed to detach from %s:%i\n%s",RED, ip, nbPort,RESET);
				}
			}else{
				group * old_child_group = search_by_port(nbPort, children_list);
				if(old_child_group != NULL && strcmp(old_child_group->addip, ip) == 0) {
					if(detachFromChild(localGroup, old_child_group, parents_list) == 1){
						//delete parent from list of parents
						delete(old_child_group, children_list);
						if(check_is_in(old_child_group, routing_list) == 1){
							delete(old_child_group, routing_list);
						}
					}else{
						printf("%sFailed to detach from %s:%i\n%s",RED, ip, nbPort,RESET);
					}
				}else{
					printf("%sGroup with %s:%i not found in parents nor in children\n%s",RED, ip, nbPort,RESET);

				}
			}
		}
	}else{
		printf("%sError args in /detach, use \'/detach [GROUP_PORT] [GROUP_IP]\'\n%s",RED,RESET);
	}
}

/**
* Run an detach command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param parents_list parents of current process
* \param children_list children of current process
*/
void runDetachCommandName(char ** args, int nb_arg, group * localGroup, List * parents_list, List * children_list, List * routing_list){
	if(nb_arg == 2){
		int i = 0;
		int nb_arg_rebuilt = 3; //cmd, port, ip
		char ** cmd_args = calloc(nb_arg_rebuilt, sizeof(char *));
		for(i = 0; i < nb_arg_rebuilt; i++){
			cmd_args[i] = calloc(255, sizeof(char));
			memset(cmd_args[i], '\0', 255);

		}

		char * group_name = args[1];
		deleteLineBreaks(group_name);

		group * detach_group = getGroupFromName(group_name, parents_list);
		if(detach_group == NULL){
			detach_group = getGroupFromName(group_name, children_list);
			if(detach_group == NULL){
				printf("%sCan't find the group with name : %s\n%s",RED, group_name, RESET);
				return;
			}
		}

		char char_port[100];
		sprintf(char_port, "%d", detach_group->port);

		strcpy(cmd_args[0], "/detach");
		strcpy(cmd_args[1], char_port);
		strcpy(cmd_args[2], detach_group->addip);

		runDetachCommand(cmd_args, nb_arg_rebuilt, localGroup, parents_list, children_list, routing_list);
	}else{
		printf("%sError args in /detna, use \'/detna [GROUP_NAME]\'\n%s",RED,RESET);
	}
}

/**
* Run an msg command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runMsgCommand(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list){
	int nbPort = 0;
	char * ip;
	char * message;

	if(nb_arg == 4){//check params
		nbPort = atoi(args[1]);//group to send the msg
		ip = args[2];
		message = args[3];//message to send
		group * rcv_group = makeGroup(-1, nbPort, ip, "");//useless descriptor (-1) since we just use port and ip

		if(compareGroup(localGroup, rcv_group) == 1){
			//this is the group where we want to send, so send msg to children
			List * current_g_cell = children_list->next;
			while (current_g_cell != NULL) {
				group * dest_group = current_g_cell->g;
				msgRoot(message, localGroup, dest_group, rcv_group);// /!\ this is the correct method since we send to children
				current_g_cell = current_g_cell->next;
			}
		}else{
			//the group we are searching is in parents
			List * current_g_cell = parents_list->next;
			while (current_g_cell != NULL) {
				group * dest_group = current_g_cell->g;
				msgRoot(message, localGroup, dest_group, rcv_group);
				current_g_cell = current_g_cell->next;
			}
		}

	}else{
		printf("%sError args in /msg, use \'/msg [GROUP_PORT] [GROUP_IP] \"MSG\"\'\n%s",RED,RESET);

	}
}

/**
* Run an msg command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runMsgCommandName(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list){
	if(nb_arg == 3){
		int i = 0;
		int nb_arg_rebuilt = 4; //cmd, port, ip, msg
		char ** cmd_args = calloc(nb_arg_rebuilt, sizeof(char *));
		for(i = 0; i < nb_arg_rebuilt - 1; i++){
			cmd_args[i] = calloc(255, sizeof(char));
			memset(cmd_args[i], '\0', 255);
		}

		cmd_args[3] = calloc(strlen(args[2]), sizeof(char));
		memset(cmd_args[3], '\0', strlen(args[2]));

		char * group_name = args[1];
		char * msg = args[2];
		deleteLineBreaks(group_name);
		deleteLineBreaks(msg);

		group * detach_group = getGroupFromName(group_name, parents_list);
		if(detach_group == NULL){
			detach_group = getGroupFromName(group_name, children_list);
			if(detach_group == NULL){
				printf("%sCan't find the group with name : %s\n%s",RED, group_name, RESET);
				return;
			}
		}

		char char_port[100];
		sprintf(char_port, "%d", detach_group->port);

		strcpy(cmd_args[0], "/msg");
		strcpy(cmd_args[1], char_port);
		strcpy(cmd_args[2], detach_group->addip);
		strcpy(cmd_args[3], msg);

		runMsgCommand(cmd_args, nb_arg_rebuilt, localGroup, children_list, parents_list);
	}else{
		printf("%sError args in /msgna, use \'/msgna [GROUP_NAME] \"MSG\"\'\n%s",RED,RESET);
	}
}

/**
* Run an info command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runInfoCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list){
	printf("%sIP = %s\n%s",BOLDGREEN, localGroup->addip, RESET);
	printf("%sPORT = %i\n%s", BOLDGREEN, localGroup->port, RESET);
	printf("%sNAME = %s\n%s", BOLDGREEN, localGroup->name, RESET);
	printf("%s########## PARENTS ARE ##########%s\n", BOLD, RESET);
	display(parents_list);
	printf("%s########## CHILDREN ARE ##########%s\n", BOLD, RESET);
	display(children_list);
	printf("%s########## ROUTING ARE ##########%s\n", BOLD, RESET);
	display(routing_list);
}

/**
* Run an children command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runChildrenCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list){
	printf("########## CHILDREN ARE ##########\n");
	display(children_list);
}

/**
* Run an parents command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runParentsCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list){
	printf("########## PARENTS ARE ##########\n");
	display(parents_list);
}

/**
* Run an routes command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runRoutingCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list){
	printf("########## ROUTING ARE ##########\n");
	display(routing_list);
}

/**
* Run a file command using ip/port
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runFileCommand(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list){
	int nbPort = 0;
	int size;
	char * ip;
	char filename[BUFFSIZE];
	char * filecontent;
	char * message;

	if(nb_arg == 4){	//check params
		nbPort = atoi(args[1]);//group to send the msg
		ip = args[2];
		strcpy(filename,args[3]);

		FILE * file_to_send;
		file_to_send = fopen(filename, "rb");

		if (file_to_send != NULL){
	  	fseek (file_to_send, 0, SEEK_END);
	  	size = ftell (file_to_send);
	  	fseek (file_to_send, 0, SEEK_SET);
	  	filecontent = calloc(size,sizeof(char));
	  	fread (filecontent, sizeof(char), size, file_to_send);
	  	fclose (file_to_send);

			message = calloc(strlen(filename)+strlen(filecontent)+1,sizeof(char));

			strcpy(message,filename);
			strcat(message,"&");
			strcat(message,filecontent);

			//message to send
			group * rcv_group = makeGroup(-1, nbPort, ip, "");//useless descriptor (-1) since we just use port and ip

			if(compareGroup(localGroup, rcv_group) == 1){
				//this is the group where we want to send, so send msg to children
				List * current_g_cell = children_list->next;
				while (current_g_cell != NULL) {
					group * dest_group = current_g_cell->g;
					fileRoot(message, localGroup, dest_group, rcv_group);// /!\ this is the correct method since we send to children
					current_g_cell = current_g_cell->next;
				}
			}else{
				//the group we are searching is in parents
				List * current_g_cell = parents_list->next;
				while (current_g_cell != NULL) {
					group * dest_group = current_g_cell->g;
					fileRoot(message, localGroup, dest_group, rcv_group);
					current_g_cell = current_g_cell->next;
				}
			}

		}else{
			printf("%sVerify that GROUP_PORT and GROUP_IP are correct !\n%s",RED,RESET);
		}
	}else{
		printf("%sError args in /file, use \'/file [GROUP_PORT] [GROUP_IP] \"FILENAME\"\'\n%s",RED,RESET);
	}
}


/**
* Run a file command using name
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runFileCommandName(char ** args, int nb_arg, group * localGroup, List * children_list, List * parents_list){
	if(nb_arg == 3){
		int i = 0;
		int nb_arg_rebuilt = 4; //cmd, port, ip, msg
		char ** cmd_args = calloc(nb_arg_rebuilt, sizeof(char *));
		for(i = 0; i < nb_arg_rebuilt; i++){
			cmd_args[i] = calloc(255, sizeof(char));
			bzero(cmd_args[i], 255);
		}

		char * group_name = args[1];
		char * msg = args[2];
		deleteLineBreaks(group_name);
		deleteLineBreaks(msg);

		group * detach_group = getGroupFromName(group_name, parents_list);
		if(detach_group == NULL){
			detach_group = getGroupFromName(group_name, children_list);
			if(detach_group == NULL){
				printf("Can't find the group with name : %s\n", group_name);
				return;
			}
		}

		char char_port[100];
		sprintf(char_port, "%d", detach_group->port);

		strcpy(cmd_args[0], "/file");
		strcpy(cmd_args[1], char_port);
		strcpy(cmd_args[2], detach_group->addip);
		strcpy(cmd_args[3], msg);

		runFileCommand(cmd_args, nb_arg_rebuilt, localGroup, children_list, parents_list);
	}else{
		printf("Error args in /filena, use \'/filena [GROUP_NAME] \"FILENAME\"\'\n");
	}
}

/**
* Run an help command
* \param args args of the command, the first one is the command identifier (/attach, /detach ...)
* \param nb_arg number of args of the command
* \param localGroup the current process
* \param routing_list subtree of current process
* \param children_list children of current process
* \param parents_list parents of current process
*/
void runHelpCommand(char ** args, int nb_arg, group * localGroup, List * routing_list, List * children_list, List * parents_list){
	printf("\e[1m/attach [PARENT_PORT] [PARENT_IP] \e[0m\n\t=> attach current process to a group by its ip/port\n");
	printf("\e[1m/attna [PARENT_NAME] \e[0m\n\t=> attach current process to a group by its name /!\\ NOT IMPLEMENTED YET\n");
	printf("\e[1m/detach [GROUP_PORT] [GROUP_IP] \e[0m\n\t=> detach current process from another (parent or child) by its port\n");
	printf("\e[1m/detna [GROUP_NAME] \e[0m\n\t=> detach current process from another (parent or child) by its name\n");
	printf("\e[1m/msg [GROUP_PORT] [GROUP_IP] \"MSG\" \e[0m\n\t=> send a message to a group using ip/port of the group, all children of this group will receive this message\n");
	printf("\e[1m/msgna [GROUP_NAME] \"MSG\" \e[0m\n\t=> send a message to a group using name of the group, all children of this group will receive this message\n");
	printf("\e[1m/file [GROUP_PORT] [GROUP_IP] \"FILENAME\" \e[0m\n\t=> send a file to a group using ip/port of the group, all children of this group will receive this file\n");
	printf("\e[1m/filena [GROUP_NAME] \"FILENAME\" \e[0m\n\t=> send a file to a group using name of the group, all children of this group will receive this file\n");
	printf("\e[1m/info \e[0m\n\t=> show current process name, ip, port and more\n");
	printf("\e[1m/children \e[0m\n\t=> show children of current process\n");
	printf("\e[1m/parents \e[0m\n\t=> show parents of current process\n");
	printf("\e[1m/routes \e[0m\n\t=> show routes (children and children of children and ...) of current process\n");
	printf("\e[1m/help \e[0m\n\t=> show this help message\n");
	printf("\e[1m/exit \e[0m\n\t=> stop current process, children will attach to all of current parents\n");
}
