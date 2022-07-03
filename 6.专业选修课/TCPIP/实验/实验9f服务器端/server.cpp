#include <stdio.h>
#include <WinSock2.h> //windows socket的头文件
#include <Windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <process.h>
#pragma comment(lib, "ws2_32.lib") //连接winsock2.h的静态库文件
using namespace std;

//mutex 每个线程在对资源操作前都尝试先加锁，成功加锁才能操作，操作结束解锁。
//同一时刻，只能有一个线程持有该锁。
mutex m;

//定义结构体用来设置
typedef struct my_file
{
	SOCKET clientSocket; //文件内部包含了一个SOCKET 用于和客户端进行通信
	sockaddr_in clientAddr; //用于保存客户端的socket地址
	int id; //文件块的序号
}F;

DWORD WINAPI transmmit(const LPVOID arg)
{
	//实际上这里为了追求并发性不应该加锁，上锁是为了方便看输出
	m.lock();

	F* temp = (F*)arg;
	//获取文件的序号
	//int file_id = temp->id;
	//获取客户机的端口号
	//ntohs(temp -> clientAddr.sin_port); 
	cout << "测试开始,等待客户端发送消息..." << endl;
	//从客户端处接受数据
	char Buffer[MAXBYTE] = { 0 }; //缓冲区
	recv(temp->clientSocket, Buffer, MAXBYTE, 0); //recv方法 从客户端通过clientScocket接收
	cout << "线程" << temp->id << "从客户端的" << ntohs(temp->clientAddr.sin_port) << "号端口收到:" << Buffer << endl;

	//发送简单的字符串到客户端
	const char* s = "Server file";
	send(temp->clientSocket, s, strlen(s) * sizeof(char) + 1, NULL);
	cout << "线程" << temp->id << "通过客户端的" << ntohs(temp->clientAddr.sin_port) << "号端口发送:" << s << endl;

	m.unlock();

	return 0;
}

int main()
{
	//加载winsock库,第一个参数是winsocket load的版本号（2.3）
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 3), &wsaData);

	//创建服务器端的socket（协议族， sokcet类型）
	SOCKET servSocket = socket(AF_INET, SOCK_STREAM, 0);//如果改成SOCK_DGRAM则使用UDP

	// 初始化socket信息
	sockaddr_in servAddr; //服务器的socket地址，包含sin_addr表示IP地址，sin_port保持端口号和sin_zero填充字节
	memset(&servAddr, 0, sizeof(SOCKADDR)); //初始化socket地址

	//设置Socket的连接地址、方式和端口，并绑定
	servAddr.sin_family = PF_INET; //设置使用的协议族
	servAddr.sin_port = htons(2017); //设置使用的端口
	servAddr.sin_addr.s_addr = inet_addr("192.168.43.68"); //设置绑定的IP地址
	::bind(servSocket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)); //将之前创建的servSocket和端口，IP地址绑定

	HANDLE hThread[20]; //获取句柄
	listen(servSocket, 20); //监听服务器端口，最多20个连接
	for (int i = 0; i < 20; i++)
	{
		F* temp = new F; //创建新的传输结构体
		sockaddr_in clntAddr;
		int nSize = sizeof(SOCKADDR);
		SOCKET clientSock = accept(servSocket, (SOCKADDR*)&clntAddr, &nSize);
		//temp数据成员赋值
		temp->clientSocket = clientSock;
		temp->id = i + 1;
		temp->clientAddr = clntAddr;
		//通过句柄创建子线程
		hThread[i] = CreateThread(NULL, 0, &transmmit, temp, 0, NULL);
	}

	//等待子线程完成
	WaitForMultipleObjects(20, hThread, TRUE, INFINITE);
	cout << WSAGetLastError() << endl; //查看错误信息

	//关闭socket，释放winsock
	closesocket(servSocket);
	WSACleanup();

	cout << "服务器连接已关闭。" << endl;
	system("pause");

	return 0;
}
