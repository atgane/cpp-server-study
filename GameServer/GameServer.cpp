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

// 2. 클라가 서버에 접속하면 session이라는 구조체를 이용하여 관리한다. 
// 동접 숫자 만큼 세션이 생성된다. 
const int32 BUFSIZE = 1000;
struct Session
{
	SOCKET socket;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0; 
};

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

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

	// 1. Select모델: Select함수가 핵심이 된다. 
	// 클라이언트도 네트워크에 연동하는 경우, iocp를 써야할 필요가 없기 때문에
	// 이런 경우 iocp보다 select모델을 이용하는 경우가 있다. 
	// 이는 리눅스에도 사용할 수 있다. 
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 있다. 
	// 수신 버퍼에 데이터가 없는데 read를 한다거나 송신버퍼가 꽉 찼는데 write를 한다거나
	// 블로킹 논블로킹 모두에 응용핧 수 있다. 
	// 블로킹 상황의 경우 조건을 만족하지 않아 블로킹되는 상황을 예방한다. 
	// 논블로킹 상황의 경우 조건을 만족하지 않아 불필요하게 반복체크하는 상황을 예방한다. 

	// socket set
	// read / write / except 관찰 대상을 등록할 수 있다. 
	// select를 해서 관찰을 시작하고 적어도 하나의 소켓이 준비되면 리턴한다. 

	// FD_ZERO: 비움
	// FD_SET: 소켓 s를 넣는다. 
	// SD_CLR: 소켓 제거
	// SD_ISSET: 소켓 s가 set에 들어있으면 0이 아닌 값을 이용

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		// 3. 소켓 셋 초기화. select를 호출하고 반환할 때 reads와 writes의 일부가 날라가기에
		// 매번 루프를 돌며 초기화해주어야 한다. 
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// 4. listenSocket등록. accept를 할 대상이 있는가를 탐색한다. read쪽 set에 넣어줘야 한다. 
		FD_SET(listenSocket, &reads);

		for (Session& s : sessions)
		{
			// 5. recvBytes가 sendBytes보다 이하이면 read에 등록 아니면 write에 등록
			if (s.recvBytes <= s.sendBytes)
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}

		// 마지막 timeout인자 설정 가능. 무한대기하지 않음 
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;

		// 6. 이후 남은 것은 준비가 된 소켓들
		// listener 소켓 체크. true이면 accept를 할 준비가 됐다는 것. 클라가 connect를 했다는 것을 의미한다. 
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			
			// 7. 클라이언트 소켓이 정상적이라면 세션에 등록시킨다. 
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "client connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		// 8. 나머지 소켓 체크
		for (Session& s : sessions)
		{
			//read 체크
			if (FD_ISSET(s.socket, &reads))
			{
				// 실질적으로 받은 데이터 크기 만큼을 recvLen으로 받는다. 
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					// TODO: session 제거
					continue;
				}

				s.recvBytes = recvLen;
			}

			//write
			if (FD_ISSET(s.socket, &writes))
			{
				// 데이터를 쓸 준비가 됐다!
				// send를 호출할 때 send를 한 크기를 리턴한다. 
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// TODO: session 제거
					continue;
				}

				s.recvBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}

	::WSACleanup();
}