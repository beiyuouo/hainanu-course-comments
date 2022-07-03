#include <winsock2.h>	
#include <stdio.h>
#include <windows.h>
#pragma comment(lib,"WS2_32.lib")
#define BUF_SIZE 256
#define NAME_SIZE 30

DWORD WINAPI send_msg(LPVOID lpParam);
DWORD WINAPI recv_msg(LPVOID lpParam);
void error_handling(const char* msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main()
{
	HANDLE hThread[2];
	DWORD dwThreadId;
	// 初始化WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);

	/*设置登录用户名*/

	printf("Input your Chat Name:");
	scanf("%s", name);
	getchar();	
	// 创建套节字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		error_handling("Failed socket()");

	// 填写远程地址信息
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8888);
	// 如果你的计算机没有联网，直接使用本地地址127.0.0.1
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		error_handling("Failed connect()");
	printf("connect success\n");
	hThread[0] = CreateThread(
		NULL,		// 默认安全属性
		NULL,		// 默认堆栈大小
		send_msg,	// 线程入口地址（执行线程的函数）
		&sock,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号
	hThread[1] = CreateThread(
		NULL,		// 默认安全属性
		NULL,		// 默认堆栈大小
		recv_msg,	// 线程入口地址（执行线程的函数）
		&sock,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号

	// 等待线程运行结束
	WaitForMultipleObjects(2, hThread, true, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	printf(" Thread Over,Enter anything to over.\n");
	getchar();
	// 关闭套节字
	closesocket(sock);
	// 释放WS2_32库
	WSACleanup();
	return 0;
}

DWORD WINAPI send_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		sprintf(name_msg, "[%s]: %s", name, msg);
		int nRecv = send(sock, name_msg, strlen(name_msg), 0);
	}
	return NULL;
}

DWORD WINAPI recv_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;
	while (1)
	{
		str_len = recv(sock, name_msg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (str_len == -1)
			return -1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

void error_handling(const char* msg)
{
	printf("%s\n", msg);
	WSACleanup();
	exit(1);
}