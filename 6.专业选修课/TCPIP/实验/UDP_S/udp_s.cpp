#include "Winsock2.h""
#include "stdio.h"
#include "string.h"'
#pragma comment(lib, "wsock32.lib")

#define RECV_PORT 3000
#define SEND_PORT 3001

SOCKET sock;
SOCKET ServerSock;
sockaddr_in ServerAddr; 
sockaddr_in broadAddr;

void main()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("套接字初始化失败!\n"); 
		return;
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)
	{
		printf("套接字创建失败!\n");
		WSACleanup();
		return;
	}
	
	ServerSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ServerSock == SOCKET_ERROR)
	{
		printf("套接字创建失败!\n");
		WSACleanup();
		return;
	}

	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr("10.210.225.191");
	ServerAddr.sin_port = htons(RECV_PORT);

	memset(&broadAddr,0,sizeof(broadAddr));
	broadAddr.sin_family = AF_INET;
	broadAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	printf("广播地址: %s \n", inet_ntoa(broadAddr.sin_addr));
	broadAddr.sin_port = htons(SEND_PORT);

	bool bBroadcast = true;
	int n = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&bBroadcast, sizeof(bBroadcast));

	int opt = sizeof(bBroadcast);
	getsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, &opt);

	if (bBroadcast == true)
	{
		printf("\n测试是否可以广播: 是\n");
	}
	else
	{
		printf("\n测试是否可以广播: 否 \n");
		return;
	}

	char buf[1024] = "/name:开始点名！\0";
	n = sendto(sock, buf, strlen(buf) + 1, 0, (const sockaddr*)&broadAddr, sizeof(broadAddr));

	if (n == SOCKET_ERROR)
	{
		printf("广播数据错误!\n");
		printf("错误代码:%d\n", WSAGetLastError());
	}
	else
	{
		printf("\n发送广播数据:\"%s\" 成功! \n\n", buf);
	}
	if (bind(ServerSock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("绑定失败!\n");
		printf("错误代码:%d\n", WSAGetLastError());
		closesocket(ServerSock);
		getchar();
		return;
	}
	else
	{
		printf("绑定成功！开始接受来自客户端的数据...\n");
	}

	while (1)
	{
		sockaddr_in ClientAddr;
		memset(&buf, '\0', sizeof(buf));
		memset(&ClientAddr, 0, sizeof(ClientAddr));
		n = sizeof(ClientAddr);
		n = recvfrom(ServerSock, buf, sizeof(buf), 0, (struct sockaddr*)&ClientAddr, &n);

		if (n != SOCKET_ERROR)
		{
			printf("\n 接受远程主机 %s 传回的数据：%s \n", inet_ntoa(ClientAddr.sin_addr), buf);
		}
	}

	closesocket(sock);
	closesocket(ServerSock);
	WSACleanup();
}