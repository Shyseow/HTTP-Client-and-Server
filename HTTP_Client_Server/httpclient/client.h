#define   	SERV_TCP_PORT  40105 /* default server listening port */
#define 	BUFSIZE 4096

//to check if port beside default code was used and return the URL of the host
char *checkPort(char *input, int *p, char host[]);

//Handle the get, head and trace request
void handleReq(int sd, char* URL, int fl, char* input, char *filePath);

//Check if header is HEAD or TRACE
char *check_header(char* input);

//Print out the usage message for client
void print_usage();

