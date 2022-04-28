#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

#define ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
	
	char ResponseData[BUFFER_SIZE];
	std::string RequestData;
	
	while (true) {
		int sock = 0, valread = 0;
		struct sockaddr_in serv_addr;
		
		memset(&serv_addr, 0, sizeof(serv_addr));
		
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			std::cout << "[ERROR] CAN'T CREATE SOCKET" << std::endl;
			return 0;
		}
		
		// assign IP, PORT
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);
		
		// Convert IPv4 and IPv6 addresses from text to binary form
		if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) == -1) {
			std::cout << "[ERROR] INVALID ADDRESS/ ADDRESS NOT SUPPORTED" << std::endl;
			return 0;
		}
		
		// connect
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
			std::cout << "[ERROR] CAN'T CONNECT TO THE HOST " << ADDRESS << ":" << PORT << std::endl;
			return 0;
		}
		
		std::cout << "[message]> ";
		getline(std::cin, RequestData);
		
		if (RequestData == "") {
			RequestData = "default message";
		}
		
		if (send(sock, RequestData.c_str(), BUFFER_SIZE, 0) > 0) {
			std::cout << "[SEND] " << RequestData << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T SEND MESSAGE TO SERVER" << std::endl;
		}
		
		if (recv(sock, ResponseData, BUFFER_SIZE, 0) > 0) {
			std::cout << "[RECEIVE] " << ResponseData << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T READ SERVER RESPONSE" << std::endl;
		}
		
		// clear server response data
		memset(ResponseData, 0, BUFFER_SIZE);
		RequestData = "";
		
		// close connection
		close(sock);
    }
    
  return 0; 
}
