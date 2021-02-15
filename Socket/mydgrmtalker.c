//TCP client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "4950" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char* argv[]){
	struct addrinfo hint, *res, *addrp;
	int sockfd;
	char s[INET6_ADDRSTRLEN];

	if(argc <2){
		printf("Usage: ./taskname serverAddress");
		return 1;
	}

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1],PORT, &hint, &res);
	if(rc != 0 ){
		perror("getAddrinfo error:");
		return 1;
	}
	
	for(addrp = res; addrp != NULL; addrp = addrp->ai_next){
		sockfd = socket(addrp->ai_family, addrp->ai_socktype, addrp->ai_protocol);
		if(sockfd == -1){
			perror("sockfd");
			continue;
		}
		break;
	}

	if(addrp == NULL){
		printf("client: failed to connect\n");
		return 2;
	}

	inet_ntop(addrp->ai_family, get_in_addr(addrp->ai_addr), s, sizeof(s));
	printf("client: connecting to %s\n", s);
	
	const char* msg = "Hello There I am fast I send and forget\n";
	int bytes = sendto(sockfd, msg, strlen(msg), 0, addrp->ai_addr, addrp->ai_addrlen);

	printf("sent %d bytes of data which is %s \n", bytes, msg);
	freeaddrinfo(res);

	close(sockfd);
	return 0;
}
