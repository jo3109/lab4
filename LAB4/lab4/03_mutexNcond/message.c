#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 5
#define MAX_MESSAGE 256

// 공유 데이터 구조체
typedef struct {
	char message[MAX_MESSAGE];
	int message_available;
	pthread_mutex_t mutex;
	pthread_cond_t message_cond;
	int client_count;
} SharedData;

SharedData shared_data;

// 클라이언트(자식) 쓰레드 함수
void* client_thread(void* arg) {
	int client_id = *((int*)arg);
	char message[MAX_MESSAGE];

	while(1) {
		// 메시지 생성
		snprintf(message, MAX_MESSAGE, "Message from Client %d", client_id);

		// 뮤텍스 잠금
		pthread_mutex_lock(&shared_data.mutex);

		// 메시지 전송
		strcpy(shared_data.message, message);
		shared_data.message_available = 1;

		// 다른 쓰레드에게 알림
		pthread_cond_broadcast(&shared_data.message_cond);

		// 뮤텍스 해제
		pthread_mutex_unlock(&shared_data.mutex);

		// 다른 클라이언트의 메시지 수신 대기
		pthread_mutex_lock(&shared_data.mutex);
		while (!shared_data.message_available) {
			pthread_cond_wait(&shared_data.message_cond, &shared_data.mutex);
		}

		// 메시지 수신 및 출력
		printf("Client %d received: %s\n", client_id, shared_data.message);
		shared_data.message_available = 0;

		pthread_mutex_unlock(&shared_data.mutex);

		// 잠시 대기
		sleep(2);
	}

	return NULL;
}

int main() {
	pthread_t clients[MAX_CLIENTS];
	int client_ids[MAX_CLIENTS];

	// 공유 데이터 초기화
	pthread_mutex_init(&shared_data.mutex, NULL);
	pthread_cond_init(&shared_data.message_cond, NULL);
	shared_data.message_available = 0;
	shared_data.client_count = MAX_CLIENTS;

	// 클라이언트 쓰레드 생성
	for(int i = 0; i < MAX_CLIENTS; i++) {
		client_ids[i] = i + 1;
		if(pthread_create(&clients[i], NULL, client_thread, &client_ids[i]) != 0) {
			perror("Failed to create client thread");
			exit(1);
		}
	}

	// 클라이언트 쓰레드 종료 대기
	for(int i = 0; i < MAX_CLIENTS; i++) {
		pthread_join(clients[i], NULL);
	}

	// 리소스 정리
	pthread_mutex_destroy(&shared_data.mutex);
	pthread_cond_destroy(&shared_data.message_cond);

	return 0;
}
