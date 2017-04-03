/*
* Projekt do IPK - Klient a server
* Soubor: client.h
* Autor: 	František Matečný (xmatec00)
*/
typedef struct String{
	
	char* str;
	struct String* next;
	
} string;

typedef struct Tlist{
	
	string* First;
	string* Act;
	
} list;

typedef struct Message{
	
	list login_uid;
	
	char search_login_uid;
	
	int sLogin;
	int sUid;
	int sGid;
	int sGecos;
	int sHomeDir;
	int sShell;	
	
} message;

void memory_free();
void copy_string(char** s1, char* s2);
void create_message();
void load_arguments(int argc, char **argv);

