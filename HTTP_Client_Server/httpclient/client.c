#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>       /* struct sockaddr_in, htons, htonl */
#include <netdb.h>            /* struct hostent, gethostbyname() */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

void print_usage()
{
    printf("Usage: myhttp [-m <method>] [-a] <URL>\n");
}

char *check_header(char* input)
{
    char * output;
    //check if HEAD was used
    if(strcmp(input, "HEAD") ==0)
    {
        output ="HEAD";
        printf("The output value is %s\n",output);
        return output;
    }
    //check if TRACE was used
    else if(strcmp(input, "TRACE") ==0)
    {
        output ="TRACE";
        printf("The output value is %s\n",output);
        return output;
    }
    else
    {
        printf("The method must be HEAD or TRACE");
    }
    return NULL;
}

void handleReq(int sd, char* URL, int fl, char* input, char *filePath)
{
    char buf[BUFSIZE];
    int ret;
    char host[60];
    gethostname(host, sizeof(host));
    char hrequest[1024];
    //check if method is GET
    if(strcmp(input, "GET")==0)
    {
        if(filePath ==NULL)
        {
            sprintf(hrequest, "GET / HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",URL, "en-us",host);
            printf("%s\n",hrequest);
        }
        else
        {

            sprintf(hrequest, "GET %s HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",filePath, URL, "en-us", host);
            printf("%s\n",hrequest);
        }
    }
    //check if method is GET
    else if(strcmp(input,"HEAD")==0)
    {
        if(filePath ==NULL)
        {
            sprintf(hrequest, "HEAD / HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",URL, "en-us",host);
            printf("%s\n",hrequest);
        }
        else
        {
            sprintf(hrequest, "HEAD %s HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",filePath,URL, "en-us",host);
            printf("%s\n",hrequest);
        }
    }
    //check if method is GET
    else if(strcmp(input,"TRACE")==0)
    {
        if(filePath ==NULL)
        {
            sprintf(hrequest, "HEAD / HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",URL, "en-us",host);
            printf("%s\n",hrequest);
        }
        else
        {
            sprintf(hrequest, "TRACE %s HTTP/1.1\r\n\r\n \nHOST: %s \nAccept-Language:%s  \nFrom:%s",filePath,URL, "en-us",host);
            printf("%s\n",hrequest);
        }
    }

    write(sd, hrequest, sizeof(hrequest)-1);
    memset(&buf, 0, sizeof(buf));

    printf("\n\nReceived information:\n\n");


    while((ret = recv(sd, buf, BUFSIZE-1,0))!=0)
    {
        if(fl==1)
        {
            //print out whole response
            printf("%s", buf);
            memset(&buf, 0, BUFSIZE);
        }//fl = 0
        else //if -a is disable
        {
            char *body_capture;
            //char *header_capture;

            body_capture = strtok(buf, "<");
            //int hs = sizeof(header_capture);
            printf("<");

            do
            {
                if(!(body_capture = strtok(NULL, "")))
                {
                    break;
                }
                //print only the content without header field
                printf("%s",body_capture);
            }
            while(1);
        }
        printf("\n");
    }//end while
}

char *checkPort(char *input, int *p, char host[])
{
    const char ch = ':';
    const char s[2] = ":";
    const char t[2] = "/";
    char *token;
    char *URL;
    if(strstr(input, "http://") != NULL)
    {
        input = input + 7;
    }
    if(strchr(input, ch)==NULL)//use default port
    {
        *p = SERV_TCP_PORT;
        if(strstr(input, t) == NULL)//no file to get
        {
            strcpy(host,input);
            return NULL;
        }
        else
        {
            token = strtok(input, t);
            strcpy(host,token);
            token = strtok(NULL, "");//move to the token after / to get file URL
            URL = token;
            return URL;
        }

    }
    else //use assigned Port
    {
        token = strtok(input, s);
        strcpy(host,token);
        token = strtok(NULL, s);//move to the token after :
        if (atoi(token) >= 1024 && atoi(token) < 65536)
        {
            *p = atoi(token);
            if(strstr(token, t) != NULL)
            {
                token = strtok(token, t);
                token = strtok(NULL, "");//move to the token after /
                URL = token;
                return URL;
            }
        }
        else
        {
            printf("Error: port number must be between 1024 and 65535\n");
            exit(1);
        }//end else
        return NULL;
    }
}

