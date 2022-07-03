#include <winsock2.h>	
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define MAX_CLNT 256
#define BUF_SIZE 100
#pragma comment(lib,"WS2_32.lib")

void error_handling(const char* msg);		/*错误处理函数*/
DWORD WINAPI ThreadProc(LPVOID lpParam);	/*线程执行函数*/
void send_msg(char* msg, int len);			/*消息发送函数*/
HANDLE g_hEvent;			/*事件内核对象*/
int clnt_cnt = 0;			//统计套接字
int clnt_socks[MAX_CLNT];	//管理套接字
HANDLE hThread[MAX_CLNT];	//管理线程

int main()
{
	// 初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);	//请求了一个2.2版本的socket

	// 创建一个自动重置的（auto-reset events），受信的（signaled）事件内核对象
	g_hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	// 创建套节字
	SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == INVALID_SOCKET)
		error_handling("Failed socket()");
	
	// 填充sockaddr_in结构
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);				//8888端口
	sin.sin_addr.S_un.S_addr = INADDR_ANY;	//本地地址 

	// 绑定这个套节字到一个本地地址
	if (bind(serv_sock, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		error_handling("Failed bind()");

	// 进入监听模式
	if (listen(serv_sock, MAX_CLNT) == SOCKET_ERROR)
		error_handling("Failed listen()");
	printf("Start listen:\n");
	// 循环接受客户的连接请求
	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);
	DWORD dwThreadId;	/*线程ID*/
	SOCKET clnt_sock;

	while (TRUE)
	{
		printf("等待新连接\n");
		// 接受一个新连接
		clnt_sock = accept(serv_sock, (SOCKADDR*)&remoteAddr, &nAddrLen);
		if (clnt_sock == INVALID_SOCKET)
		{
			printf("Failed accept()");
			continue;
		}
		/*等待内核事件对象状态受信*/
		WaitForSingleObject(g_hEvent, INFINITE);
		hThread[clnt_cnt] = CreateThread(
			NULL,		// 默认安全属性
			NULL,		// 默认堆栈大小
			ThreadProc,	// 线程入口地址（执行线程的函数）
			(void*)&clnt_sock,		// 传给函数的参数
			0,		// 指定线程立即运行
			&dwThreadId);	// 返回线程的ID号
		clnt_socks[clnt_cnt++] = clnt_sock;
		SetEvent(g_hEvent);				/*设置受信*/

		printf(" 接受到一个连接：%s 执行线程ID：%d\r\n", inet_ntoa(remoteAddr.sin_addr), dwThreadId);
	}
	WaitForMultipleObjects(clnt_cnt, hThread, true, INFINITE);
	for (int i = 0; i < clnt_cnt; i++)
	{
		CloseHandle(hThread[i]);
	}
	// 关闭监听套节字
	closesocket(serv_sock);
	// 释放WS2_32库
	WSACleanup();
	return 0;
}


void error_handling(const char* msg)
{
	printf("%s\n", msg);
	WSACleanup();
	exit(1);
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	int clnt_sock = *((int*)lpParam);
	int str_len = 0, i;
	char msg[BUF_SIZE];

	while ((str_len = recv(clnt_sock, msg, sizeof(msg), 0)) != -1)
	{
		send_msg(msg, str_len);
		printf("群发送成功\n");
	}
	printf("客户端退出:%d\n", GetCurrentThreadId());
	/*等待内核事件对象状态受信*/
	WaitForSingleObject(g_hEvent, INFINITE);
	for (i = 0; i < clnt_cnt; i++)
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i++ < clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	SetEvent(g_hEvent);		/*设置受信*/
	// 关闭同客户端的连接
	closesocket(clnt_sock);
	return NULL;
}

void send_msg(char* msg, int len)
{
	int i;
	/*等待内核事件对象状态受信*/
	WaitForSingleObject(g_hEvent, INFINITE);
	for (i = 0; i < clnt_cnt; i++)
		send(clnt_socks[i], msg, len, 0);
	SetEvent(g_hEvent);		/*设置受信*/
}
