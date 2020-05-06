#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define MAX_LENGTH 1024

int main(int argc, char**argv){
    char* mode="r";
    char cmdstring[MAX_LENGTH]={0}, buffer[MAX_LENGTH+1]={0};
    FILE*handle=NULL;
    int n,sockfd;
    struct sockaddr_in servaddr;

    if(argc<3){
        printf("usage: telnet <IP address> <command> <arg1> <arg2> ...\r\n");
        exit(1);
    }

    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){
        printf("socket error!\r\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(50000);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0){
        printf("inet_ptonn error for %s\r\n", argv[1]);
    }

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))){
        printf("connect error\r\n");
        exit(1);
    }

    for(int i=2;i<argc;i++){
        if(i!=2)
            strcat(cmdstring, " ");
        strcat(cmdstring, argv[i]);
    }

    write(sockfd, cmdstring, strlen(cmdstring));

    while((n=read(sockfd, buffer, MAX_LENGTH))>0){
        buffer[n]='\0';
        printf("%s", buffer);
    }

    if(n<0){
        printf("read error\r\n");
        exit(1);
    }
    exit(0);
}