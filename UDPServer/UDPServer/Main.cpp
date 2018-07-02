#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
	//startup winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOk = WSAStartup(version, &data);

	if (wsOk != 0) {
		cout << "cant start winsock" << wsOk;	
	}

	//bind socket to ip address and port
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(54000); //convert from little to big endian  

	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) != SOCKET_ERROR) {
		cout << "cant bind socket!" << WSAGetLastError() << endl;
		return;
	}

	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);
	
	char buff[1024];
	
	//enter a loop
	while (true) {
		ZeroMemory(buff, 1024); //zero buffer anytime we recv it
		//wait for message
		int bytesIn = recvfrom(in, buff, sizeof(buff), 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR) {
			cout << "error receiving from client" << WSAGetLastError() << endl;
			continue;
		}
		//display message and client info
		char clientIp[256];
		ZeroMemory(clientIp, 256);

		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		cout << "message receive from " << clientIp << " : " << buff << endl;
	}		
	//close socket
	closesocket(in);
	//shut down winsock
	WSACleanup();
}