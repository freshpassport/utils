#include <list>

#ifndef __DEVICE_H__
#define __DEVICE_H__

#define MAX_NIC 3

namespace TestClient{

/* ������Ϣ */
struct NetAdapter
{
	unsigned int Ipv4Address;
	unsigned char MacAddress[6];
};

/* ϵͳ�ڴ����� */
union SystemMemory
{
	MEM512M,
	MEM1G,
	MEM2G
}

/* �豸���� */
union DeviceType
{
	G2,
	MS2000
};

struct DeviceStruct
{
	string Name;                    // ����
	string ProductName;             // �豸����
	string CPU_UID;                 // CPU-UID
	NetAdapter NetAds[MAX_NIC];     // ������Ϣ 0-�����
	DeviceType Type;                // �豸����
	SystemMemory MemSize;           // �ڴ��С
};

class Device
{
	public:
		DeviceStruct DevInfo;

	private:
		NetConn *Connection;
	
	public:
		Device();
		Device(NetConn *Conn);         // �ȴ������ӵķ�ʽ
		Device(string IPv4Addr);       // ָ���������豸��ַ�ķ�ʽ
		Device(DeviceStruct DevInfo);  // ���������ݿⴴ��
		~Device();
	
	public:
		void LanuchTest(list<TestItem> *TestItem);
	
	private:
		void GetDevInfo();               // ��ȡ�豸��Ϣ
		void UpdateConfDatabase();       // �����豸��Ϣ���������ݿ�
};

};
#endif/*__DEVICE_H__*/

