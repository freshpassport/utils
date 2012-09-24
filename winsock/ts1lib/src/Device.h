#include <list>

#ifndef __DEVICE_H__
#define __DEVICE_H__

#define MAX_NIC 3

namespace TestClient{

/* 网口信息 */
struct NetAdapter
{
	unsigned int Ipv4Address;
	unsigned char MacAddress[6];
};

/* 系统内存规格定义 */
union SystemMemory
{
	MEM512M,
	MEM1G,
	MEM2G
}

/* 设备类型 */
union DeviceType
{
	G2,
	MS2000
};

struct DeviceStruct
{
	string Name;                    // 名称
	string ProductName;             // 设备名称
	string CPU_UID;                 // CPU-UID
	NetAdapter NetAds[MAX_NIC];     // 网口信息 0-管理口
	DeviceType Type;                // 设备类型
	SystemMemory MemSize;           // 内存大小
};

class Device
{
	public:
		DeviceStruct DevInfo;

	private:
		NetConn *Connection;
	
	public:
		Device();
		Device(NetConn *Conn);         // 先创建连接的方式
		Device(string IPv4Addr);       // 指定被测试设备地址的方式
		Device(DeviceStruct DevInfo);  // 从配置数据库创建
		~Device();
	
	public:
		void LanuchTest(list<TestItem> *TestItem);
	
	private:
		void GetDevInfo();               // 获取设备信息
		void UpdateConfDatabase();       // 更新设备信息到配置数据库
};

};
#endif/*__DEVICE_H__*/

