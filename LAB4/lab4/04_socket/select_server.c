#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>

#define MYPORT 3490
#define QLEN 6

int main(){
	int sockfd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in cad; // 클라이언트 addr
	int alen;

	fd_set readfds, activefds;
	int i, maxfd = 0, numbytes;
	char buf[100];

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket() failed");
		exit(1);	
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(MYPORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0){
		perror("bind() failed");
		exit(1);
	}

	if(listen(sockfd, QLEN) < 0){
		perror("listen");
		exit(1);
	}

	alen = sizeof(cad);

	FD_ZERO(&activefds);
	FD_SET(sockfd, &activefds);
	maxfd = sockfd;
	fprintf(stderr, "Servere up and running.\n");
	while(1){
		printf("SERVER : Waiting for contact... %d\n", maxfd);

		readfds = activefds;
		if(select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0){
			perror("select");
			exit(EXIT_FAILURE);
		}
		for(i = 0; i <= maxfd; ++i){
			if (FD_ISSET(i, &readfds)){

				if((new_fd = accept(sockfd, (struct sockaddr *)&cad, &alen)) < 0){
					perror("accept");
					exit(1);
				}

				FD_SET(new_fd, &activefds);
				printf("set\n");
				if(new_fd > maxfd)
					maxfd = new_fd;
				else{
					if((numbytes=recv(i, buf, 100, 0))==-1)
						perror("recv");
					buf[numbytes] = '\0';
					if(send(i, buf, strlen(buf), 0) == -1)
						perror("send");

					close(i);
					FD_CLR(i, &activefds);
				}
			}
		}
	}

	close(new_fd);
}
