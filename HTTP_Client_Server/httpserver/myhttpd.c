#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>     /* strlen(), strcmp() etc */
#include <stdio.h>      /* printf()  */
#include <string.h>     /* strlen(), strcmp() etc */
#include <errno.h>      /* extern int errno, EINTR, perror() */
#include <signal.h>     /* SIGCHLD, sigaction() */
//#include  <syslog.h>
//#include <fcntl.h>
//#include <time.h>
#include <sys/types.h>  /* pid_t, u_long, u_short */
#include <sys/socket.h> /* struct sockaddr, socket(), etc */
#include <sys/wait.h>   /* waitpid(), WNOHAND */
#include <netinet/in.h> 
//#include <dirent.h>
//#include <sys/sendfile.h>
#include "httpserver.h"

int main(int argc, char *argv[])
{
     
     MIME_set = 0;

     struct sigaction ACT;
     

     int sd, nsd, logging=0;
     char location_buf[60];

     pid_t pid;
     unsigned short port;   // server listening port
     socklen_t cli_addrlen;  
     
     struct sockaddr_in ser_addr, cli_addr; 
     char host[60];
     int fork_set = 5;
     /* get the port number */
     if (argc == 1) {
          port = SERV_TCP_PORT;
          setCurrentRoot();
     } else {


     //Server options*******************************************
     int option = 1,onum = 1, trigger = 0,port_set=0, root_set=0,prefork_set=0;

     while(argv[option]!=NULL){
     trigger = 0;
     
     //option -p <port number>
     if(strcmp(argv[option], "-p\0")==0){//1 if
          if(argv[option+1]==NULL){
          usage_error();
          }
     trigger = 1;

     int n = atoi(argv[option+1]);   
          if (n >= 1024 && n < 65536){ 
              port = n;
              printf("Port set to %d\n", port);
              port_set=1;
              }
          else {
              printf("Error: port number must be between 1024 and 65535\n");
              exit(1);
                }//end else
     }//end 1 if
     
     //option -d <document root>
     if(strcmp(argv[option], "-d\0")==0){//2 if
          if(argv[option+1]==NULL){
          usage_error();
          }else{
              trigger = 1;
              sprintf(document_root, "%s",argv[option+1]);
            //  document_root = argv[option+1];
            //  printf("The document root is set to %s\n", document_root);
              root_set = 1;
               }
      }//end 2 if


      //option -l <log file>
      if(strcmp(argv[option], "-l\0")==0){//3 if
          if(argv[option+1]==NULL){
          usage_error();
          }
         trigger = 1;

         logging = 1;
       
         sprintf(location_buf, "%smyhttpd.log",argv[option+1]);
        // strcpy(location,argv[option+1]);
      //   printf("The location now is at %s\n", location_buf);

       }//end 3 if

   
       //option -m <file for mime types>
       if(strcmp(argv[option], "-m\0")==0){//4 if
          if(argv[option+1]==NULL){
          usage_error();
          }
         trigger = 1;
         sprintf(MIME_file, "%s",argv[option+1]);
         if(open_MIME_type(MIME_file)==-1)
         printf("ERROR in opening file\n");
      //   printf("The MIME file's location is at %s\n", MIME_file);
       }//end 4 if


       //option -f <number of preforks>
       if(strcmp(argv[option], "-f\0")==0){//5 if
          if(argv[option+1]==NULL){
          usage_error();
          }
         trigger = 1;
         int pre = atoi(argv[option+1]);   
          if (pre >= 1 && pre < 15){ 
              fork_set = pre;
              //printf("Prefork number is set to %d\n", fork_set);
              prefork_set=1;
              }
          else {
              printf("Error: prefork number must be between 1 and 15\n");
              exit(1);
                }

       }//end 5 if
     onum++;//counting
     option = option+2;//move to next argv
     
     if(trigger==0){//0 means no option is triggered, is error
       printf("Invalid option\n");
       usage_error();
      // exit(0); 
     }

     }//end while
     

     if(port_set!=1)
     port = SERV_TCP_PORT;

     if(prefork_set!=1)
     fork_set = 5;

     if(root_set!=1){
     setCurrentRoot();
     }

     if(logging!=1)
     sprintf(location, "%smyhttpd.log",document_root);
     //strcat(document_root, location);
     else{
     sprintf(location, "%s%s",document_root, location_buf);
     }
     
     }//end the very first else

     //Server option******************************************

     /* turn the program into a daemon */
     daemon_init(); 

     ACT.sa_flags = 0;
     ACT.sa_handler = catcher;
     sigfillset(&(ACT.sa_mask));
     if(sigaction(SIGTERM, &ACT, NULL)!=0){
     printf("Error with signaction(int)\n");
     exit(1);
     }

     /* set up listening socket sd */
     if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
           perror("server:socket"); exit(1);
     } 

     /* build server Internet socket address */
     bzero((char *)&ser_addr, sizeof(ser_addr));
     ser_addr.sin_family = AF_INET;
     ser_addr.sin_port = htons(port);
     ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
     /* note: accept client request sent to any one of the
        network interface(s) on this host. 
     */

     /* bind server address to socket sd */
     if (bind(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0){
           perror("server bind"); exit(1);
     }
     gethostname(host, sizeof(host));
     printf("\nThe hostname is %s\n", host);


     int index_fork = 0;

     int test = logFile(location,"\r\n Server is started\r\n",logging);
        if(test==-1)
        {   
            printf("an error here\n");
            perror(location);
        }

     while(1){
     while(index_fork<fork_set)
     {
     if ((pid=fork()) <0) {
              perror("fork"); 
              exit(1);
          } else if (pid == 0) { //is child
              printf("A new child process is created, the pid is %d\n", getpid());
              break;

          }else if (pid > 0) { //is parent
              index_fork++;
           }

       }//end while

     if(pid==0){

     while (1) {
          /* become a listening socket */
          listen(sd, 5);
          /* wait to accept a client request for connection */
          cli_addrlen = sizeof(cli_addr);
          nsd = accept(sd, (struct sockaddr *) &cli_addr, &cli_addrlen);
          if (nsd < 0) {
               if (errno == EINTR)   /* if interrupted by SIGCHLD */
                    continue;
               perror("server:accept"); exit(1);
          }

          /* now in child, serve the current client */
          close(sd); 
          serve_a_client(nsd,location, logging);
          printf("Socket is closed\n");
          printf("Child process with pid %d is stopped\n", getpid());
          sleep(2);
          exit(0);
     }//end while
     }//end if pid == 0

     //wait for the child process terminal
     waitpid(pid, NULL, 0);
     
     printf("A child process is terminal\n");
     index_fork = fork_set-1;
     

   }//end while 1
}
