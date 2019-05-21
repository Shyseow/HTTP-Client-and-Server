#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>     /* strlen(), strcmp() etc */
#include <stdio.h>      /* printf()  */
#include <string.h>     /* strlen(), strcmp() etc */
#include <errno.h>      /* extern int errno, EINTR, perror() */
#include <signal.h>     /* SIGCHLD, sigaction() */
#include <syslog.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>  /* pid_t, u_long, u_short */
#include <sys/socket.h> /* struct sockaddr, socket(), etc */
#include <sys/wait.h>   /* waitpid(), WNOHAND */
#include <netinet/in.h> 
#include <dirent.h>
#include <sys/sendfile.h>
#include "httpserver.h"

int logFile(char *location, char *buf, int trigger)
{
    FILE *fp;
    if(trigger == 0)
    {
        sprintf(location, "%smyhttpd.log",document_root);
        //printf("The log file location is %s\n",location);
    }

    time_t timer;
    char buffer[4096];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 4096, "%c", tm_info);
    strcat(buffer,buf);

    if(fopen(location,"r")==NULL)
    {
        fp=fopen(location,"w+");
        //printf("Logging location is at %s\n",location);
        if(fp==NULL)
        {
            printf("file location is null\n");
            return -1;
        }
    }
    else
    {
        fp=fopen(location,"a+");
    }
    fputs(buffer, fp);
    fclose(fp);
    return 0;
}

int send404(int sd)
{
FILE *fp;
char html404[1024];
int fd, num;
char buf[1024];

    time_t timenow;
    struct tm * timeinfo;
    time (&timenow);
    timeinfo = localtime(&timenow);
    char host[60];
    char http_not_found[] = "HTTP/1.1 404 Not Found\n";
    char header[4096];
    char *filetype = "text/html";

char *content = "<!DOCTYPE html>\n<html>\n<body>\n<br><center><h1>404</h1></center>\n<center><h2>PAGE NOT FOUND</h2></center>\n</body>\n</html>";

gethostname(host, sizeof(host));
sprintf(html404, "%s404.html",document_root);

if(fopen(html404,"r")==NULL)
    {
        fp=fopen(html404,"w+");
        if(fp==NULL)
        {
            printf("404 file location is null\n");
            return -1;
        }
        fputs(content, fp);
    }
    else
    {
        fp=fopen(html404,"a+");

    }
    fclose(fp);

    if((fd = open(html404, O_RDONLY))!=-1){
    int size = get_file_size(html404);
    sprintf(header, "%sContent-type: %s\nContent-Length: %d\nDate: %sServer: %s\n\n",http_not_found, filetype, size, asctime(timeinfo), host); 

    send(sd, header, strlen(header), 0);

    while((num = read(fd, buf, size))>0){
            send(sd, buf, size,0);
            }//end while
    }
    else{
    printf("404 File missing\n");
    return -1;
    }
    return 0;
}

int open_MIME_type(char *URL)
{
int num, fd;
if((fd = open(URL, O_RDONLY))!=-1){

    while((num = read(fd, File_type, 10000))>0){
            }//end while
            MIME_set = 1;
    }
    else{
    printf("404 File missing\n");
    return -1;
    }

return 0;

}

void claim_children()
{
     pid_t pid=1;

     while (pid>0) { /* claim as many zombies as we can */
         pid = waitpid(0, (int *)0, WNOHANG); 
     } 
}


void daemon_init(void)
{       
     pid_t   pid;
     struct sigaction act;
     
     if ( (pid = fork()) < 0) {
          perror("fork"); exit(1); 
     } else if (pid > 0) {
          printf("The server is started. PID: %d\n", pid);
          exit(0);                  /* parent goes bye-bye */
     }

     /* child continues */
     setsid();                      /* become session leader */
     chdir("/");                    /* change working directory */
     umask(0);                      /* clear file mode creation mask */

     /* catch SIGCHLD to remove zombies from system */
     act.sa_handler = claim_children; /* use reliable signal */
     sigemptyset(&act.sa_mask);       /* not to block other signals */
     act.sa_flags   = SA_NOCLDSTOP;   /* not catch stopped children */
     sigaction(SIGCHLD,(struct sigaction *)&act,(struct sigaction *)0);
     
}

void catcher(int signo){ 
if(signo == 15){

int test = logFile(location,"\nServer/Process is stopped now\n",1);
        if(test==-1)
        {   
            printf("an error here\n");
            perror(location);
        }
exit(0);
}
}

//get the file type
//get the file type
char * get_file_type(char *URL){
char ch = '.';
char * temp;
temp = strchr(URL, ch);
char * read;
if(MIME_set!=1){

if(strcmp(temp, ".html")==0){
return "text/html";
}

if(strcmp(temp, ".gif")==0){
return "image/gif";
}

if(strcmp(temp, ".txt")==0){
return "text/plain";
}

if(strcmp(temp, ".jpeg")==0){
return "image/jpeg";
}

if(strcmp(temp, ".jpg")==0){
return "image/jpg";
}
return "Invalid";

}

else {//file type exists
read = strtok(File_type, "\n");
while(read!=NULL){

if(strcmp(temp, read)==0){

if(strcmp(read, ".html")==0){
return "text/html";
}

if(strcmp(read, ".gif")==0){
return "image/gif";
}

if(strcmp(read, ".txt")==0){
return "text/plain";
}

if(strcmp(read, ".jpeg")==0){
return "image/jpeg";
}

if(strcmp(temp, ".jpg")==0){
return "image/jpg";
}
return read;
}
read = strtok(NULL, "\n");

}//end while
return "Invalid";
}//end else

}//end function


//Get the file size, useful to assign value to buf 
int get_file_size(char *URL){
    FILE *p_file;
    int size;
    p_file = fopen(URL,"rb");
     if(p_file==NULL){
        printf("Error opening file\n");
        return -1;
        }
          
       fseek(p_file,0,SEEK_END);
       size = ftell(p_file);
       fclose(p_file);

       return size;
}

void sendWebpage(int sd, int size, char * filetype, char *url, int fd){

int fd2;

if((fd2 = open(url, O_RDONLY))!=-1){

sendfile(sd, fd2, NULL, size);

close(fd);

}//end if find file
}
void get_head_method(int sd, char *URL, char *command){
    time_t timenow;
    struct tm * timeinfo;
    time (&timenow);
    timeinfo = localtime(&timenow);
    char host[60];
    char * http_ok = "HTTP/1.1 200 OK\n";
    char *filetype;
    char * header;
    int size;

    gethostname(host, sizeof(host));
    
    header = (char*)malloc(BUF_SIZE*sizeof(char));

         int fd, num;
         if(URL==NULL){
         printf("Null URL");
         return;
         }

         
            if((fd = open(URL, O_RDONLY))==-1){//4
            filetype = "NULL";
            if(send404(sd)==-1){
            printf("Error: 404 NOT FOUND\n");
            }
            getStatus = "HTTP/1.1 404 Not Found \n";
            printf("Respond sent\n");
            return;
            }//4

            //assuming the file type
           filetype = get_file_type(URL);
           if(strcmp(filetype, "Invalid")!=0){
           }
           else{
           if(send404(sd)==-1){
           printf("Error: Invalid file type\n");
           }
           getStatus = "HTTP/1.1 404 Not Found (invalid file type) \n";
           printf("Respond sent\n");
           return;
           }

           //allocate memory for the buf based on the file size
           size = get_file_size(URL);
           char buf[size];

           //setup the 200 header
           sprintf(header, "%sContent-type: %s\nContent-Length: %d\nDate: %sServer: %s\n\n",http_ok, filetype, size, 		asctime(timeinfo), host); 

            //Send the header to the client, same apply on HEAD or GET
            send(sd, header, strlen(header), 0);
       
            //process the GET method
            if(strcmp(command, "GET\0")==0){

            if(strcmp(filetype, "text/html")==0){//if html

            //sending the file
            while((num = read(fd, buf, size))>0){
            send(sd, buf, size-1,0);
            
            }
            }//html
           else{//Try process other file-types
           sendWebpage(sd, size, filetype, URL, fd);
           }
            getStatus = "HTTP/1.1 200 OK              \n";
            printf("Respond sent\n");
            return;
            }
       
}
void trace_method(int sd, char* URL){
    time_t timenow;
    struct tm * timeinfo;
    time (&timenow);
    timeinfo = localtime(&timenow);
    char host[60];
    char * http_ok = "HTTP/1.1 200 OK\n";
    char header[2048];
    gethostname(host, sizeof(host));

sprintf(header, "%sContent-type: %s\nDate: %sServer: %s\nConnection: close\n\n",http_ok, "message/http", asctime(timeinfo), host); 

send(sd, header, strlen(header), 0);
getStatus = "HTTP/1.1 200 OK              \n";
printf("Respond sent\n");
}

//handle the directory
void handle_directory(int sd, char *URL, char *command){
DIR *dir_ptr;
struct dirent * direntp;
int fd;
char newURL[BUFSIZE];
char newURL2[BUFSIZE];
char newURL3[BUFSIZE];
//printf("url is %s\n", URL);
if(URL != NULL){

//Try to find the index.html, index.htm or default.html
sprintf(newURL, "%sindex.html",URL);
sprintf(newURL2, "%sindex.htm",URL);
sprintf(newURL3, "%sdefault.html",URL);

//Index.html is found, pass it to get or trace method
if((fd = open(newURL, O_RDONLY))!=-1){

//get, trace and head method
         if(strcmp(command, "TRACE\0")==0){
             trace_method(sd, newURL);
            }
         //is GET or HEAD
         if((strcmp(command, "GET\0")==0)||(strcmp(command, "HEAD\0")==0)){
         get_head_method(sd, newURL, command);
           }
}
else if((fd = open(newURL2, O_RDONLY))!=-1){

//get, trace and head method
         if(strcmp(command, "TRACE\0")==0){
             trace_method(sd, newURL2);
            }
         //is GET or HEAD
         if((strcmp(command, "GET\0")==0)||(strcmp(command, "HEAD\0")==0)){
         get_head_method(sd, newURL2, command);
           }
}
else if((fd = open(newURL3, O_RDONLY))!=-1){

//get, trace and head method
         if(strcmp(command, "TRACE\0")==0){
             trace_method(sd, newURL3);
            }
         //is GET or HEAD
         if((strcmp(command, "GET\0")==0)||(strcmp(command, "HEAD\0")==0)){
         get_head_method(sd, newURL3, command);
           }
}

//no index.html is found
else{
if((fd = open(URL, O_RDONLY))!=-1){

dir_ptr = opendir(URL);
send(sd, "\nDirectory list\n-------------\n", strlen("\nDirectory list\n-------------\n"), 0);
while((direntp = readdir(dir_ptr))!= NULL){

//send the directory list to client
send(sd, direntp->d_name, strlen(direntp->d_name), 0);
send(sd, "\n", strlen("\n"), 0);
}//end while
getStatus = "HTTP/1.1 200 OK (Valid directory)  \n";
printf("Respond sent\n");
}


else{
printf("Invalid directory\n");
if(send404(sd)==-1){
printf("Error: Invalid directory\n");
}
getStatus = "HTTP/1.1 404 Not Found (invalid directory) \n";
printf("Respond sent\n");
return;
}

}//end else

}
closedir(dir_ptr);

}

//print the error message
void usage_error(){
printf("Usage error: myhttpd [-p <portnumber>]\n");
printf("                     [-d <document root>]\n");
printf("                     [-l <log file>]\n");
printf("                     [-m <file for mime types>]\n");
printf("                     [-f <number of preforks>]\n");
exit(0);
}

//check the url and select an appropriate method
void serve_a_client(int sd, char *location, int trigger)
{   
    char buf[BUF_SIZE];
    char *OrinUrl;
    char url[1024];

    char log[14096];
    char from[BUF_SIZE];

    int num;//check the received message

    char *command;
    char *HTTP;
    char new_url[1024];

    while (1){
         num = recv(sd, buf, sizeof(buf)-1, 0);
         //get the request

         if(num<=0){//avoid the empty message

         return;
         }

  
         strcpy(from,buf);

         printf("\nThe received request is : \n%s\n\n", buf);

         command = strtok(buf, " \t\n");//getting the command

         OrinUrl = strtok(NULL, " \t\n");//getting the url

         sprintf(url, "%s%s",document_root, OrinUrl);
         
         HTTP = strtok(NULL, " \t\n");//getting the http:
         
        //get client hostname
        char *client = strstr(from, "From");
        int index = client-from;
        client = from + index;


         char * findChar;//check the last char


         //check if it is a valid HTTP request
         if(strcmp(HTTP, "HTTP/1.1")==0){
            printf("Not a HTTP request\n");
            send(sd, "Not a HTTP request\n", sizeof("Not a HTTP request\n"),0);
            return;
            }


         int s = strlen(url);//get the length of url

         if(strcmp(url+s-1, "/")==0){//if the last char is /, then process

         //is a directory
         handle_directory(sd, url, command);
         }

         //if '.' is not found in the url, assume it is a directory
         else if((findChar = strchr(url, '.'))==NULL){

         //adds "/" to the end of the url
         sprintf(new_url, "%s/",url);
    
         //process the new url
         handle_directory(sd, new_url, command);
         }

         else{//else if not a directory


         //command is TRACE
         if(strcmp(command, "TRACE\0")==0){
             trace_method(sd, url);
            }

         //command is GET or HEAD
         if((strcmp(command, "GET\0")==0)||(strcmp(command, "HEAD\0")==0)){
         get_head_method(sd, url, command);
         }
         }//end else
         

         // Logging of file
        sprintf(log," %s / %s %s %s %s\n", command, url, HTTP, client, getStatus);
       
        int test = logFile(location,log,trigger);
        if(test==-1)
        {   
            printf("an error here\n");
            perror(location);
        }

         close(sd);
   }//while
}

void setCurrentRoot(){
char cwd[1024];
     if (getcwd(cwd, sizeof(cwd)) != NULL){
       sprintf(document_root, "%s/",cwd);
       printf("\nCurrent working directory is at : %s\n", document_root);
     }
     else{
       printf("getcwd() error\n");
       
     }
}
