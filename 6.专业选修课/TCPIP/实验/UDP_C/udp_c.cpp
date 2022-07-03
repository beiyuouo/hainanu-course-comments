#include "Winsock2.h""
#include "stdio.h"
#include "string.h"
#pragma comment(lib, "wsock32.lib")

#define RECV_PORT 3001
#define SEND_PORT 3000

SOCKET sock;
sockaddr_in ServerAddr;
sockaddr_in ClientAddr;

void main()
{
	int n;
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		printf("套接字初始化失败!\n");
		return;
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)
	{
		printf("套接字创建成功!\n");
		WSACleanup();
		return;
	}

	memset(&ClientAddr, 0, sizeof(ClientAddr));
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = inet_addr("10.210.225.191");
	ClientAddr.sin_port = htons(RECV_PORT);

	n = bind(sock, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr));
	if (n == SOCKET_ERROR)
	{
		printf("绑定失败!\n");
		return;
	}
	else
	{
		printf("绑定成功!\n");
	}

	char buf[1024] = "\0";
	int len;
	printf("开始接受数据...\n");
	while (1)
	{
		memset(&buf, '\0', sizeof(buf));
		memset(&ServerAddr, 0, sizeof(ServerAddr)); 
		len = sizeof(ServerAddr);
		

		n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&ServerAddr, (int *)&len);

		
		if (n == SOCKET_ERROR)
		{
			printf("\n 获取广播数据失败! \n'");
			printf("\n 错误代码: %d\n", WSAGetLastError());
		}
		
		else
		{
			printf("\n 获取广播数据: \" % s\" 来自 %s \n", buf, inet_ntoa(ServerAddr.sin_addr));
			if (strcmp(buf, "/nane") >= 0)
			{
				ServerAddr.sin_family = AF_INET;
				ServerAddr.sin_addr.s_addr = inet_addr("10.210.225.191");
				ServerAddr.sin_port = htons(SEND_PORT);

				memset(&buf, '\0', sizeof(buf));
				strcpy(buf, "20181685310031 张迈");
				n = sendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));

				if (n == SOCKET_ERROR)
				{
					printf("发送失败!\n");
				}
				else if (n = 0)
				{
					printf("发送失败!\0");
				}
				else if (n != SOCKET_ERROR)
				{
					printf("发送成功!\n");
				}
			}
		}
		Sleep(1000);
	}
	getchar();
	closesocket(sock);
	WSACleanup();
}