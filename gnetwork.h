#ifndef __GNETWORK_H__
#define __GNETWORK_H__

#include "group.h"
#include "payload.h"
#include "list.h"

/**
* Attach the current process to another process with port parent_port, it can be used to attach a child to a parent or a parent to a child
* \param parent_port the port of the process to attach
* \param child_group the child to attach
* \param parent_group the parent group
* \return 0 if error, and 1 if success
*/
int attach(int parent_port, char * parent_ip, group * child_group, group * parent_group, List * routing_list);

/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param detach_type detach from parent or detach from child ?
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detach(group * child_group, group * parent_group, int detach_type, List * parents_list);

/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detachFromParent(group * child_group, group * parent_group, List * parents_list);

/**
* Detach the current process from another process with port parent_port
* \param child_group the child to attach
* \param parent_group the parent group
* \param parents_list list of process' parents
* \return 0 if error, and 1 if success
*/
int detachFromChild(group * child_group, group * parent_group, List * parents_list);

/**
* Send a payload to a group
* \param p payload to send
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendPayload(payload * p, group * dest_group);


/**
* Read a payload from a group
* \param p payload to read
* \param recv_group where the payload will be read
* \return 0 if error, and 1 if success
*/
int readPayload(payload * p, group * recv_group);

/**
* Send a string to a group
* \param str string to send
* \param msg_type the type of message (CMD/MSG...)
* \param src_group the group which send the string
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendString(char * str, int msg_type, group * src_group, group * dest_group);

/**
* Read a string from a group
* \param str string to read
* \param msg_type the type of message (CMD/MSG...)
* \param src_group the group which send the string
* \param dest_group where the payload will be read
* \return 0 if error, and 1 if success
*/
int readString(char * str, int msg_type, group * src_group, group * dest_group);

/**
* Send a message to a group
* \param msg message to send
* \param src_group the group which send the string
* \param dest_group where the payload will be sent
* \return 0 if error, and 1 if success
*/
int sendMsg(char * msg, group * src_group, group * dest_group);

/**
* Read a message from a group
* \param msg message to send
* \param src_group the group which receive the string
* \param dest_group the group which has sent the string
* \return 0 if error, and 1 if success
*/
int readMsg(char * msg, group * src_group, group * dest_group);

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \return 0 if error, and 1 if success
*/
int msg(char * msg, group * from, group * dest_group, group * final_dest_group, int msg_type);

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \return 0 if error, and 1 if success
*/
int msgRoot(char * msg, group * from, group * dest_group, group * final_dest_group);

/**
* Send a msg to a group
* \param msg message to send
* \param from the local group from where the msg is sent
* \param dest_group the group where the message will be sent to pass through the tree
* \param final_dest_group the group where the msg will be handled
* \param msg_type MSG_ROOT (going top in the tree) or MSG_GRP (going down in the tree)
* \return 0 if error, and 1 if success
*/
int msgGroup(char * msg, group * from, group * dest_group, group * final_dest_group);

/**
* Send a file to a group
* \param msg file to send (name + content)
* \param from the local group from where the file is sent
* \param dest_group the group where the file will be sent to pass through the tree
* \param final_dest_group the group where the file will be handled
* \return 0 if error, and 1 if success
*/
int fileRoot(char * message, group * from, group * dest_group, group * final_dest_group);

/**
* Send a file to a group
* \param msg file to send
* \param from the local group from where the file is sent
* \param dest_group the group where the file will be sent to pass through the tree
* \param final_dest_group the group where the file will be handled
* \return 0 if error, and 1 if success
*/
int fileGroup(char * message, group * from, group * dest_group, group * final_dest_group);

/**
* Send an attach notification to a group telling what's happening in sub-tree
* \param incoming_group the group which is arriving in the tree (passed through the tree)
* \param local_group the current process
* \param parents_list parents of current process
* \param routing_list subtree of group of current process
* \return 0 if error, and 1 if success
*/
int notifyAttach(group * new_group, group * local_group, List * parents_list, List * routing_list);

/**
* Send a detach notification to a group telling what's happening in sub-tree
* \param new_group the group which is leaving in the tree (passed through the tree)
* \param local_group the current process
* \param parents_list parents of current process
* \param routing_list subtree of group of current process
* \return 0 if error, and 1 if success
*/
int notifyDetach(group * new_group, group * local_group, List * parents_list, List * routing_list);

#endif
