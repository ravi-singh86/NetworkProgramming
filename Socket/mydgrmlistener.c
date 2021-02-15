#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "4950"
#define MAX_MSG_SIZE 1000
//Usage execute code in shell1 from another shell send msg to this service
//telnet localhost 3490

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(){
    struct addrinfo hint, *res, *addrp;
    struct sockaddr theirAddr;
	char buff[MAX_MSG_SIZE];
    char s[INET6_ADDRSTRLEN];
    int sockfd;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_PASSIVE;

    int rc = getaddrinfo(NULL, PORT, &hint, &res);
    if(rc !=0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    for(addrp = res; addrp!=NULL; addrp = addrp->ai_next){
        if((sockfd = socket(addrp->ai_family, addrp->ai_socktype, addrp->ai_protocol))==-1){
            perror("Unable to open socket");
            continue;
        }
       
        int yes = 1;
        if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof yes) == -1) {
            perror("setdockopt");
            continue;
        }
        
        if(bind(sockfd, addrp->ai_addr, addrp->ai_addrlen) == -1) {
            perror("Unable to bind to socket");
            continue;
        }
        break;
    }
	if(addrp == NULL){
		printf(" listenr failed to bind");
		return 2;
	}

    freeaddrinfo(res);

	int addrLen = sizeof(theirAddr);
	while(1){
		int byte = recvfrom(sockfd, buff, sizeof(buff)-1, 0, &theirAddr, &addrLen);
		buff[byte] = '\0';

		inet_ntop(theirAddr.sa_family,get_in_addr(&theirAddr), s, sizeof(s));
		printf("Msg received from: %s and message is %s \n", s, buff);
	}
    return 0;
}


