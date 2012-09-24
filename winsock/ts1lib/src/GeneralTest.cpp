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
	
	// 构造测试项目请求报文
	// 发送报文
}

void GeneralTestItem::Stop()
{
	// 构造停止测试项目报文
}

