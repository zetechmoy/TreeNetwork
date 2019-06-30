
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "payload.h"
#include "group.h"

/**
* Create a payload
* \param ptype the type of the payload : PTYPE_MSG, PTYPE_MSGROOT, PTYPE_ATTACH, PTYPE_BRDCST, PTYPE_DETACHFC, PTYPE_DETACHFP, PTYPE_NOTIFATT, PTYPE_NOTIFDET
* \param sender_port the port of the sender group
* \param sender_ip the ip of the sender group
* \param sender_name the name of the sender group
* \param recver_port the port of the receiver group
* \param recver_ip the ip of the receiver group
* \param recver_name the name of the receiver group
* \param content content of the payload
* \return the created payload
*/
payload * createPayload(int ptype, int sender_port, char* sender_ip, char* sender_name, int recver_port, char* recver_ip, char* recver_name, char * content){
	payload * p = (payload *) malloc(sizeof(payload));
	p->ptype = ptype;

	p->sender_port = sender_port;
	strcpy(p->sender_ip, sender_ip);
	strcpy(p->sender_name, sender_name);

	p->recver_port = recver_port;
	strcpy(p->recver_ip, recver_ip);
	strcpy(p->recver_name, recver_name);

	strcpy(p->content, content);
	return p;
}

/**
* Create a payload
* \param ptype the type of the payload : PTYPE_MSG, PTYPE_MSGROOT, PTYPE_ATTACH, PTYPE_BRDCST, PTYPE_DETACHFC, PTYPE_DETACHFP, PTYPE_NOTIFATT, PTYPE_NOTIFDET
* \param sender_group the sender group
* \param recver_group the sender group
* \param content content of the payload
* \return the created payload
*/
payload * createPayloadFromGroups(int ptype, group * sender_group, group * recver_group, char * content){
	return createPayload(ptype, sender_group->port, sender_group->addip, sender_group->name, recver_group->port, recver_group->addip, recver_group->name, content);
}

/**
* Create a payload with initialized values wich will be filled after
*/
payload * createEmptyPayload(){
	payload * p = (payload *) malloc(sizeof(payload));
	p->ptype = PTYPE_MSG;
	p->sender_port = -1;
	strcpy(p->sender_ip, "");
	strcpy(p->sender_name, "");
	p->recver_port = -1;
	strcpy(p->recver_ip, "");
	strcpy(p->recver_name, "");
	strcpy(p->content, "");
	return p;
}

/**
* Show attributes of a paylaod
* \param p the payload to show
*/
void displayPayload(payload * p){

	if(p->ptype == PTYPE_MSG){
		printf("Payload Type : PTYPE_MSG\n");
	}else if(p->ptype == PTYPE_ATTACH){
		printf("Payload Type : PTYPE_ATTACH\n");
	}else if(p->ptype == PTYPE_BRDCST){
		printf("Payload Type : PTYPE_BRDCST\n");
	}else if(p->ptype == PTYPE_DETACHFP){
		printf("Payload Type : PTYPE_DETACHFP\n");
	}else if(p->ptype == PTYPE_DETACHFC){
		printf("Payload Type : PTYPE_DETACHFC\n");
	}else if(p->ptype == PTYPE_MSGROOT){
		printf("Payload Type : PTYPE_MSGROOT\n");
	}
	printf("Sender (%s) : %s:%i\n", p->sender_name, p->sender_ip, p->sender_port);
	printf("Receiver (%s) : %s:%i\n", p->recver_name, p->recver_ip, p->recver_port);
	printf("Content : %s\n", p->content);
}
