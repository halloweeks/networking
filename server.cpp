#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

#define ADDRESS "0.0.0.0"
#define PORT 8080
#define CONCURRENT_CONNECTION 200
#define QUEUE_CONNECTION 100
#define BUFFER_SIZE 65356
#define THREAD_STACK_SIZE 524288

pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

int connection = 0;

void *connection_handler(void*);

int main(int argc, char *argv[]) {
	pthread_t thread_id;
	
	pthread_attr_t attr;
	
	if (pthread_attr_init(&attr) != 0) {
		printf("[ERROR][THREAD][INIT] %s\n", strerror(errno));
		return 1;
	}
	
	if (pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) != 0) {
		printf("[ERROR][THREAD][STACK] %s\n", strerror(errno));
		return 1;
	}
	
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		printf("[ERROR][THREAD][DETACH] %s\n", strerror(errno));
		return 1;
	}
	
	int master_socket, conn_id;
	struct sockaddr_in server, client;
	
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	
	signal(SIGPIPE, SIG_IGN);
	
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("[ERROR] CAN'T CREATE TO SOCKET\n");
		return 1;
	} else {
		printf("[NOTE] SOCKET CREATED DONE\n");
	}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ADDRESS);
	server.sin_port = htons(PORT);
	
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
	if (bind(master_socket, (struct sockaddr *)&server, sizeof(server)) == -1) {
		printf("[ERROR][BIND] %s\n", strerror(errno));
		return 1;
	} else {
		printf("[NOTE] BIND %s:%d\n", ADDRESS, PORT);
	}
	
	if (listen(master_socket, QUEUE_CONNECTION) == -1) {
		printf("[ERROR][LISTEN] %s\n", strerror(errno));
		return 1;
	} else {
		printf("[INFO] WAITING FOR INCOMING CONNECTIONS\n");
	}
	
	while (1) {
		conn_id = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen);
		
		if (conn_id == -1) {
			printf("[WARNING] CAN'T ACCEPT NEW CONNECTION\n");
		} else {
			if (connection >= CONCURRENT_CONNECTION) {
				printf("[WARNING] CONNECTION LIMIT REACHED\n");
				close(conn_id);
			} else {
				int *conn_id_heap = (int*)malloc(sizeof(int));
				
				if (conn_id_heap == NULL) {
					perror("[ERROR] Memory allocation failed");
					close(conn_id);
					continue;
				}
				
				*conn_id_heap = conn_id;
				
				printf("[INFO] NEW CONNECTION ACCEPTED FROM %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
				
                if (pthread_create(&thread_id, &attr, connection_handler, (void*)conn_id_heap) == -1) {
                    printf("[WARNING] CAN'T CREATE NEW THREAD\n");
                    close(conn_id);
                    free(conn_id_heap);
                } else {
                    printf("[INFO] NEW THREAD CREATED\n");
                    connection++;
                }
            }
        }
    }
    
    return 0;
}

void *connection_handler(void *sock_fd) {
	int conn_id = *((int *)sock_fd);
	free(sock_fd);
	
	clock_t start, end;
	start = clock();
	
    char buffer[BUFFER_SIZE] = {0};
    
    char response[1024] = "user=hello&pass=1234\n";

    while (recv(conn_id, buffer, BUFFER_SIZE, 0) > 0) {
    	printf("[RECEIVED] %s\n", buffer);
        memset(buffer, 0, BUFFER_SIZE);

        if (send(conn_id, response, strlen(response), 0) > 0) {
            printf("[SEND] %s\n", response);
        } else {
            printf("[WARNING][SEND] %s\n", strerror(errno));
        }
    }

    close(conn_id);
    
    printf("[INFO] CONNECTION CLOSED\n");
    
    pthread_mutex_lock(&connection_mutex);
    connection--;
    pthread_mutex_unlock(&connection_mutex);
   
    printf("[INFO] THREAD TERMINATED\n");
    
    end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[TIME] PROCESS COMPLETE IN %.5f SEC\n", time_taken);
    printf("------------------------\n");
    
    pthread_exit(NULL);
}
