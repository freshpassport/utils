#include <string>
#include <winsock2.h>

#ifndef __NET_CONN_H__
#define __NET_CONN_H__

using namespace std;

namespace TestClient
{
	class NetConn
	{
		private:
			string m_IPv4Addr;
			short m_TcpPort;
			SOCKET m_ClientSocket;
			bool m_Connected;

		public:
			NetConn(string addr, short port);
			~NetConn();

		public:
			bool Connect();
			void Disconnect();
			bool isConnect();
			int Send(char &data);
			int Recv(char &data);
	};
}
#endif/*__NET_CONN_H__*/

