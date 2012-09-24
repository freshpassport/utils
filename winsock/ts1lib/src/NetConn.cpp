#include "NetConn.h"

#pragma comment(lib, "ws2_32.lib")

using namespace TestClient;

NetConn::NetConn(string addr, short port)
{
	m_IPv4Addr = addr;
	m_TcpPort = port;
	m_Connected = false;
	Connect();
}

bool NetConn::Connect()
{
	WSADATA wsaData;
	SOCKADDR_IN ServerAddr;

	if (m_IPv4Addr=="")
		return false;
	if (m_TcpPort<1)
		return false;

	WSAStartup(MAKEWORD(2,0), &wsaData);

	m_ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&ServerAddr, 0, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(m_TcpPort);
	ServerAddr.sin_addr.s_addr = inet_addr(m_IPv4Addr.c_str());


}

void NetConn::Disconnect()
{
}

bool NetConn::isConnect()
{
	return m_Connected;
}

int NetConn::Send(char &data)
{
	return 0;
}

int NetConn::Recv(char &data)
{
	return 0;
}

