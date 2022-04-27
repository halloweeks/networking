#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#define ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]){
	
	while (true) {
		
		int sock = 0, valread;
		struct sockaddr_in serv_addr;
		char buffer[BUFFER_SIZE];
		std::string msg = "Hello, server";
		
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			std::cout << "[ERROR] CAN'T CREATE SOCKET" << std::endl;
			return 0;
		} else {
			std::cout << "[NOTE] SOCKET CREATED" << std::endl;
		}
		
		// assign IP, PORT
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);
		
		// Convert IPv4 and IPv6 addresses from text to binary form
		if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) == -1) {
			std::cout << "[ERROR] INVALID ADDRESS/ ADDRESS NOT SUPPORTED" << std::endl;
			return 0;
		} else {
			std::cout << "[NOTE] ADDRESS IS OK" << std::endl;
		}
		
		// connect
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
			std::cout << "[ERROR] CAN'T CONNECT TO THE HOST " << ADDRESS << ":" << PORT << std::endl;
			return 0;
		} else {
			std::cout << "[NOTE] CONNECTION CONNECTED" << std::endl;
		}
		
		if (send(sock, msg.c_str(), BUFFER_SIZE, 0) > 0) {
			std::cout << "[SEND] " << msg << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T SEND MESSAGE TO SERVER" << std::endl;
		}
		
		if (recv(sock, buffer, BUFFER_SIZE, 0) > 0) {
			std::cout << "[RECEIVE] " << buffer << std::endl;
			memset(buffer, 0, BUFFER_SIZE);
		} else {
			std::cout << "[ERROR] CAN'T READ SERVER RESPONSE" << std::endl;
		}
    }
  return 0; 
}
