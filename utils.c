#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
#include <time.h>

#include "gconst.h"
#include "utils.h"
#include "list.h"
#include "payload.h"

/**
* Parse a command
* \param cmd the command to parse
* \param args where will be stored arguments
* \return number of args in cmd
*/
int parseArgs(char * cmd, char * args[]){
	deleteLineBreaks(cmd);

	int nb_arg = 0;
	int i = 0;
	int have_quote = 0;
	char * arg = calloc(BUFFSIZE, sizeof(char));
	memset(arg, '\0', BUFFSIZE);
	int ai = 0;

	for(i = 0; i < strlen(cmd); i++){
		if(cmd[i] == '"'){
			if(have_quote == 0){//premier guillemet
				have_quote = 1;
			}else{//deuxieme guillemet
				have_quote = 0;
			}
		}else if(cmd[i] == ' ' || cmd[i] == '\0'){
			if(have_quote == 0){
				args[nb_arg] = arg;
				arg = calloc(BUFFSIZE, sizeof(char));
				memset(arg, '\0', BUFFSIZE);
				nb_arg++;
				ai = 0;
			}

			if(have_quote == 1){
				arg[ai] = cmd[i];
				ai++;
			}
		}else{
			arg[ai] = cmd[i];
			ai++;
		}
	}

	if(strcmp(arg, " ") != 0 && strcmp(arg, "") != 0){
		args[nb_arg] = arg;
		nb_arg++;
	}

	for(i = 0; i < nb_arg; i++){
		//printf("arg => |%s|\n",args[i]);
		deleteLineBreaks(args[i]);
	}

	return nb_arg;
}

/**
* Get IP from an interface name
* \param iface the interface name
* \param addr wher will be stored the ip
*/
void getIP(char* iface, struct in_addr *addr){
	struct ifaddrs *ifaddr, *ifa;
	int s, n;
	char host[BUFFSIZE];
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
	}
	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family == AF_INET6)
		continue;
		else if (strcmp(ifa->ifa_name,iface) ==0){
			if((s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host,BUFFSIZE,NULL,0,NI_NUMERICHOST)) == 0){
				inet_aton(host,addr);
			}
		}
	}
	freeifaddrs(ifaddr);
}

/**
* Check if the current process is root of a group
* \param parents_list parents of current group
* \return 1 if current group is a root else 0
*/
int isRoot(List * parents_list){
	if(size(parents_list) == 0){
		return 1;
	}
	return 0;
}

/**
* Add a payload to an history file
* \param p the payload to write
*/
void historilog(payload * p){
	time_t current_time;
	char* c_time_string;
	current_time = time(NULL);
	c_time_string = ctime(&current_time);
	c_time_string[strlen(c_time_string)-1] = '\0';
	FILE *fd;
	fd = fopen("log","a");
	fprintf( fd , "[%s] ",c_time_string);

	if(p->ptype == PTYPE_ATTACH){
		fprintf( fd , "ptype : ATTACH ");
	}else if(p->ptype == PTYPE_BRDCST){
		fprintf( fd , "ptype : BROADCAST ");
	}else if(p->ptype == PTYPE_DETACHFP){
		fprintf( fd , "ptype : DETACH_FP ");
	}else if(p->ptype == PTYPE_DETACHFC){
		fprintf( fd , "ptype : DETACH_FC ");
	}else if(p->ptype == PTYPE_MSGROOT){
		fprintf( fd , "ptype : MSGROOT ");
	}else if(p->ptype == PTYPE_MSG){
		fprintf( fd , "ptype : MSG ");
	}else if(p->ptype == PTYPE_NOTIFATT){
		fprintf( fd , "ptype : NOTIFATT ");
	}else if(p->ptype == PTYPE_NOTIFDET){
		fprintf( fd , "ptype : NOTIFDET ");
	}else if(p->ptype == PTYPE_FILEROOT){
		fprintf( fd , "ptype : FILEROOT ");
	}else if(p->ptype == PTYPE_FILE){
		fprintf( fd , "ptype : FILE ");
	}

	fprintf( fd , "sender port : %d	" , p->sender_port);
	fprintf( fd , "sender ip : %s " , p->sender_ip);
	fprintf( fd , "sender name : %s " , p->sender_name);
	fprintf( fd , "recver port : %d " , p->recver_port);
	fprintf( fd , "recver ip : %s " , p->recver_ip);
	fprintf( fd , "recver name : %s " , p->recver_name);
	fprintf( fd , "content : %s\n" , p->content);
	fclose(fd);
}

/**
* Retrieve a group from it's name in list l
* \param group_name the name of the oayload to find
* \param l the list to retrieve group_name
* \return the group if found else NULL
*/
group * getGroupFromName(char * group_name, List * l){
	List * current_g_cell = l->next;
	while (current_g_cell != NULL) {
		group * g = current_g_cell->g;
		if(strcmp(group_name, g->name) == 0){
			return g;
		}

		current_g_cell = current_g_cell->next;
	}
	return NULL;
}

/**
* Retrieve a group from it's ip/port
* \param ip the ip of the oayload to find
* \param port the port of the oayload to find
* \param l the list to retrieve group_name
* \return the group if found else NULL
*/
group * getGroupFromIPPort(char * ip, int port, List * l){
	List * current_g_cell = l->next;
	while (current_g_cell != NULL) {
		group * g = current_g_cell->g;

		if(strcmp(ip, g->addip) == 0 && port == g->port){
			return g;
		}

		current_g_cell = current_g_cell->next;
	}
	return NULL;
}

/**
* Delete all useless \n in a string
* \param str the string where \n will be deleted
*/
void deleteLineBreaks(char * str){
	char ch = '\n';
	int i, len, j;
	i = 0;

	len = strlen(str);

	while(i < len){
		if(str[i] == ch){
			j = i;
			while(j < len){
				str[j] = str[j + 1];
				j++;
			}
			len--;
			i--;
		}
		i++;
	}
}

void loadScreen(char** screen, FILE* fichier){
	int i;
	for(i=0;i<LIMITX;i++){
		if(fgets(screen[i],LIMITY,fichier)==NULL){
			break;
		}
	}
}
