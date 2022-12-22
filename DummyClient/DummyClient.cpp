#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	// 1. 소켓 라이브러리 초기화
	// 관련 정보가 wsaData에 채워진다. 
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 0;
	}

	// 2. 소켓 열기
	// AF_INET: IPv4, AF_INET6: IPv6
	// SOCK_STREAM: tcp, SOCK_DGRAM: udp
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket Error code: " << errCode << endl;
		return 0;
	}

	// 3. 주소만들기
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;

	// 주석 코드보다 아래 코드가 IP를 부여하는 더 현대적인 방법
	// serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	// 127.0.0.1은 loopback주소이다. 
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	
	// h: host, n: network. 호스트의 형식에서 네트워크 형식으로 포트 번호를 맞춘다는 의미
	// 엔디안을 네트워크 기반으로 맞춘다! 
	serverAddr.sin_port = ::htons(7777);

	// 4. 소켓 연결
	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Connect Error code: " << errCode << endl;
		return 0;
	}

	// 5. 연결 성공. 데이터 송수신 가능
	cout << "Connected Server!" << endl;

	while (true)
	{
		this_thread::sleep_for(1s);
	}

	// 6. 소켓 리소스 반환
	closesocket(clientSocket);

	// 7. 접속 종료
	::WSACleanup();
}