#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <mysql.h>

class MySqlHandler
{
public:
	MySqlHandler();
	~MySqlHandler();

	bool connectDb();
	bool queryDb();

private:
	MYSQL* mysql;//mysql连接
	MYSQL_FIELD* fd;//字段列数组
	char field[32][32];//存字段名二维数组
	MYSQL_RES* res;//代表返回行的一个查询结果集
	MYSQL_ROW column;//一个行数据的类型安全表示，表示数据行的列
	char query[150];//查询语句
};

