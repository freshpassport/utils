#include "GeneralTestItem.h"

GeneralTestItem::GeneralTestItem(
		int TestItemID, string TestItemName, string TestItemDescription)
{
	TestID = TestItemID;
	Name = TestItemName;
	Description = TestItemDescription;
	Connection = NULL;
}

void GeneralTestItem::Start(NetConn *conn)
{
	Connection = conn;
	
	// ���������Ŀ������
	// ���ͱ���
}

void GeneralTestItem::Stop()
{
	// ����ֹͣ������Ŀ����
}

