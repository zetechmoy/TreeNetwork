
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gconst.h"
#include "gnetwork.h"
#include "group.h"
#include "payload.h"
#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
* Attach the current process to another process with port parent_port, it can be used to attach a child to a parent or a parent to a child
* \param parent_port the port of the process to attach
* \param child_group the child to attach
* \param parent_group the parent group
* \return 0 if error, and 1 if success
*/
int attach(int parent_port, char * parent_ip, group * child_group, group * parent_group, List * routing_list){
	//WTF THERE ARE \n in ip ?!
	int i = 0;
	for(i = 0; i < strlen(parent_ip); i++){
		if(parent_ip[i] == '\n'){
			parent_ip[i] = ' ';
		}
	}
	//printf("attach() : attaching on address %s:%i\n", parent_ip, parent_port);

	//Build the parent extremity
	struct sockaddr_in parentAddr;
	parentAddr.sin_family = AF_INET;
	inet_aton(parent_ip,(struct in_addr *)&parentAddr.sin_addr);
	parentAddr.sin_port = htons(parent_port);

	int parent_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (parent_descriptor < 0){
		perror("ERROR attach() socket()");
		return 0;
	}

	parent_group->descriptor = parent_descriptor;
	parent_group->port = parent_port;
	strcpy(parent_group->addip, parent_ip);

	if (connect(parent_descriptor,(struct sockaddr *) &parentAddr, sizeof(parentAddr)) < 0){
		perror("ERROR attach() connect()");
		close(parent_descriptor);
		return 0;
	}

	payload * send_p;
	if(routing_list != NULL){
		char str_routing[1024];
		memset(str_routing, '\0', 1024);
		//create content for /detach
		List * current_g_cell = routing_list->next;
		while (current_g_cell != NULL) {
			group * g = current_g_cell->g;
			char char_arr [100];
			int num = g->port;
			sprintf(char_arr, "%d", num);
			strcat(str_routing, char_arr);
			strcat(str_routing, " ");
			strcat(str_routing, g->addip);
			strcat(str_routing, " ");
			strcat(str_routing, g->name);
			strcat(str_routing, " ");
			current_g_cell = current_g_cell->next;
		}

		//Send connection init (/attach)
		send_p = createPayloadFromGroups(PTYPE_ATTACH, child_group, parent_group, str_routing);
	}else{
		send_p = createPayloadFromGroups(PTYPE_ATTACH, child_group, parent_group, "");
	}

	//printf("### ATTACH CONNECT PAYLOAD ###\n");
	//displayPayload(send_p);
	if(sendPayload(send_p, parent_group) == 0){
		perror("ERROR attach() sendPayload()");
		return 0;
	}

	payload * read_p = createEmptyPayload();
	if(readPayload(read_p, parent_group) == 0){
		perror("ERROR attach() readPayload()");
		return 0;
	}
	//displayPayload(read_p);
	strcpy(parent_group->name, read_p->sender_name);

	historilog(send_p);
	return 1;
}


/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param detach_type detach from parent or detach from child ?
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detach(group * child_group, group * parent_group, int detach_type, List * parents_list){


	char str_local_parent[1024];
	memset(str_local_parent, '\0', 1024);

	//create content for /detach
	List * current_g_cell = parents_list->next;
	while (current_g_cell != NULL) {
		group * g = current_g_cell->g;
		char char_arr [100];
		int num = g->port;
		sprintf(char_arr, "%d", num);
		strcat(str_local_parent, char_arr);
		strcat(str_local_parent, " ");
		strcat(str_local_parent, g->addip);
		strcat(str_local_parent, " ");
		strcat(str_local_parent, g->name);
		strcat(str_local_parent, " ");
		current_g_cell = current_g_cell->next;
	}
	//Send deconnection init (/detach)
	payload * send_p = createPayloadFromGroups(detach_type, child_group, parent_group, str_local_parent);

	if(sendPayload(send_p, parent_group) == 0){
		perror("ERROR detach() sendDescriptor()");
		return 0;
	}

	if (close(parent_group->descriptor) < 0){
		perror("ERROR detach() disconnect()");
		return 0;
	}

	historilog(send_p);
	return 1;
}

/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detachFromParent(group * child_group, group * parent_group, List * parents_list){
	return detach(child_group, parent_group, PTYPE_DETACHFP, parents_list);
}

/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detachFromChild(group * child_group, group * parent_group, List * parents_list){
	return detach(child_group, parent_group, PTYPE_DETACHFC, parents_list);
}

/**
* Send a payload to a group
* \param p payload to send
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendPayload(payload * p, group * dest_group){
	int resp;
	if((resp = write(dest_group->descriptor, p, sizeof(payload))) > 0){
		return 1;
	}
	else{
		perror("sendPayload()");
		return 0;
	}
}

/**
* Read a payload from a group
* \param p payload to read
* \param recv_group where the payload will be read
* \return 0 if error, and 1 if success
*/
int readPayload(payload * p, group * recv_group){
	int resp;
	if((resp = recv(recv_group->descriptor, p, sizeof(payload),MSG_WAITALL)) > 0){
		return 1;
	}else if((resp = recv(recv_group->descriptor, p, sizeof(payload),MSG_WAITALL)) == 0){
		return 2;
	}
	else{
		perror("sendPayload()");
		return 0;
	}
}

/**
* Send a string to a group
* \param str string to send
* \param msg_type the type of message (CMD/MSG...)
* \param src_group the group which send the string
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendString(char * str, int msg_type, group * src_group, group * dest_group){
	payload * p = createPayloadFromGroups(msg_type, src_group, dest_group, str);
	return sendPayload(p, dest_group);
}

/**
* Read a string from a group
* \param str string to read
* \param msg_type the type of message (CMD/MSG...)
* \param src_group the group which send the string
* \param dest_group where the payload will be read
* \return 0 if error, and 1 if success
*/
int readString(char * str, int msg_type, group * src_group, group * dest_group){
	payload * p = createPayloadFromGroups(msg_type, src_group, dest_group, str);
	return readPayload(p, dest_group);
}

/**
* Send a message to a group
* \param msg message to send
* \param src_group the group which send the string
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendMsg(char * msg, group * src_group, group * dest_group){
	return sendString(msg, PTYPE_MSG, src_group, dest_group);
}

/**
* Read a message from a group
* \param msg message to send
* \param src_group the group which receive the string
* \param dest_group the group which has sent the string
* \return 0 if error, and 1 if success
*/
int readMsg(char * msg, group * src_group, group * dest_group){
	return readString(msg, PTYPE_MSG, src_group, dest_group);
}

/**
* Send en msg to every child of a group
* \param msg message to send
* \param from the local group from where i sent the broadcast
* \param dest_group the group where the message will be sent to each children
* \return 0 if error, and 1 if success
*/
int broadcast(char * msg, group * from, group * dest_group){
	//printf("broadcast() : sending from %s:%i to %s:%i with %s\n",from->addip,from->port,dest_group->addip, dest_group->port, msg);

	payload * brd_p = createPayloadFromGroups(PTYPE_BRDCST, from, dest_group, msg);
	//printf("########## BRDCST PAYLOAD ##########\n");
	//displayPayload(brd_p);
	if(sendPayload(brd_p, dest_group) == 0){
		perror("ERROR broadcast() sendDescriptor()");
		return 0;
	}
	historilog(brd_p);
	return 1;
}

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \return 0 if error, and 1 if success
*/
int msgRoot(char * message, group * from, group * dest_group, group * final_dest_group){
	return msg(message, from, dest_group, final_dest_group, PTYPE_MSGROOT);
}

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \return 0 if error, and 1 if success
*/
int msgGroup(char * message, group * from, group * dest_group, group * final_dest_group){
	return msg(message, from, dest_group, final_dest_group, PTYPE_MSG);
}

/**
* Send a file to a group
* \param msg file to send (name + content)
* \param from the local group from where the file is sent
* \param dest_group the group where the file will be sent to pass through the tree
* \param final_dest_group the group where the file will be handled
* \return 0 if error, and 1 if success
*/
int fileRoot(char * message, group * from, group * dest_group, group * final_dest_group){
	return msg(message, from, dest_group, final_dest_group, PTYPE_FILEROOT);
}

/**
* Send a file to a group
* \param msg file to send
* \param from the local group from where the file is sent
* \param dest_group the group where the file will be sent to pass through the tree
* \param final_dest_group the group where the file will be handled
* \return 0 if error, and 1 if success
*/
int fileGroup(char * message, group * from, group * dest_group, group * final_dest_group){
	return msg(message, from, dest_group, final_dest_group, PTYPE_FILE);
}

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \param msg_type MSG_ROOT (going top in the tree) or MSG_GRP (going down in the tree)
* \return 0 if error, and 1 if success
*/
int msg(char * msg, group * from, group * dest_group, group * final_dest_group, int msg_type){

	//printf("msg() : sending from %s:%i to %s:%i '%s'\n",from->addip,from->port,dest_group->addip, dest_group->port, msg);

	payload * pl = createPayloadFromGroups(msg_type, from, final_dest_group, msg);

	if(sendPayload(pl, dest_group) == 0){
		perror("ERROR msg() sendDescriptor()");
		return 0;
	}
	historilog(pl);
	return 1;
}

/**
* Send an attach notification to a group telling what's happening in sub-tree
* \param incoming_group the group which is arriving in the tree (passed through the tree)
* \param local_group the current process
* \param parents_list parents of current process
* \param routing_list subtree of group of current process
* \return 0 if error, and 1 if success
*/
int notifyAttach(group * new_group, group * local_group, List * parents_list, List * routing_list){
	add(new_group, routing_list);

	char str_group[1024];
	memset(str_group, '\0', 1024);
	char char_arr[100];

	int num = new_group->port;
	sprintf(char_arr, "%d", num);
	strcat(str_group, char_arr);
	strcat(str_group, " ");
	strcat(str_group, new_group->addip);
	strcat(str_group, " ");
	strcat(str_group, new_group->name);
	strcat(str_group, " ");

	//send a notify payload to each of parents to update their routing list
	List * current_g_cell = parents_list->next;
	while (current_g_cell != NULL) {
		group * parent_group = current_g_cell->g;

		payload * pl = createPayloadFromGroups(PTYPE_NOTIFATT, local_group, parent_group, str_group);

		if(sendPayload(pl, parent_group) == 0){
			perror("ERROR notifyAttach()");
			return 0;
		}

		current_g_cell = current_g_cell->next;
	}

	return 1;
}

/**
* Send a detach notification to a group telling what's happening in sub-tree
* \param new_group the group which is leaving in the tree (passed through the tree)
* \param local_group the current process
* \param parents_list parents of current process
* \param routing_list subtree of group of current process
* \return 0 if error, and 1 if success
*/
int notifyDetach(group * new_group, group * local_group, List * parents_list, List * routing_list){
	delete(new_group, routing_list);

	//send a notify payload to each of parents to update their routing list
	char str_group[1024];
	memset(str_group, '\0', 1024);
	char char_arr[100];

	int num = new_group->port;
	sprintf(char_arr, "%d", num);
	strcat(str_group, char_arr);
	strcat(str_group, " ");
	strcat(str_group, new_group->addip);
	strcat(str_group, " ");
	strcat(str_group, new_group->name);
	strcat(str_group, " ");

	//send a notify payload to each of parents to update their routing list
	List * current_g_cell = parents_list->next;
	while (current_g_cell != NULL) {
		group * parent_group = current_g_cell->g;

		payload * pl = createPayloadFromGroups(PTYPE_NOTIFDET, local_group, parent_group, str_group);

		//printf("########## notifyDetach PAYLOAD ##########\n");
		//displayPayload(pl);

		if(sendPayload(pl, parent_group) == 0){
			perror("ERROR notifyDetach()");
			return 0;
		}

		current_g_cell = current_g_cell->next;
	}

	return 1;
}
