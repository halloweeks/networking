#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#define ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
	char buffer[BUFFER_SIZE] = {0};
	std::string input;
	
	int sock = 0, valread = 0;
	struct sockaddr_in serv_addr;
	
	memset(&serv_addr, 0, sizeof(serv_addr));
		
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "[ERROR] CAN'T CREATE SOCKET" << "\n";
		return 0;
	}
	
	// assign IP, PORT
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) == -1) {
		std::cout << "[ERROR] INVALID ADDRESS/ ADDRESS NOT SUPPORTED" << "\n";
		return 0;
	}
	
	// connect
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		std::cout << "[ERROR] CAN'T CONNECT TO THE HOST " << ADDRESS << ":" << PORT << "\n";
		return -1;
	}
	
	for (;;) {
		std::cout << "[TCP]> ";
		std::cin >> input;
		
		if (input == "") {
			input = "default message";
		} else if (input == "exit") {
			std::cout << "Client connection terminated\n";
			break;
		}
		
		if (send(sock, input.c_str(), input.size(), 0) > 0) {
        	std::cout << "[SEND] " << input << "\n";
        } else {
        	std::cout << "Failed to transfer\n";
        }
		
		if (recv(sock, buffer, BUFFER_SIZE, 0) > 0) {
			std::cout << "[RECEIVE] " << buffer << "\n";
			// clear server response data
			memset(buffer, 0, BUFFER_SIZE);
		} else {
			std::cout << "[ERROR] CAN'T READ SERVER RESPONSE" << "\n";
		}
	}
		
	// close connection
	close(sock);
	return 0; 
}
