#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#include "gconst.h"
#include "group.h"

#define PTYPE_MSG 1			//msg goes to a group
#define PTYPE_MSGROOT 2		//msg goes to the root group
#define PTYPE_ATTACH 3		//attach
#define PTYPE_BRDCST 4 		//broadcast
#define PTYPE_DETACHFC 5 	//detach from child
#define PTYPE_DETACHFP 6 	//detach from parent
#define PTYPE_NOTIFATT 7 	//notif of attach
#define PTYPE_NOTIFDET 8	//notif of detach
#define PTYPE_FILEROOT 9 	//file goes to a group
#define PTYPE_FILE 10		//file goes to the root group


//payload structure
typedef struct {
	int ptype;
	int sender_port;
	char sender_ip[IPSIZE];
	char sender_name[NAMESIZE];
	int recver_port;
	char recver_ip[IPSIZE];
	char recver_name[NAMESIZE];
	char content[150*BUFFSIZE];
} payload;

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
payload * createPayload(int ptype, int sender_port, char* sender_ip, char* sender_name, int recver_port, char* recver_ip, char* recver_name, char * content);

/**
* Create a payload
* \param ptype the type of the payload : PTYPE_MSG, PTYPE_MSGROOT, PTYPE_ATTACH, PTYPE_BRDCST, PTYPE_DETACHFC, PTYPE_DETACHFP, PTYPE_NOTIFATT, PTYPE_NOTIFDET
* \param sender_group the sender group
* \param recver_group the sender group
* \param content content of the payload
* \return the created payload
*/
payload * createPayloadFromGroups(int ptype, group * sender_group, group * recver_group, char * content);

/**
* Create a payload with initialized values wich will be filled after
*/
payload * createEmptyPayload();

/**
* Show attributes of a paylaod
* \param p the payload to show
*/
void displayPayload(payload * p);

#endif
