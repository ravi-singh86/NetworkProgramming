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

#define PORT "3490"
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

    int sockfd;
    int MAX_REQUEST = 20;
    char clientAddress[INET6_ADDRSTRLEN];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
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

    freeaddrinfo(res);

    if(listen(sockfd,MAX_REQUEST) == -1){
        perror("listen error");
        return -1;
    }    

    while(1){
        socklen_t addrlen = sizeof(struct sockaddr);

        int newfd = accept(sockfd, &theirAddr,    &addrlen);
        if(newfd == -1){
            perror("Accept error");
            continue;
        }
        
        inet_ntop(theirAddr.sa_family, 
                  get_in_addr(&theirAddr), 
                  clientAddress, 
                  sizeof(clientAddress));

        printf("Server: got request from %s\n",clientAddress);

        const char* msg = "Hellow There\n";
        
        int bytes = send(newfd, msg, strlen(msg),0);
        printf("send sucessfully %d bytes\n",bytes);
        close(newfd);         
    }
    return 0;
}


