#include <string.h>
#include "unp.h"
#include "time.h"

int main(int argc, char**argv){
    struct sockaddr_in servaddr, cliaddr;
    time_t ticks;
    int len=sizeof(servaddr);
    int sockfd;
    sockfd=socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(SERV_PORT);
    bind(sockfd, (SA*)&servaddr, sizeof(servaddr));


    char buffer[MAXLINE];
    char*msg="datetime";

    while(1){
        memset(buffer, 0, MAXLINE);
        Recvfrom(sockfd, buffer, MAXLINE, 0, (SA*)&cliaddr, &len);
        if(strcmp(buffer, msg)!=0)
            continue;
        ticks = time( NULL );
		snprintf( buffer , sizeof(buffer), "%.24s\r\n" , ctime(&ticks) );
        Sendto(sockfd, buffer, MAXLINE, 0, (SA*)&cliaddr, sizeof(cliaddr));
    }
    exit(0);
}
