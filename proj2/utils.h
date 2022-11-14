#define MAX_LENGHT 256

typedef struct URL {
	char user[MAX_LENGHT]; // user
	char password[MAX_LENGHT]; // password
	char host[MAX_LENGHT]; // host
	char path[MAX_LENGHT]; // file path
	char filename[MAX_LENGHT]; // filename
} url;


int parseURL(url* url, const char* argument);

