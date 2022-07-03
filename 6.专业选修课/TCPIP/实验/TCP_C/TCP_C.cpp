#include "winsock2.h"
#include "stdio.h"
#pragma comment(lib, "ws2_32.lib")
//#define DEFAULT_PORT 5050
//#define DATA_BUFFER 1024

void main()
{
	WSAData wsa;
	WSAStartup(WINSOCK_VERSION, &wsa);

	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("Creating Socket error!");
		return;
	}

	struct sockaddr_in ServerAddr;
	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("192.168.43.68");
	ServerAddr.sin_port = htons(5000);

	if (connect(ClientSocket, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("connect error!");
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	else
	{
		printf("connect to server: %s successful\n", inet_ntoa(ServerAddr.sin_addr));
	}
	char buf[1024] = "\0";
	if (recv(ClientSocket, buf, sizeof(buf), 0) > 0)
	{
		printf("receive data from server : %s\n", buf);
	}
	char data[100] = "Hello";
	int len = strlen(data);
	data[len] = '\0';
	if (send(ClientSocket, data, strlen(data) + 1, 0) == SOCKET_ERROR)
	{
		printf("send hellp to server error");
	}
	else
	{
		printf("send hello to server successful\n");
	}
	if (ClientSocket != INVALID_SOCKET)
	{
		closesocket(ClientSocket);
	}
	WSACleanup();
}