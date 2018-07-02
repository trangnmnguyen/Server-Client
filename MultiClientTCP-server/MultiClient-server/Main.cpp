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

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				//accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				//add new connection to the list of connected client
				FD_SET(client, &master);
				//send a welcome message to a connected client
				string welcomeMsg = "Welcome to the chat server";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
				//TODO: broadcast that we have a new connected client
			}
			else {
				char buff[4096];
				ZeroMemory(buff, 4096);

				//receive message
				int bytesIn = recv(sock, buff, 4096, 0);
				if (bytesIn <= 0) {
					//drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					//send message to other client, not the listening message
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {
							send(outSock, buff, bytesIn, 0);
						}
					}
				}
				
			}
		}
	}
	//shutdown winsock
	WSACleanup();
}

////wait for a connection
//sockaddr_in client;
//int clientsize = sizeof(client);

//SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

////if(clientSocket == INVALID_SOCKET) do something

//char host[NI_MAXHOST]; //client remote name
//char service[NI_MAXSERV]; //service (i.e port) the client is connect on

//ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0, NI_MAXHOST);
//ZeroMemory(service, NI_MAXSERV);

//if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
//	cout << host << "connected on port" << service << endl;
//}
//else {
//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//	cout << host << "connected on port" << ntohs(client.sin_port) << endl;
//}

////close listening socket	
//closesocket(listening);

////while loop: accept and echo message to client
//char buf[4096];
//while (true) {
//	ZeroMemory(buf, 4096);

//	//wait for client to send data
//	int bytesReceived = recv(clientSocket, buf, 4096, 0);

//	if (bytesReceived == SOCKET_ERROR) {
//		cerr << "error in recv(). Quitting" << endl;
//		break;
//	}

//	if (bytesReceived == 0) {
//		cout << "Client disconnected" << endl;
//		break;
//	}
//	//cout << string(buf, 0, bytesReceived) << endl;
//	cout << buf << endl;
//	//echo message back to client
//	send(clientSocket, buf, bytesReceived + 1, 0);

//}

//close the socket
//closesocket(clientSocket);