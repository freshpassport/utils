#include "Device.h"

/*
 * 单纯创建对象的方式
 */
Device::Device()
{
	Name = "";
	ProductName = "";
	CPU_UID = "";
}

/*
 * 先创建连接的方式，通常用在自动发现功能
 */
Device::Device(NetConn *Conn)
{
	Connection = Conn;

	// 获取设备信息
	GetDevInfo();

	// 记录设备信息
	UpdateConfDatabase();
}

Device::Device(string IPv4Addr)
{
	Connection = new NetConn(IPv4Addr, DEFAULT_CONF_PORT);
	if (Connection!=NULL)
		GetDevInfo();
	UpdateConfDatabase();
}

Device::Device(DeviceStruct devinfo)
{
	DevInfo = devinfo;
}

Device::~Device()
{
	// 更新设备信息到配置库
	UpdateConfDatabase();
	// 通知定时器
	// 断开连接
	delete Connection;
}

void Device::LanuchTest(list<TestItem> *TestItem)
{
}

//-----------------------------------------------------------------

/* 获取设备信息 */
void Device::GetDevInfo()
{
}

void Device::UpdateConfDataBase()
{
}
