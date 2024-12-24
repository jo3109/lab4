#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *sleep_sorting(void *arg){
	int sleep_time = *(int *)arg;
	usleep(sleep_time*100);
	printf("%d ", sleep_time);
	free(arg); // 동적 할당 메모리 해제
	return arg;
}

int main(void){
	int n;
	pthread_t *tid;
	int i, status;
	printf("정렬할 값의 개수 : ");
	scanf("%d", &n);

	tid = malloc(n * sizeof(pthread_t));

	for(i = 0; i < n; i++){
		int *element = malloc(sizeof(int));
		scanf("%d", element);

		status = pthread_create(&tid[i], NULL, sleep_sorting, element);
		if(status != 0){
			fprintf(stderr, "Create thread %d: %d", i, status);
			exit(1);
		}
	}
	for(i = 0; i < n; i++){
		pthread_join(tid[i], NULL); // 각 스레드가 종료될 때까지 대기
	}
	printf("\n");
	free(tid);
	pthread_exit(NULL);
}
