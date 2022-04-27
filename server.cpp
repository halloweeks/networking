#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> // multi threading for using

// this header included for count execution time
#include <time.h>
#include <iomanip>

// maximum concurrent connections
#define CONCURRENT_CONNECTION 20

// max connection requests queued
#define QUEUE_CONNECTION 40

// default port number
#define PORT 8888

// default buffer size 1024 bytes
#define BUFFER_SIZE 5

// currently connections
int connection = 0;

// connection handler function
void *connection_handler(void *);

int main(int argc, char *argv[]) {
	// thread identifier
	pthread_t thread_id;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 1024);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	int master_socket, conn_id, len;
	struct sockaddr_in server, client;
	
	// creating master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "[ERROR] CAN'T CREATE TO SOCKET\n";
		return 0;
	} else {
		std::cout << "[NOTE] SOCKET CREATED DONE\n";
	}
	
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    len = sizeof(struct sockaddr_in);
    
    // binding socket
    if (bind(master_socket, (struct sockaddr *)&server , sizeof(server)) == -1) {
    	perror("[ERROR]");
        return 0;
    } else {
    	std::cout << "[NOTE] BINDING DONE\n";
    }
    
    // Listen on the socket, with 40 max connection requests queued
    if (listen(master_socket, QUEUE_CONNECTION) == -1) {
    	std::cout << "[ERROR] CAN'T LISTEN\n";
        return 0;
    } else {
    	std::cout << "[INFO] WAITING FOR INCOMING CONNECTIONS\n";
    }
	
	// infinity loop
    while (true) {
    	// accept new connections
        conn_id = accept(master_socket, (struct sockaddr*)&client, (socklen_t*)&len);
        
        // if connection not accepted
        if (conn_id == -1) {
        	std::cout << "[WARNING] CAN'T ACCEPT NEW CONNECTION\n";
            return 0;
        } else {
        	// if connection limit reached
        	if (connection >= CONCURRENT_CONNECTION) {
        	    std::cout << "[WARNING] CONNECTION LIMITE REACHED" << std::endl;
                send(conn_id, "SERVER IS BUSY.", 15, 0);
                close(conn_id);
        	} else {
        	    std::cout << "[INFO] NEW CONNECTION ACCEPTED\n";
                // create thread for new connection
                if (pthread_create(&thread_id, &attr, connection_handler, new int(conn_id)) == -1) {
                	std::cout << "[WARNING] CAN'T CREATE NEW THREAD\n";
                    // if the thread is not made then we will close the client connection
                    close(conn_id);
                } else {
                	std::cout << "[INFO] NEW THREAD CREATED\n";
                    connection++; // increase connection count
                }
            }
        }
    }
    return 0;
}

// This will handle connection for each client
void *connection_handler(void *sock_fd) {
	/* clock_t clock(void) returns the number of clock ticks 

       elapsed since the program was launched.To get the number  

       of seconds used by the CPU, you will need to divide by  

       CLOCKS_PER_SEC.where CLOCKS_PER_SEC is 1000000 on typical 

       32 bit system.  */

    clock_t start, end; 
	
	// Recording the starting clock tick.
    start = clock(); 
	
	// Get the socket descriptor
	int* conn_id = new int(*(int*)sock_fd);
		
	// allocate memory for buffer
	char* buffer = new char[BUFFER_SIZE];
	
	// server response message
	const char* response = "Hello, client!";
	
	// receiving messages from client
	if (recv(*conn_id, buffer, BUFFER_SIZE, 0) == -1) {
		std::cout << "[WARNING] FAILED TO READ PACKETS" << std::endl;
	} else {
		std::cout << "[REQUEST] " << buffer << std::endl;
	}
	
	// send message to client
	if (send(*conn_id, response, strlen(response), 0) == -1) {
		std::cout << "[WARNING] CAN'T SEND RESPONSE\n";
	} else {
		std::cout << "[RESPONSE] " << response << std::endl;
	}
	
	// terminate thread process
	if (pthread_detach(pthread_self()) == 0) {
		std::cout << "[INFO] THREAD TERMINATED\n";
	} else {
		std::cout << "[ERROR] CAN'T TERMINATE THREAD\n";
	}
	
	// close client connection
	if (close(*conn_id) == 0) {
		std::cout << "[INFO] CONNECTION CLOSED\n";
	} else {
		std::cout << "[ERROR] CAN'T CLOSE CONNECTION\n";
	}
	
	// decrease connection counts
	connection--;
	
	// deallocate memory
	delete[] buffer;
	delete (int*)sock_fd;
	delete conn_id;
	
	// Recording the end clock tick. 
    end = clock();
    
    // Calculating total time taken by the program. 
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC); 

    std::cout << "[TIME] PROCESS COMPLETE IN " << std::fixed << time_taken << std::setprecision(5); 
    std::cout << "sec" << std::endl;
	
	
	// exiting thread function
	pthread_exit(NULL);
} 
