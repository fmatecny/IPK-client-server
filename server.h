/*
* Projekt do IPK - Klient a server
* Soubor: server.h
* Autor: František Matečný (xmatec00)
*/
typedef struct Msg{

	char sLogin[16];
	char sUid[16];
	char sGid[16];
	char sGecos[32];
	char sHomeDir[32];
	char sShell[32];
	
	struct Msg* next;
	
}message ;

typedef struct Tlist{
	
	message* First;
	message* Act;	
	
} list;

void end_child(int proc);
void memory_free();
int answer_client(char* msg);
void load_arguments(int argc, char **argv);
void msg_not_found(char find[]);
