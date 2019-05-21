#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <netinet/in.h>       /* struct sockaddr_in, htons, htonl */
#include <netdb.h>            /* struct hostent, gethostbyname() */
#include <sys/stat.h>
#include "client.h"


int main(int argc, char *argv[])
{
    struct sockaddr_in ser_addr;
    struct hostent *hp;
    char host[60];
    int sd,c,port,flag=0; /*flag to check if -a is activated */
    char *value, *URL;

    //char host1[60];
    //gethostname(host, sizeof(host1));
     //printf("\nThe hostname is %s\n", host1);

    /* use given hostname and default port number */
    if (argc==2)
    {
        URL = checkPort(argv[1],&port,host);
        value = "GET";
        //write to check if there is a given port if not leave it as default for now
    }
    else if (argc > 2)    /* see which method the user use */
    {
        //port = SERV_TCP_PORT;
        //check if there is there client option used
        while ((c = getopt (argc, argv, "m:a")) != -1)
            //switch case to get which option is used
            switch (c)
            {
            case 'm':
        //if -m is used but no input was given
		if(optarg == NULL)
		{
		printf("Please input HEAD or TRACE");
		exit(1);
		}
                //get the input after -m
                value = check_header(optarg);
                //return the URL link and set the port number
                URL = checkPort(argv[optind],&port,host);
                printf("-m was triggered, value is now: %s\n", value);
                break;
            case 'a':
                flag = 1;
                if(value == NULL)
                {
                    value = "GET";
                    //return the URL link and set the port number
                    URL = checkPort(argv[optind],&port,host);
                    printf("-a was triggered, Host is now: %s\n", host);
                    printf("url is now %s\n", URL);
                }
                else
                {
                    //return the URL link and set the port number
                    URL = checkPort(argv[4],&port,host);
                    printf("-a was triggered, Host is now: %s\n", host);
                }
                break;
            case '?':
                print_usage();
                exit(1);
            default:
                print_usage();
                exit(1);
            }
    }
    else
    {
        print_usage();
    }

    /* get host address, & build a server socket address */
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(port);
    if ((hp = gethostbyname(host)) == NULL)
    {
        printf("host %s not found\n", host);
        exit(1);
    }
    ser_addr.sin_addr.s_addr = * (u_long *) hp->h_addr;

    /* create TCP socket & connect socket to server address */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0)
    {
        perror("client connect");
        exit(1);
    }

    if(argc==2){
        handleReq(sd, host, flag, value, URL);
    }
    else if(argc>2){
	if(value == NULL)
    	return(0);
	else
        handleReq(sd, host, flag, value, URL);
    }

    return(0);
}

