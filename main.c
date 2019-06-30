

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
#include <signal.h>

#include "gconst.h"
#include "gnetwork.h"
#include "list.h"
#include "group.h"
#include "utils.h"
#include "runners.h"
#include "handlers.h"

//#############################################################################
//This is the main function narvalo
//##############################################################################
List * parents_list; //parents' ports list
List * children_list; //children's ports list
List * routing_list; //list of all children's group and childrens of childrens etc..., this is a list which contain below group
group * localGroup;

void customExit(){
	//printf("Exiting by sending /detach payload to children...\n");
	List * current_g_cell = children_list->next;
	while (current_g_cell != NULL) {
		group * dest_group = current_g_cell->g;
		detachFromChild(localGroup, dest_group, parents_list);
		current_g_cell = current_g_cell->next;
	}
	exit(EXIT_SUCCESS);

}

void sighandler(int signum) {
	customExit();
}

int main(int argc, char ** argv){

	if (argc != 3){
		errno = EINVAL;
		perror("Error args, use ./app [INTERFACE_NAME] [GROUP_NAME]");
		exit(EXIT_FAILURE);
	}

	int i;
	FILE* startPage = fopen("hellomsg.txt","r");
	if (startPage == NULL){
		perror("A file is missing, re-download the repo !");
		exit(EXIT_FAILURE);
	}
	char* str_startPage[LIMITX];
	for(i=0;i<LIMITX;i++){
		str_startPage[i] = calloc(LIMITY,sizeof(char));
	}
	loadScreen(str_startPage,startPage);
	fclose(startPage);

	for(i = 0; i < LIMITX ; i++){
		printf("%s%s%s",BOLD,str_startPage[i],BOLD);
	}
	printf ("\n\n     %s /!\\ Use command \"/help\" to see all available commands /!\\ %s\n\n",RED,RESET);
	printf("\n	##################################################\n");

	char * iface = argv[1];
	char * gname = argv[2];

	parents_list = makeList(); //parents' ports list
	children_list = makeList(); //children's ports list
	routing_list = makeList(); //all process in the subtree

	signal(SIGINT, sighandler);

	pid_t current_pid = getpid();

	//Create an entry of type SOCK_STREAM to allow communication ---------------
	struct sockaddr_in localSockAddr;
	localSockAddr.sin_family = AF_INET;
	//getIP(iface, &localSockAddr.sin_addr);
	inet_aton(argv[1], (struct in_addr *) &localSockAddr.sin_addr);
	//localSockAddr.sin_port = htons(12000);//A random port is given by os

	int localSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(localSocket == -1){
		perror("socket()");
		exit(EXIT_SUCCESS);
	}

	if(bind(localSocket, (struct sockaddr*)&localSockAddr, sizeof(localSockAddr)) != 0){
		perror("bind()");
		exit(EXIT_SUCCESS);
	}

	socklen_t lengthLocalSockAddr = sizeof(struct sockaddr_in);
	if(getsockname(localSocket, (struct sockaddr*)&localSockAddr, &lengthLocalSockAddr) == -1){
		perror("getsockname()");
		exit(EXIT_FAILURE);
	}

	if(listen(localSocket, 50) != 0){
		perror("listen()");
		exit(EXIT_SUCCESS);
	}

	localGroup = makeGroup(localSocket, ntohs(localSockAddr.sin_port), inet_ntoa(localSockAddr.sin_addr), gname);
	printf("	%sPID = %i\n%s",BOLDGREEN, current_pid, RESET);
	printf("	%sIP = %s\n%s",BOLDGREEN, inet_ntoa(localSockAddr.sin_addr), RESET);
	printf("	%sPORT = %i\n%s", BOLDGREEN, ntohs(localSockAddr.sin_port), RESET);
	printf("	%sNAME = %s\n%s", BOLDGREEN, gname, RESET);
	printf("	##################################################\n");


	//Now the server is ready to receive things --------------------------------

	//Which contains commands to send to groups from terminal
	char cmdIn[BUFFSIZE];

	//Behind is the requests/commands handler ----------------------------------
	while(1){

		printf("\n\n%sWait for action or enter command :%s\n", BOLD, RESET);

		//Create FD_SET for select
		//group with random port only used to handle stdin port
		group * term_group = makeGroup(stdin_desc, -1,"127.0.0.1", "STDIN");
		List * allgroups = list_concat(parents_list, children_list);
		add(localGroup, allgroups);
		add(term_group,allgroups);
		fd_set * fds = listToFDSET(allgroups);

		if((select(FD_SETSIZE, fds, NULL, NULL, NULL)) < 0){
			perror("select()");
		}

		int i = 0;
		for (i = 0; i < size(allgroups) + 2; ++i){
			if (FD_ISSET(i, fds)){

				if (i == stdin_desc){
					//a command has been entered in stdin, so read it and exec it
					char * args[BUFFSIZE];
					int nb_arg = readTermCommand(cmdIn, args);
					char * command = args[0];
					memset(cmdIn, '\0', BUFFSIZE); //bzero(cmdIn,BUFFSIZE);
					deleteLineBreaks(command);

					if(strcmp(command, "/attach") == 0){
						runAttachCommand(args, nb_arg, localGroup, parents_list, routing_list);
					}else if(strcmp(command, "/attna") == 0){//attach name
						runAttachCommandName(args, nb_arg, localGroup, parents_list, routing_list);
					}else if(strcmp(command, "/detach") == 0){
						runDetachCommand(args, nb_arg, localGroup, parents_list, children_list, routing_list);
					}else if(strcmp(command, "/detna") == 0){
						runDetachCommandName(args, nb_arg, localGroup, parents_list, children_list, routing_list);
					}else if(strcmp(command, "/msg") == 0){
						runMsgCommand(args, nb_arg, localGroup, children_list, parents_list);
					}else if(strcmp(command, "/msgna") == 0){
						runMsgCommandName(args, nb_arg, localGroup, children_list, parents_list);
					}else if(strcmp(command, "/file") == 0){
						runFileCommand(args, nb_arg, localGroup, children_list, parents_list);
					}else if(strcmp(command, "/filena") == 0){
						runFileCommandName(args, nb_arg, localGroup, children_list, parents_list);
					}else if(strcmp(command, "/info") == 0){
						runInfoCommand(args, nb_arg, localGroup, routing_list, children_list, parents_list);
					}else if(strcmp(command, "/children") == 0){
						runChildrenCommand(args, nb_arg, localGroup, routing_list, children_list, parents_list);
					}else if(strcmp(command, "/parents") == 0){
						runParentsCommand(args, nb_arg, localGroup, routing_list, children_list, parents_list);
					}else if(strcmp(command, "/routes") == 0){
						runRoutingCommand(args, nb_arg, localGroup, routing_list, children_list, parents_list);
					}else if(strcmp(command, "/help") == 0){
						runHelpCommand(args, nb_arg, localGroup, routing_list, children_list, parents_list);
					}else if(strcmp(command, "/exit") == 0){
						customExit();
					}else{
						printf("-> Lu :[%s] => command = [%s]\n", cmdIn, command);
					}

				}else if (i == localSocket){
					//This is a new client

					struct sockaddr_in extSockAddr;
					socklen_t lengthExtSockAddr = sizeof(struct sockaddr_in);
					int ear = accept(localSocket, (struct sockaddr*)&extSockAddr, &lengthExtSockAddr);

					group * new_child_group = makeGroup(ear, -1, "", "");

					//Preparing to receive a payload
					payload * read_p = createEmptyPayload();
					if(readPayload(read_p, new_child_group) == 0){
						perror("ERROR receiving new client payload");
					}

					//Because the ear_port returned by ntohs(extSockAddr.sin_port); is wrong
					new_child_group->port = read_p->sender_port;
					strcpy(new_child_group->addip,inet_ntoa(extSockAddr.sin_addr));
					strcpy(new_child_group->name,read_p->sender_name);

					//Handle payload commands
					if(read_p->ptype == PTYPE_ATTACH){
						//This is an attach command, client wants to connect
						handeNewClientAttach(read_p, new_child_group, localGroup, children_list, parents_list, routing_list);
					}/*else if (...) for other commands*/

				}else{
					//This is an already connected client

					//Find the group which correspond to descriptor (i)
					group * incoming_group = search_by_desc(i, children_list);
					if(incoming_group == NULL){
						incoming_group = search_by_desc(i, parents_list);
					}

					//Preparing to receive a payload
					payload * read_p = createEmptyPayload();
					int read = readPayload(read_p, incoming_group);
					if(read == 0){
						perror("ERROR receiving already connected payload");
					}else if(read == 2){
						// Lost connection with client (Brutal interruption)
						printf("Connection lost with group : \n");
						displayGroup(incoming_group);
						//create custom paylaod since the payload is not well packaged
						payload * read_p = createPayloadFromGroups(PTYPE_DETACHFC, incoming_group, localGroup, "");
						handleDetach(read_p, localGroup, children_list, parents_list, routing_list);
						close(incoming_group->descriptor);
						break;
					}

					if(read_p->sender_port !=0){

						//Handle payload commands
						if(read_p->ptype == PTYPE_DETACHFC || read_p->ptype == PTYPE_DETACHFP){
							handleDetach(read_p, localGroup, children_list, parents_list, routing_list);
						}else if(read_p->ptype == PTYPE_MSG || read_p->ptype == PTYPE_MSGROOT){
							handleMsg(read_p, localGroup, children_list, parents_list, routing_list);
						}else if(read_p->ptype == PTYPE_NOTIFATT){
							handleNotifAttach(read_p, localGroup, parents_list, routing_list);
						}else if(read_p->ptype == PTYPE_NOTIFDET){
							handleNotifDetach(read_p, localGroup, parents_list, routing_list);
						}else if(read_p->ptype == PTYPE_FILEROOT || read_p->ptype == PTYPE_FILE){
							handleFile(read_p, localGroup, children_list, parents_list, routing_list);
						}/*else if (...) for other commands*/
					}
				}
			}
		}
	}

	exit(EXIT_SUCCESS);
}
