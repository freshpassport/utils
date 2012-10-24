#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * 函数回调原型：
 * typedef int (*sqlite3_callback)(void*, int, char**, char**);
 */

// 每查询到一条记录，sqlite3会调用一次此函数
int LoadInfo(void *para, int n_column, char **column_value, char **column_name)
{
	int i;

	printf("记录包含字段个数：%d\n\n", n_column);
	for (i=0; i<n_column; i++)
		printf("字段名: %s\t字段值: %s\n\n", column_name[i], column_value[i]);
	printf("-------------------------------------------\n");
	return 0;
}

// 创建一个供测试的数据库
int db_create()
{
	sqlite3 *db;
	int result;
	char *errmsg;

	result = sqlite3_open("temp.db", &db);
	result = sqlite3_exec(db, "CREATE TABLE MyTable(ID integer primary key autoincrement,name nvarchar(32))", NULL, NULL, &errmsg);
	if (result)
		printf("err: %s\n", errmsg);

	result = sqlite3_exec(db, "INSERT INTO MyTable(name) VALUES('张三')", NULL, NULL, &errmsg);
	if (result)
		printf("err: %s\n", errmsg);
	result = sqlite3_exec(db, "INSERT INTO MyTable(name) VALUES('里斯')", NULL, NULL, &errmsg);
	result = sqlite3_exec(db, "INSERT INTO MyTable(name) VALUES('哇哇哇')", NULL, NULL, &errmsg);
	return sqlite3_close(db);
}

int db_write1s(int times)
{
	sqlite3 *db;
	int loop, result;
	char *errmsg;
	time_t t;
	char sql_cmd[128];

	result = sqlite3_open("temp.db", &db);

	printf("times = %d\n", times);

	loop = 0;
	do {
		if (times!=0)
			loop++;
		t = time(NULL);
		sprintf(sql_cmd, "INSERT INTO MyTable(name) VALUES('%s --- %d')", asctime(localtime(&t)), (int)getpid());
		sqlite3_exec(db, sql_cmd, NULL, NULL, &errmsg);
		sleep(1);
	} while(loop<times);

	return sqlite3_close(db);
}

int main(int argc, char *argv[])
{
	sqlite3 *db;
	int result;
	char *errmsg = NULL;

	result = sqlite3_open("temp.db", &db);

	printf("argc = %d\n", argc);

	if ( (argc>=2) && (!strcmp(argv[1], "create")) )
		return db_create();
	if ( (argc>=3) && (!strcmp(argv[1], "write")) )
		return db_write1s(atoi(argv[2]));
	if ( (argc>=2) && (!strcmp(argv[1], "write")) )
		return db_write1s(0);

	result = sqlite3_exec(db, "SELECT * FROM MyTable", LoadInfo, NULL, &errmsg);

	sqlite3_close(db);

	return 0;
}
