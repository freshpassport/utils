#include "Device.h"

/*
 * ������������ķ�ʽ
 */
Device::Device()
{
	Name = "";
	ProductName = "";
	CPU_UID = "";
}

/*
 * �ȴ������ӵķ�ʽ��ͨ�������Զ����ֹ���
 */
Device::Device(NetConn *Conn)
{
	Connection = Conn;

	// ��ȡ�豸��Ϣ
	GetDevInfo();

	// ��¼�豸��Ϣ
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
	// �����豸��Ϣ�����ÿ�
	UpdateConfDatabase();
	// ֪ͨ��ʱ��
	// �Ͽ�����
	delete Connection;
}

void Device::LanuchTest(list<TestItem> *TestItem)
{
}

//-----------------------------------------------------------------

/* ��ȡ�豸��Ϣ */
void Device::GetDevInfo()
{
}

void Device::UpdateConfDataBase()
{
}
