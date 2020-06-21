#include <string.h>
#include "unp.h"

int main(int argc, char**argv){
    struct sockaddr_in servaddr, recvaddr;
    int len=sizeof(servaddr);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERV_PORT);
    servaddr.sin_addr.s_addr=htonl(INADDR_BROADCAST);

    int sockfd;
    sockfd=socket(AF_INET, SOCK_DGRAM, 0);
    int optval = 1;    
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval,sizeof(int));

    char buffer[MAXLINE]="datetime";
    Sendto(sockfd, buffer, MAXLINE, 0, (SA*)&servaddr, sizeof(servaddr));

    int n=Recvfrom(sockfd, buffer, MAXLINE, 0, (SA*)&recvaddr, &len);
    buffer[n]='\0';
    printf("%s\r\n", buffer);
    exit(0);
}
