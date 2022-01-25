#include "MySqlHandler.h"



MySqlHandler::MySqlHandler()
{
}


MySqlHandler::~MySqlHandler()
{
}

bool MySqlHandler::connectDb() {
	mysql_init(mysql);
}