
#ifndef __TEST_ITEM_H__
#define __TEST_ITEM_H__

namespace TestClient{

	union TestType
	{
		DEFAULT_TYPE
	};

	struct TestItemState
	{
		int a,b;
	};

	class TestItem
	{
		public:
			int TestID;
			string Name;
			string Description;

		private:
			TestType Type;
			NetConn *Connection;

		public:
			virtual void Start(NetConn &conn) = 0;
			virtual void Stop() = 0;
			void GetStatus(TestItemState &tiStatus);
	};

};
#endif/*__TEST_ITEM_H__*/

