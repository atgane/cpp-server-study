#include "pch.h"
#include "CorePch.h"

#include <iostream>
#include <atomic>
#include <mutex>
#include <future>
#include <string>
#include <vector>
#include <thread>

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << "ErrorCode : " << errCode << endl;
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	// 1. 논블로킹 방식에서는 아래와 같은 방식을 적용한다. 
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0; 

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;
	
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);
	
	while(true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// Error
			break;
		}

		cout << "Client Connected!" << endl;

		// recv

		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				break;
			}
			else if (recvLen == 0)
				break;

			cout << "Recv Data Len = " << recvLen << endl;

			// send
			while (true)
			{
				if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
				{
					if (::WSAGetLastError() == WSAEWOULDBLOCK)
						continue;

					break;
				}
				
				cout << "Send Data Len = " << recvLen << endl;
				break;
			}
		}
	}

	::WSACleanup();
}