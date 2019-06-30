
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
//Functions to handle received commands (These are server functions in fact)
//##############################################################################


/**
* Handle a payload of type attach
* \param read_p the received payload
* \param new_child_group the new incoming group
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handeNewClientAttach(payload * read_p, group * new_child_group, group * localGroup, List * children_list, List * parents_list, List * routing_list){
	//just to know what's happening when using
	printf("Received /attach command from %s:%i (%s)\n", new_child_group->addip, new_child_group->port, new_child_group->name);

	group * route_group = search_by_port(new_child_group->port, routing_list);
	if(check_is_in(new_child_group, routing_list) == 1 && route_group != NULL && route_group->descriptor != -1){//if new client is already in routing list
		printf("%sFailed to attach on %s:%i because already in children\n%s",RED, new_child_group->addip, new_child_group->port, RESET);

		//displayGroup(new_child_group);
		close(new_child_group->descriptor);
	}else{
		//send a response to the new client with name of the local group
		payload * send_p = createPayloadFromGroups(PTYPE_ATTACH, localGroup, new_child_group, "");
		if(sendPayload(send_p, new_child_group) == 0){
			perror("ERROR handeNewClientAttach() sendPayload()");
			return;
		}

		//read routing_list from attach and add them to the current routing list
		if(strlen(read_p->content) > 0){
			char * ipn[BUFFSIZE];
			int nb_arg = parseArgs(read_p->content, ipn);
			//for each of parent of dsconnected parent, let connect to it
			int i = 0;
			for(i = 0; i < nb_arg; i = i + 3){

				int port = atoi(ipn[i]);
				char * ip = ipn[i+1];
				char * name = ipn[i+2];

				group * route_group = makeGroup(-1, port, ip, name);
				add(route_group, routing_list);
			}
		}


		add(new_child_group, children_list);
		group * g;
		if((g = search_by_port(new_child_group->port, routing_list)) != NULL){
			delete(g, routing_list);
		}
		notifyAttach(new_child_group, localGroup, parents_list, routing_list);
	}

}

/**
* Handle a payload of type detach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleDetach(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list){

	group * out_group;
	if((out_group = search_by_port(read_p->sender_port, children_list)) != NULL){
		//displayGroup(out_group);
		delete(out_group, children_list);
		notifyDetach(out_group, localGroup, parents_list, routing_list);
		if (close(out_group->descriptor) < 0){
			perror("ERROR handleDetach()");
			exit(0);
		}
	}else if((out_group = search_by_port(read_p->sender_port, parents_list)) != NULL){
		//displayGroup(out_group);
		delete(out_group, parents_list);
		if (close(out_group->descriptor) < 0){
			perror("ERROR handleDetach()");
			exit(0);
		}
	}

	if(read_p->ptype == PTYPE_DETACHFC){
		//just to know what's happening when using
		printf("Received /detach (from child) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);


		//get parent's extremities of parent (grand parent)
		if(strlen(read_p->content) > 0){
			char * ip_ports[BUFFSIZE];
			int nb_arg = parseArgs(read_p->content, ip_ports);
			//for each of parent of dsconnected parent, let connect to it
			int i = 0;
			for(i = 0; i < nb_arg; i = i + 3){

				int port = atoi(ip_ports[i]);
				char * ip = ip_ports[i+1];
				//char * name = ip_ports[i+2]; //unused

				group * new_parent_group = makeNullGroup();
				if(attach(port, ip, localGroup, new_parent_group, routing_list) == 1){
					//Add parent to list of parents
					add(new_parent_group, parents_list);
				}else{
					printf("%sFailed to attach on %s:%i\n%s",RED, ip, port, RESET);
				}
			}
		}
	}else{
		//just to know what's happening when using
		printf("Received /detach (from parent) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);
	}


}

/**
* Handle a payload of type msg
* \param read_p the received payload
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleMsg(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list){

	char * message = read_p->content;

	//group * snd_group = makeGroup(-1, read_p->sender_port, read_p->sender_ip);//useless descriptor (-1) since we just use port and ip
	group * rcv_group = makeGroup(-1, read_p->recver_port, read_p->recver_ip, read_p->recver_name);//useless descriptor (-1) since we just use port and ip
	group * snd_group = makeGroup(-1, read_p->sender_port, read_p->sender_ip, read_p->sender_name);//useless descriptor (-1) since we just use port and ip

	//routing msg
	if(read_p->ptype == PTYPE_MSGROOT && compareGroup(localGroup, rcv_group) == 1){
		//snd_group->descriptor = localGroup->descriptor;//modify snd_descriptor so that we can have the sender group info at the end and the correct descriptor for sending
		//just to know what's happening when using
		printf("Received /msg (root reached, diffusing) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);
		printf("%sGot message from (%s) %s:%i: \n--> %s%s%s\n", YELLOW,read_p->sender_name,read_p->sender_ip, read_p->sender_port, BOLD,message,RESET);



		//we've reach the group so we have to send the msg to children
		List * current_g_cell = children_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			msgGroup(message, localGroup, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}else if(read_p->ptype == PTYPE_MSGROOT && compareGroup(localGroup, rcv_group) == 0){
		//just to know what's happening when using
		printf("Received /msg (root not reached) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);

		//we are searching for the group but not reached so sending to parents
		List * current_g_cell = parents_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			msgRoot(message, localGroup, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}else if(read_p->ptype == PTYPE_MSG){
		//just to know what's happening when using
		printf("Received /msg (root reached, child, diffusing) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);

		printf("%sGot message from (%s) %s:%i: \n--> %s%s%s\n", YELLOW,read_p->sender_name,read_p->sender_ip, read_p->sender_port, BOLD,message,RESET);
		snd_group->descriptor = localGroup->descriptor;//modify snd_descriptor so that we can have the sender group info at the end and the correct descriptor for sending

		//send the msg to children and print it
		List * current_g_cell = children_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			msgGroup(message, snd_group, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}
}

/**
* Handle a payload of type notif attach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleNotifAttach(payload * read_p, group * localGroup, List * parents_list, List * routing_list){
	//just to know what's happening when using
	printf("Received notif attach from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);

	char * ipn[BUFFSIZE];
	parseArgs(read_p->content, ipn);
	int port = atoi(ipn[0]);
	char * ip = ipn[1];
	char * name = ipn[2];

	group * the_group = makeGroup(-1, port, ip, name);
	//int notifyAttach(group * new_group, group * local_group, List * parents_list, List * routing_list){
	notifyAttach(the_group, localGroup, parents_list, routing_list);
}

/**
* Handle a payload of type notif detach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleNotifDetach(payload * read_p, group * localGroup, List * parents_list, List * routing_list){
	//just to know what's happening when using
	printf("Received notif detach from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);

	char * ipn[BUFFSIZE];
	parseArgs(read_p->content, ipn);
	int port = atoi(ipn[0]);
	char * ip = ipn[1];
	char * name = ipn[2];

	group * the_group = makeGroup(-1, port, ip, name);
	//int notifyDetach(group * new_group, group * local_group, List * parents_list, List * routing_list){
	notifyDetach(the_group, localGroup, parents_list, routing_list);
}

void handleFile(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list){

	char * message = read_p->content;

	//group * snd_group = makeGroup(-1, read_p->sender_port, read_p->sender_ip);//useless descriptor (-1) since we just use port and ip
	group * rcv_group = makeGroup(-1, read_p->recver_port, read_p->recver_ip, read_p->recver_name);//useless descriptor (-1) since we just use port and ip
	group * snd_group = makeGroup(-1, read_p->sender_port, read_p->sender_ip, read_p->sender_name);//useless descriptor (-1) since we just use port and ip

	//routing msg
	if(read_p->ptype == PTYPE_FILEROOT && compareGroup(localGroup, rcv_group) == 1){
		int i, parse;
		char* filename;

		for (i=0;i<strlen(message);i++){
			if (message[i] == '&'){
				parse = ++i;
				break;
			}
		}

		filename = calloc(parse,sizeof(char));

		for (i=0;i<parse-1;i++){
			filename[i] = message[i];
		}
		//snd_group->descriptor = localGroup->descriptor;//modify snd_descriptor so that we can have the sender group info at the end and the correct descriptor for sending
		printf("Received /file (root reached, diffusing) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);
		printf("%sGot file from (%s) %s:%i: \n--> %s%s%s\n", YELLOW,read_p->sender_name,read_p->sender_ip, read_p->sender_port, BOLD,filename,RESET);

		//we've reach the group so we have to send the msg to children
		List * current_g_cell = children_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			fileGroup(message, localGroup, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}else if(read_p->ptype == PTYPE_FILEROOT && compareGroup(localGroup, rcv_group) == 0){
		//we are searching for the group but not reached so sending to parents
		printf("Received /file (root not reached) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);

		List * current_g_cell = parents_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			fileRoot(message, localGroup, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}else if(read_p->ptype == PTYPE_FILE){



		int i, parse,content_size=0;
		char* filename;
		char* filecontent;

		for (i=0;i<strlen(message);i++){
			if (message[i] == '&'){
				parse = ++i;
				break;
			}
		}

		filename = calloc(parse,sizeof(char));

		for (i=0;i<parse-1;i++){
			filename[i] = message[i];
		}

		for (i=parse;i<strlen(message);i++){
			content_size++;
		}

		filecontent = calloc(content_size,sizeof(char));

		for (i=parse;i<strlen(message);i++){
			filecontent[i-parse] = message[i];
		}

		printf("Received /file (root reached, child, diffusing) command from %s:%i (%s)\n", read_p->sender_ip, read_p->sender_port, read_p->sender_name);
		printf("%sGot file from (%s) %s:%i: \n--> %s%s%s\n", YELLOW,read_p->sender_name,read_p->sender_ip, read_p->sender_port, BOLD,filename,RESET);

		FILE* fileReceived;
		fileReceived = fopen(filename,"wb");

		if (fileReceived != NULL){
			fwrite(filecontent,sizeof(char),content_size, fileReceived);
			fclose(fileReceived);
		}else{
			perror("Error creating file");
		}

		snd_group->descriptor = localGroup->descriptor;//modify snd_descriptor so that we can have the sender group info at the end and the correct descriptor for sending

		//send the msg to children and print it
		List * current_g_cell = children_list->next;
		while (current_g_cell != NULL) {
			group * dest_group = current_g_cell->g;
			fileGroup(message, snd_group, dest_group, rcv_group);
			current_g_cell = current_g_cell->next;
		}
	}
}
