#include <iostream> // input output
#include <string> // strlen string
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> // multi threading
#include <errno.h>

// Included for count execution time
#include <time.h>
#include <iomanip>

// server address
#define ADDRESS "0.0.0.0"

// port number
#define PORT 8888

// maximum concurrent connections
#define CONCURRENT_CONNECTION 10

// maximum connection requests queued
#define QUEUE_CONNECTION 20

// buffer size 1KB
#define BUFFER_SIZE 1024

// Thread stack size 64KB
#define THREAD_STACK_SIZE 65536

// current connections
int connection = 0;

// connection handler function
void *connection_handler(void*);

int main(int argc, char *argv[]) {
	// thread identifier
	pthread_t thread_id;
	// thread attribute
	pthread_attr_t attr;
        
	if (pthread_attr_init(&attr) != 0) {
		std::cout << "[ERROR][THREAD][INIT] " << strerror(errno) << "\n";
		return -1;
	}
        
	// stack size 1MB
	if (pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) != 0) {
		std::cout << "[ERROR][THREAD][STACK] " << strerror(errno) << "\n";
		return -1;
	}
	
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		std::cout << "[ERROR][THREAD][DETACH] " << strerror(errno) << "\n";
		return -1;
	}
	
	int master_socket, conn_id, len;
	struct sockaddr_in server, client;
	
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	
	// creating master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "[ERROR] CAN'T CREATE TO SOCKET\n";
	    return -1;
	} else {
		std::cout << "[NOTE] SOCKET CREATED DONE\n";
	}
	
	
     // Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ADDRESS);
	server.sin_port = htons(PORT);
    
	len = sizeof(struct sockaddr_in);
    
	// binding address and port
	if (bind(master_socket, (struct sockaddr *)&server , sizeof(server)) == -1) {
		std::cout << "[ERROR][BIND] " << strerror(errno) << "\n";
	    return -1;
	} else {
		std::cout << "[NOTE] BIND " << ADDRESS << ":" << PORT << "\n";
	}
	
	// Listen on the socket, with 20 max connection requests queued
	if (listen(master_socket, QUEUE_CONNECTION) == -1) {
		std::cout << "[ERROR][LISTEN] " << strerror(errno) << "\n";
		return -1;
	} else {
		std::cout << "[INFO] WAITING FOR INCOMING CONNECTIONS\n";
	}
    
	// infinity loop
	while (true) {
		// accept new connections
		conn_id = accept(master_socket, (struct sockaddr*)&client, (socklen_t*)&len);
		
        // if connection acception failed
        if (conn_id == -1) {
        	std::cout << "[WARNING] CAN'T ACCEPT NEW CONNECTION " << conn_id << " ERRNO " << errno << "\n";
        } else {
        	 // if connection limit reached
        	 if (connection >= CONCURRENT_CONNECTION) {
        	     std::cout << "[WARNING] CONNECTION LIMITE REACHED\n";
                     send(conn_id, "server is busy. please try again later.", 39, 0);
                     close(conn_id); // close connection
        	 } else {
        	     std::cout << "[INFO] NEW CONNECTION ACCEPTED\n";
                     // create new thread for new connection
                     if (pthread_create(&thread_id, &attr, connection_handler, new int(conn_id)) == -1) {
                         std::cout << "[WARNING][THREAD] " << strerror(errno) << "\n";
                	 // std::cout << "[WARNING] CAN'T CREATE NEW THREAD\n";
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
	
	// byte size
	int read_byte = 0;
	
	// Get the socket descriptor
	int conn_id = *(int*)sock_fd;
	
	// request data
	char buffer[BUFFER_SIZE] = {0};
	
	// response data
	char response[] = "Hello ðŸ–ï¸";
	
	// read response continue
	while ((read_byte = recv(conn_id, buffer, BUFFER_SIZE, 0)) > 0) {
		std::cout << "[RECEIVED] " << buffer << "\n";
		// clear buffer data
		memset(buffer, 0, BUFFER_SIZE);
		
		// send response
		if (send(conn_id, response, strlen(response), 0) > 0) {
			std::cout << "[SEND] " << response << "\n";
		} else {
			std::cout << "[WARNING][SEND] " << strerror(errno) << "\n";
		}
		
	}
	
	// terminate connection
	close(conn_id);
	std::cout << "[INFO] CONNECTION CLOSED\n";
	
	// decrease connection counts
	connection--;
	
	// thread automatically terminate after exit connection handler
	std::cout << "[INFO] THREAD TERMINATED" << std::endl;
	
	delete (int*)sock_fd;
	
	// Recording the end clock tick. 
	end = clock();
    
	// Calculating total time taken by the program. 
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC); 
    
	// print process time
	std::cout << "[TIME] PROCESS COMPLETE IN " << std::fixed << time_taken << std::setprecision(5); 
	std::cout << " SEC" << std::endl;
	
	// print line
	std::cout << "------------------------" << std::endl;
	
	// exiting
	pthread_exit(NULL);
} 
