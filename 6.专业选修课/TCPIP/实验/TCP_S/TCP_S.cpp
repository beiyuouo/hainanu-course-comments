#include "Winsock2.h"
#include "stdio.h"
#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_PORT 5050
#define DATA_BUFFER 1024

void main()
{
	WSAData wsa;
	WSAStartup(WINSOCK_VERSION, &wsa);

	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (ServerSocket == INVALID_SOCKET)
	{
		printf("Creating Socket error!");
		return;
	}

	struct sockaddr_in ServerAddr;
	memset(&ServerAddr,0,sizeof(ServerAddr)); 
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr= inet_addr("192.168.43.68");
    ServerAddr.sin_port = htons( 5000);

	if (bind(ServerSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("Binding error！");
		closesocket(ServerSocket); 
		WSACleanup();
		return;
	}
	if (listen(ServerSocket, 5) == SOCKET_ERROR)
	{
		printf("Listening error！");
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}
	else
	{
		printf("Server start...\n");
	}
	while (1)
	{
		SOCKET SocketClient; 
		struct sockaddr_in from; 
		int len = sizeof(from);
		SocketClient = accept(ServerSocket, (struct sockaddr*)&from, &len); 
		if (SocketClient != INVALID_SOCKET)
		{
			printf("远程主机：%s 通过端口：%d 连接到了服务器...\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
			char buf[1024], sendData[100] = "Welcome";
			len = strlen(sendData);
			sendData[len] = '\0';
			if (send(SocketClient, sendData, strlen(sendData) + 1, 0) != SOCKET_ERROR)
			{
				printf("send welcone to client successful \n");
			}
			if (recv(SocketClient, buf, sizeof(buf), 0) > 0)
			{
				printf("recieve data from client:%s\n", buf);
			}
			if (closesocket(SocketClient) != SOCKET_ERROR)
			{
				printf("close client Socket successful\n\n");
			}

		}
	}
}