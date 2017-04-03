/*
* Projekt do IPK - Klient a server
* Soubor: client.c
* Autor: 	František Matečný (xmatec00)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "client.h"

#define h_addr h_addr_list[0]

message m;
char* host_name;
char msg[1024];
int port;

/**
 * uvolnenie pamati
 */
void memory_free(){
	
	free(host_name);
		
		
	string* pom = m.login_uid.First;
	m.login_uid.Act = m.login_uid.First;
	while (m.login_uid.Act != NULL)
	{
		free(m.login_uid.Act->str);
		pom = m.login_uid.Act;
		m.login_uid.Act = m.login_uid.Act->next;
		free(pom);
	}		

}

/**
 * Kopírovanie retazca s alokaciou pamati
 */
void copy_string(char** s1, char* s2){

	*s1 = malloc(sizeof(char) * (strlen(s2)+1));
	if (*s1 == NULL){
		fprintf(stderr, "chyba alokacie pmati\n");
		exit(-1);
		}
	strcpy(*s1, s2);
}

/**
 * Vytvorenie spravy
 */
void create_message(){
	
	m.login_uid.Act = m.login_uid.First;

	if(m.search_login_uid == 'l'){
		strcpy(msg, "-l ");}
	else if(m.search_login_uid == 'u'){
		strcpy(msg, "-u ");
		}
	strcat(msg, m.login_uid.Act->str);
	strcat(msg, " ");

	for (m.login_uid.Act = m.login_uid.Act->next; m.login_uid.Act != NULL ; m.login_uid.Act = m.login_uid.Act->next)
	{
		strcat(msg, m.login_uid.Act->str);
		strcat(msg, " ");
	}	

	strcat(msg, "-");

	for (int i = 1; i < 7; i++)
	{
		if(m.sLogin == i){strcat(msg, "L");}					
		else if(m.sUid == i){strcat(msg, "U");}
		else if(m.sGid == i){strcat(msg, "G");}
		else if(m.sGecos == i){strcat(msg, "N");}
		else if(m.sHomeDir == i){strcat(msg, "H");}
		else if(m.sShell == i){strcat(msg, "S");}			
	}			
	//printf("sprava: %s\n", msg);

}

/**
 * Nacitanie a kontrola argumentov
 */
void load_arguments(int argc, char **argv){
	
	//nastavenie parametrov struktury message
	m.login_uid.First = NULL;
	m.login_uid.Act = NULL;
	m.sLogin = 0;
	m.sUid = 0;
	m.sGid = 0;
	m.sGecos = 0;
	m.sHomeDir = 0;
	m.sShell = 0;
	m.search_login_uid = '0';
	
	int ch;
	int arg_sum = 1;	
	int search = 0;
		
	if(argc < 7){
		fprintf(stderr, "Zle argumenty\n");
		exit(-1);}
	
	
	while ((ch = getopt(argc, argv, "h:p:l:u:LUGNHS")) != -1)
	{

		switch (ch) {
			
		case 'h':	arg_sum+=2;
					copy_string(&host_name, optarg);
					//printf("host: %s\n", host_name);
					break;

		case 'p': 	arg_sum+=2;
					port = atoi(optarg);
					if (port == 0){
						fprintf(stderr, "Chybny port %d\n", port);
						exit(-1);
						}
					//printf("port: %d\n", port);
					break;
				 
		case 'l': 	if (m.search_login_uid != '0'){
							string* pom = m.login_uid.First;
							m.login_uid.Act = m.login_uid.First;
							while (m.login_uid.Act != NULL)
							{
								free(m.login_uid.Act->str);
								pom = m.login_uid.Act;
								m.login_uid.Act = m.login_uid.Act->next;
								free(pom);
							}		

					}
					m.search_login_uid = 'l';
					arg_sum++;
					optind--;
					for(int i = 0;optind < argc && *argv[optind] != '-'; optind++, i++){
							if (i == 0)
							{
								m.login_uid.Act = malloc (sizeof(string));
								m.login_uid.First = m.login_uid.Act;
							}
							else{
								m.login_uid.Act->next = malloc (sizeof(string));
								m.login_uid.Act = m.login_uid.Act->next;
								}
								
							if (m.login_uid.Act == NULL){
								fprintf(stderr,"chyba alokacie pamati\n");
								exit(-1);
								}
							copy_string(&m.login_uid.Act->str, argv[optind]);
                            //printf("login: %s\n", m.login_uid.Act->str);
                            arg_sum++; 
                            m.login_uid.Act->next = NULL;                         
                        }
					break;
				 
		case 'u': 	if (m.search_login_uid != '0'){
							string* pom = m.login_uid.First;
							m.login_uid.Act = m.login_uid.First;
							while (m.login_uid.Act != NULL)
							{
								free(m.login_uid.Act->str);
								pom = m.login_uid.Act;
								m.login_uid.Act = m.login_uid.Act->next;
								free(pom);
							}
					}
					m.search_login_uid = 'u';
					arg_sum++;
					optind--;
					for(int i = 0;optind < argc && *argv[optind] != '-'; optind++, i++){
							if (i == 0){
							m.login_uid.Act = malloc (sizeof(string));
							m.login_uid.First = m.login_uid.Act;}
							else{
								m.login_uid.Act->next = malloc (sizeof(string));
								m.login_uid.Act = m.login_uid.Act->next;
								}
								
							if (m.login_uid.Act == NULL){
								fprintf(stderr,"chyba alokacie pamati\n");
								exit(-1);
								}
							if ((atoi(argv[optind]) == 0) && (argv[optind][0] != '0')){
								fprintf(stderr, "chybne uid\n");
								exit(-1);
								}
							copy_string(&m.login_uid.Act->str, argv[optind]);
                            //printf("uid: %s\n", m.login_uid.Act->str);
                            arg_sum++; 
                            m.login_uid.Act->next = NULL;                         
                        }
					break;
				 
		case 'L': 	arg_sum++;
					search++;
					m.sLogin = search;
					break;
				 
		case 'U': 	arg_sum++;
					search++;
					m.sUid = search;
					break;
				 
		case 'G': 	arg_sum++;
					search++;
					m.sGid = search;
					break;
				 
		case 'N': 	arg_sum++;
					search++;
					m.sGecos = search;
					break;
				 
		case 'H': 	arg_sum++;
					search++;
					m.sHomeDir = search;
					break;
				 
		case 'S': 	arg_sum++;
					search++;
					m.sShell = search;
					break;
 
		default:	fprintf(stderr, "Zle argumenty2\n");
					exit(-1);
					break;
		}
	}
	
	if((arg_sum < argc) || (m.search_login_uid == '0')){
		fprintf(stderr, "Zle argumenty\n");
		exit(-1);}
	
}

/**
 * Hlavny program
 */
int main (int argc, char **argv) {

	load_arguments(argc, argv);

	int s,n;
	struct sockaddr_in sa;
	struct hostent *hptr;


	//vytvorenie spravy
	create_message();

	if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) { /* create socket*/
		memory_free();
		printf("error on socket\n");  /* socket error */
		return -1;
	}
	memset(&sa,0,sizeof(sa));
	sa.sin_family = PF_INET;              /*set protocol family to Internet */
	sa.sin_port = htons(port); 		 /* set port no. */
	sa.sin_addr.s_addr  = INADDR_ANY;   /* set IP addr to any interface */

	if ((hptr =  gethostbyname(host_name)) == NULL){
		fprintf(stderr, "gethostname error: %s", host_name);
		memory_free();
		return -1;
	}
	
	memcpy( &sa.sin_addr, hptr->h_addr, hptr->h_length);
	
	if (connect (s, (struct sockaddr *)&sa, sizeof(sa) ) < 0 ){
		perror("error on connect");
		memory_free();
		return -1;   /* connect error */
	}
	
	//poslanie poziadavky serveru
	if ( write(s, msg, strlen(msg) +1) < 0 ) {  /* send message to server */
		memory_free();
		perror("error on write");	return -1; /*  write error */
	}

	while(1){
		//odpoved od servera
		if ( (n = read(s, msg, sizeof(msg) ) ) <0) {  /* read message from server */
			perror("error on read"); return -1; /*  read error */
		}		
		msg[n] = '\0';
		if(strcmp(msg, "EndOfMsg") == 0) break;
		
		if(strncmp(msg, "Nenajdeny:", 10) == 0){
			fprintf(stderr, "%s\n", msg);
			}
		else{
		printf ("%s\n", msg);}
		
		strcpy(msg, "OK");
		if ( write(s, msg, strlen(msg) +1) < 0 ) {  /* send message to server */
			perror("error on write");	return -1; /*  write error */
		}
	}

	memory_free();

return 0;
}
