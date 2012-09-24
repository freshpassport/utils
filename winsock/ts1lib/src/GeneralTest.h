#include "TestItem.h"

namespace TestClient
{
	class GenerelTestItem
	{
		public:
			GeneralTestItem(int TestItemID, string Name, string Description);

		public:
			void Start(NetConn *conn);
			void Stop();
	};
}

