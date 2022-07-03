#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> 
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI ThreadProc(LPVOID lpParameter)		//次线程，发消息
{
	//创建socket2
	SOCKET s2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//创建套接口
	sockaddr_in tcpaddr2;
	tcpaddr2.sin_family = AF_INET;
	tcpaddr2.sin_port = htons(5050);  //发送端口5050
	tcpaddr2.sin_addr.S_un.S_addr = inet_addr("10.210.225.191");
	int len = sizeof(SOCKADDR);

	while (1)
	{
		printf("我：\n    ");
		char sendBuf[100] = "\0";
		scanf_s("%s", sendBuf);
		sendto(s2, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&tcpaddr2, len);
		Sleep(1000);
	}

	closesocket(s2);
	return 0;
}

int main(int argc, char* argv[])
{

	WORD wVersionRequested;
	WSADATA wsaData;

	int iSockErr, iSockErr2;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("Winsock 初始化错误！\n");//Winsock初始化错误
		system("pause");
	}
	if (wsaData.wVersion != wVersionRequested)
	{
		printf("Winsock 版本不匹配！\n");//Winsock版本不匹配
		WSACleanup();
		system("pause");
	}

	//创建socket
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//创建套接口
	sockaddr_in tcpaddr;
	tcpaddr.sin_family = AF_INET;
	tcpaddr.sin_port = htons(5050);  //接收端口5050
	tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接口
	if (bind(s, (LPSOCKADDR)&tcpaddr, sizeof(tcpaddr)) == SOCKET_ERROR)
	{
		iSockErr = WSAGetLastError();
		printf("%d", iSockErr);//根据不同的错误类型进行不同的处理
		system("pause");
	}
	//进入多线程，发信息
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	CloseHandle(hThread);

	char recvBuf[50] = "\0";
	int len = sizeof(SOCKADDR);
	sockaddr_in addrClient;
	while (1)
	{
		recvfrom(s, recvBuf, 50, 0, (SOCKADDR*)&addrClient, &len);
		printf("他：\n    %s\n", recvBuf);
		Sleep(1000);
	}
	closesocket(s);
	WSACleanup();
	return 0;
}