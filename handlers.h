#ifndef __HANDLERS_H__
#define __HANDLERS_H__

/**
* Handle a payload of type attach
* \param read_p the received payload
* \param new_child_group the new incoming group
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handeNewClientAttach(payload * read_p, group * new_child_group, group * localGroup, List * children_list, List * parents_list, List * routing_list);

/**
* Handle a payload of type detach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleDetach(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list);

/**
* Handle a payload of type msg
* \param read_p the received payload
* \param localGroup the current process in group form
* \param children_list children of current group
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleMsg(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list);

/**
* Handle a payload of type notif attach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleNotifAttach(payload * read_p, group * localGroup, List * parents_list, List * routing_list);

/**
* Handle a payload of type notif detach
* \param read_p the received payload
* \param localGroup the current process in group form
* \param parents_list parents of current group
* \param routing_list subtree of current group
*/
void handleNotifDetach(payload * read_p, group * localGroup, List * parents_list, List * routing_list);

void handleFile(payload * read_p, group * localGroup, List * children_list, List * parents_list, List * routing_list);

#endif
