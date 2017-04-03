/*
* Projekt do IPK - Klient a server
* Soubor: server.c
* Autor: František Matečný (xmatec00)
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
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

#include "server.h"

list l;
int port;
char* not_found;

/**
 * Ukonci zombie procesy
 */
void end_child(int proc)
{
    wait(&proc);
}

/**
 * Uvolnenie pamati
 */
void memory_free(){
	
	free(not_found);
	message* pom;
	l.Act = l.First;
	
	while(l.Act != NULL){
		pom = l.Act;
		l.Act = l.Act->next;
		free(pom);		
	}
}

/**
 * Ulozenie nenajdenych loginov/uid
 */
void msg_not_found(char find[]){

	if (not_found == NULL){
		not_found = malloc(sizeof(char) * (strlen(find) + 2));
	}
	else {
		not_found = realloc (not_found, sizeof(char) * (strlen(not_found) + strlen(find) + 2));
		}

	if(not_found == NULL){
		fprintf(stderr, "chyba alokacie pamati\n");
		memory_free();
		exit(-1);			
	}
	strcat(not_found, find);
	strcat(not_found, " ");
}



/**
 * funkcia ulozi odpoved klientovi do strukturi
 */
int answer_client(char* msg){
	
	//otvorenie suboru etc/passwd
	FILE *f;
	f = fopen("/etc/passwd", "r");
	if (f == NULL){
		fprintf(stderr, "chyba pri otvarani suboru\n");
		exit(-1);		
		}
	setbuf(f, NULL);
	
	int i;
	char c;
	char find[16];
	char pom[50];
	char pom_login[50];
	int msg_seek = 3;
	find[0] = '\0';
	int found1 = 0;
	int found2 = 0;

	//precitanie vsetkych looginov/uid zo spravy od klienta
	while (msg[msg_seek] != '-'){
		found2++;
		
		//nacitanie loginu/iud zo spravy od klienta
		for (i = 0; msg[msg_seek] != ' ' ; i++)
		{
			find[i] = tolower(msg[msg_seek]);
			msg_seek++;
		}
		find[i] = '\0';
		
		c = tolower(fgetc(f));
		while (c != EOF){
		
			//pom = login v etc/passwd
			if (msg[1] == 'l'){
				for(i = 0; c != ':'; i++){
					pom[i] = c;
					c = tolower(fgetc(f));
					}
			}
			//pom_login = login v etc/passwd
			//pom = uid v etc/passwd
			else if(msg[1] == 'u'){
					for(i = 0; c != ':'; i++){
					pom_login[i] = c;
					c = tolower(fgetc(f));
					}
					pom_login[i] = '\0';
					c = tolower(fgetc(f));
					for(i = 0; c != ':'; i++){
					c = tolower(fgetc(f));
					}
					c = tolower(fgetc(f));
					for(i = 0; c != ':'; i++){
					pom[i] = c;
					c = tolower(fgetc(f));
					}
				}
			else {fprintf(stderr, "chyba spravy od clienta\n"); return -1;}
			
			pom[i] = '\0';
			
			//zhoda loginu/uid
			if (strcmp(pom, find) == 0){
				
				found1++;
				
				//allokacia struktury
				if (l.First != NULL){
					l.Act->next = malloc(sizeof(message));
					l.Act = l.Act->next;}
				else{
					l.Act = malloc (sizeof(message));}
				if (l.First == NULL){
					l.First = l.Act;}
				
				//ulozenie loginu, uid z passwd do struktury
				if (msg[1] == 'l')
				{
					strcpy(l.Act->sLogin, pom);
					c = tolower(fgetc(f));
					
					for(i = 0; c != ':'; i++){
					c = tolower(fgetc(f));
					}
					c = tolower(fgetc(f));
					
					for(i = 0; c != ':'; i++){
					pom[i] = c;
					c = tolower(fgetc(f));
					}
					pom[i] = '\0';
					strcpy(l.Act->sUid, pom);
				}
				else
					{
					strcpy(l.Act->sLogin, pom_login);
					strcpy(l.Act->sUid, pom);
					}
					
				c = tolower(fgetc(f));
				
				//ulozenie gid
				for(i = 0; c != ':'; i++){
				pom[i] = c;
				c = tolower(fgetc(f));
				}
				pom[i] = '\0';
				strcpy(l.Act->sGid, pom);
				c = tolower(fgetc(f));
				
				//ulozenie gecos
				for(i = 0; c != ':'; i++){
				pom[i] = c;
				c = tolower(fgetc(f));
				}
				pom[i] = '\0';
				strcpy(l.Act->sGecos, pom);
				c = tolower(fgetc(f));
				
				//ulozenie home
				for(i = 0; c != ':'; i++){
				pom[i] = c;
				c = tolower(fgetc(f));
				}
				pom[i] = '\0';
				strcpy(l.Act->sHomeDir, pom);
				c = tolower(fgetc(f));
				
				//ulozenie shell
				for(i = 0; c != '\n'; i++){
				pom[i] = c;
				c = tolower(fgetc(f));
				}
				pom[i] = '\0';
				strcpy(l.Act->sShell, pom);
				
				//printf("%s,%s,%s,%s,%s,%s\n", l.Act->sLogin, l.Act->sUid, l.Act->sGid, l.Act->sGecos, l.Act->sHomeDir, l.Act->sShell);
				l.Act->next = NULL;
				}
			else{
				//preskocenie riadka
				while((c != '\n') && (c != EOF)){c = tolower(fgetc(f));}
				}
		if(c != EOF){			
		c = tolower(fgetc(f));}
		}
		
		//printf("%d,%d\n",found2,found1);
		//nenajdeny login/uid
		if (found1 < found2){
			msg_not_found(find);//ulozenie nenajdenych loginov/uid
			found1++;
			}

		rewind(f);
		msg_seek++;
	}
	
msg_seek++;

fclose(f);

return msg_seek;
}

/**
 * Nacitanie argumentov programu
 */
void load_arguments(int argc, char** argv){
	
	if(argc != 3){
		fprintf(stderr, "Zle argumenty\n");
		exit(-1);}
		
	int ch;
	bool flag = false;

	while ((ch = getopt(argc, argv, "p:")) != -1) {
		switch (ch) {
		case 'p':
					flag = true;
					port = atoi(optarg);
					if (port == 0) {
						fprintf(stderr, "Chybny port\n");
						exit(-1);
					}
					//printf("port: %d\n", port);
					break;
		default:
			break;
		}
	}
     
	if(!flag){
	 fprintf(stderr, "Zle argumenty\n");
	 exit(-1);}	
	
}



/**
 * Hlavny program 
 */
int main (int argc, char **argv) {

	//nacitanie argumentov programu
	load_arguments(argc, argv);

	int s, rc;
	char msg[1024];
	struct sockaddr_in sa;
	struct sockaddr_in sa_client;
	char str[INET_ADDRSTRLEN];
	socklen_t sa_client_len=sizeof(sa_client);

	//vytvorenie socketu
	if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) {
		printf("error on socket\n");
		return -1;
	}

	//nastavenie parametrov socketu
	memset(&sa,0,sizeof(sa));
	sa.sin_family = PF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr  = INADDR_ANY;
	
	//bind
	if ((rc = bind(s, (struct sockaddr*)&sa, sizeof(sa))) < 0)
	{
		perror("bind() failed");
		exit(EXIT_FAILURE);		
	}
	//listen
	if ((rc = listen(s, 1)) < 0)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);				
	}
	
	char msg_answer[1024];
	l.First = NULL;
	l.Act = NULL;
	not_found = NULL;
	
	//signal pre ukoncenie processu
	signal(SIGKILL, memory_free);
	//signal pre ukoncenie zombie process
	signal(SIGCHLD, end_child); 
	
	while(1)
	{
		//signal pre ukoncenie zombie process
		signal(SIGCHLD, end_child);
		//signal pre ukoncenie processu
		signal(SIGKILL, memory_free);

		int comm_socket = accept(s, (struct sockaddr*)&sa_client, &sa_client_len);		
		if (comm_socket > 0)
		{
			//forknutie procesu
			int boss = fork();
			if (boss < 0) {fprintf(stderr, "chyba fork\n");}

			//signal pre ukoncenie zombie process
			signal(SIGCHLD, end_child);
			//signal pre ukoncenie processu
			signal(SIGKILL, memory_free);
			
			//dieta - child
			if (boss == 0)
			{

				if(inet_ntop(AF_INET, &sa_client.sin_addr, str, sizeof(str))) {
					printf("New connection %s\n", str);
					//printf("Client address is %s\n", str);
					//printf("Client port is %d\n", ntohs(sa_client.sin_port));
				}
				
				//precitanie spravy od klienta
				if (read(comm_socket, msg, sizeof(msg) ) <0) {  
				  printf("error on read\n");
				  memory_free();
				  exit(-1);
				}
				
				//ulozenie odpovede klientovi do struktury				
				int msg_seek = answer_client(msg);
				l.Act = l.First;
				int old_msg_seek = msg_seek;

				//odpoved klientovi
				while((l.Act != NULL) || (not_found != NULL))
				{

					if (not_found == NULL)
					{
						msg_seek = old_msg_seek;
						msg_answer[0] = '\0';
						
						//vytvorenie jedneho riadku spravy
						while(msg[msg_seek] != '\0')
						{
							if (msg[msg_seek] == 'L'){strcat(msg_answer, l.Act->sLogin);strcat(msg_answer, " ");}
							if (msg[msg_seek] == 'U'){strcat(msg_answer, l.Act->sUid);strcat(msg_answer, " ");} 
							if (msg[msg_seek] == 'G'){strcat(msg_answer, l.Act->sGid);strcat(msg_answer, " ");} 
							if (msg[msg_seek] == 'N'){strcat(msg_answer, l.Act->sGecos);strcat(msg_answer, " ");} 
							if (msg[msg_seek] == 'H'){strcat(msg_answer, l.Act->sHomeDir);strcat(msg_answer, " ");}
							if (msg[msg_seek] == 'S'){strcat(msg_answer, l.Act->sShell);strcat(msg_answer, " ");}
							msg_seek++;
						}
						msg_answer[strlen(msg_answer)-1] = '\0';
						
						
						l.Act = l.Act->next;
					}
					else {
						strcat(msg_answer, "Nenajdeny: ");
						strcat(msg_answer, not_found);
						free(not_found);
						not_found = NULL;
						}
					//sleep(5);	
					//printf("odpoved: %s\n", msg_answer);
					
					//odpovedanie klientovi
					if ( write(comm_socket, msg_answer, strlen(msg_answer) + 1) < 0 ) {
						fprintf(stderr,"error on write\n");
						memory_free();
						exit(-1);
					}
					
					//echo od klienta (musi byt 'OK')
					if ( read(comm_socket, msg, sizeof(msg) ) <0) {
						memory_free();
						fprintf(stderr,"error on read\n");
						exit(-1);
					}

					//echo od klienta nie je 'OK'
					if(strcmp(msg,"OK") != 0){
						fprintf(stderr,"chyba komunikacie\n");
						memory_free();
						exit(-1);}
						
				}//while(l.Act != NULL)

				//koniec komunikacie
				strcpy(msg_answer, "EndOfMsg");
				if ( write(comm_socket, msg_answer, strlen(msg_answer) + 1) < 0 ) {
				  fprintf(stderr,"error on write\n");
				  memory_free();
				  exit(-1);
				}
				printf("Connection to %s closed\n",str);
				close(comm_socket);
				memory_free();
				exit(0);
			}//boss
			else {
				close(comm_socket);
				}
		}//if comm_socket
	}//while

return 0;
}
