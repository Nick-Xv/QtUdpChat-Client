#pragma once
#pragma execution_character_set("utf-8")
/*
UdpChatService
服务中间层，处理数据
1.接收来自IocpServer的信号（包含缓冲区指针）
2.分析缓冲区第一位确定服务类型
3.按照服务类型进行数据解析和相应功能的处理
4.向UI发送信号（包含解析后的数据）

1.接收来自UI类的信号
2.分析信号，处理后给IocpServer发信号
*/
typedef void(UdpChatService::*pFunc) (char* pval);
int __stdcall CALLBACKFun(pFunc pFun);

#include "iocpserver.h"
#include <winsock2.h>
#include <map>
#include <utility>
#include <iostream>
#include "Config.h"
using std::pair;
using std::map;
using std::cout;
using std::endl;
enum SERVICE_TYPE {
	NO_SERVICE,
	//GET
	GET_PASSWORD,
	GET_RECORD,
	//POST
	POST_REGIST,
	POST_RECORD,
	//CHECK
	CHECK_PASSWORD,
	CHECK_HEARTBEAT,
	//SEND
	SEND_RECORD,
	SEND_RECORDS,
	SEND_HEARTBEAT_ACK,
	SEND_REGIST_ACK,
	SEND_SIGNIN_ACK
};

extern class QtUdpChat;
extern class ChatRoom;

class UdpChatService
{
public:
	UdpChatService(QtUdpChat* q, ChatRoom* c); 
	~UdpChatService();
	bool initService();
	void closeService();
	//发送请求报文
	void s_PostRequest(char* addr, vector<char*> args);
	void serviceDispatcher(char* buf);

private:
	IocpServer* iocpServer;

	QtUdpChat* qtUdpChat;
	ChatRoom* chatRoom;

	//获取一条聊天记录
	void s_GetRecord(char* buf);
	//获取一堆聊天记录
	void s_GetRecords(char* buf);
	//用户注册
	void s_GetRegistACK(char* buf);
	//检查密码
	void s_GetSigninACK(char* buf);
	//获取心跳监测答复
	void s_GetHeartbeatACK(char* buf);
};

