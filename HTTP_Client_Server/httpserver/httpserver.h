#define   SERV_TCP_PORT   40105   /* default server listening port */
#define   BUF_SIZE 1024
#define   BUFSIZE 4096

//Global Variable
char *getStatus;
char document_root[BUF_SIZE];//document root
char MIME_file[BUF_SIZE];//location of MIME file
char File_type[10000];//file type
char MIME_set;
char location[100]; //logfile default location
int logFile(char *location, char *buf, int trigger);

int send404(int sd);

int open_MIME_type(char *URL);

void claim_children();

void daemon_init(void);

void catcher(int signo);

char *get_file_type(char *URL);

//Get the file size, useful to assign value to buf 
int get_file_size(char *URL);

void sendWebpage(int sd, int size, char * filetype, char *url, int fd);

void trace_method(int sd, char* URL);

void handle_directory(int sd, char *URL, char *command);

//print the error message
void usage_error();

//check the url and select an appropriate method
void serve_a_client(int sd, char *location, int trigger);

void setCurrentRoot();

void get_head_method(int sd, char *URL, char *command);


