#include <iostream>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
	//initiate winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk) {
		cerr << "Can't initiate winsock! Quitting" << endl;
		return;
	}

	//create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl;
	}

	//bind the socket to an ip address and port 
	sockaddr_in hint;
	hint.sin_family = AF_INET;		
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	//wait for a connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

	//if(clientSocket == INVALID_SOCKET) do something
	
	char host[NI_MAXHOST]; //client remote name
	char service[NI_MAXSERV]; //service (i.e port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << "connected on port" << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" << ntohs(client.sin_port) << endl;
	}

	//close listening socket	
	closesocket(listening);

	//while loop: accept and echo message to client
	char buf[4096];
	while (true) {
		ZeroMemory(buf, 4096);

		//wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR) {
			cerr << "error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client disconnected" << endl;
			break;
		}
		//cout << string(buf, 0, bytesReceived) << endl;
		cout << buf << endl;
		//echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);	
		
	}

	//close the socket
	closesocket(clientSocket);

	//shutdown winsock
	WSACleanup();
}