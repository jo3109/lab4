#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 3490
#define MAXDATASIZE 100

int main(int argc, char *argv[]){
	int csock, numbytes;
	char buf[MAXDATASIZE];
	struct sockaddr_in serv_addr;
	int len;

	if(argc != 3){
		fprintf(stderr, "Usage : tcp_client <SERVER_IP> <ECHO STRING>\n");
		exit(1);
	}

	// 클라이언트 소켓 생성
	if((csock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	// 서버 주소 설정
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(PORT);

	// 서버 연결
	if(connect(csock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr)) == -1){
		perror("connect");
		close(csock);
		exit(1);
	}

	// 전송할 문자열 설정 및 전송
	memset(buf, 0, MAXDATASIZE);
	strcpy(buf, argv[2]);
	len = strlen(buf);

	sprintf(buf, "Hello, server\n");
	if((numbytes = send(csock, buf, MAXDATASIZE, 0)) == -1){
		fprintf(stderr, "send failed...\n");
		close(csock);
		exit(1);
	}

	// 서버로부터 응답 수신
	memset(buf, 0, MAXDATASIZE);
	if((numbytes = recv(csock, buf, MAXDATASIZE, 0)) == -1){
		perror("recv");
		close(csock);
		exit(1);
	}
	buf[numbytes] = '\0';
	// 수신한 메시지 출력
	printf("Received : %s\n", buf);

	// 소켓 닫기
	close(csock);
	return 0;
}

