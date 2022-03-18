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
#ifndef _DEBUGPRINTF_H_    
#define _DEBUGPRINTF_H_ 
#endif
#include <tchar.h>
#define DP(fmt) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt));OutputDebugString(sOut);}
#define DP1(fmt,var) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var);OutputDebugString(sOut);}
#define DP2(fmt,var1,var2) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2);OutputDebugString(sOut);}    
#define DP3(fmt,var1,var2,var3) {TCHAR sOut[256];_stprintf_s(sOut,_T(fmt),var1,var2,var3);OutputDebugString(sOut);}

#include "iocpserver.h"
#include <winsock2.h>
#include <map>
#include <utility>
#include <iostream>
#include "Config.h"
#include <functional>
using namespace std::placeholders;
using std::pair;
using std::map;
using std::endl;

//Fun函数模板，接受char*参数，返回void
typedef std::function<void(char*)> Fun;
//Emiter函数模板，用于调用GUI发送信号
typedef std::function<void(int, char*)> Emiter;
//ArgsEmiter函数模板，用于调用带参数GUI发送信号
typedef std::function<void(int, vector<char*>*)> ArgsEmiter;

//ChatEmiter函数模板，用于Chat界面GUI发送信号
typedef std::function<void(int, char*)> ChatEmiter;
//ChatArgsEmiter函数模板，用于Chat界面带vector参数GUI信号
typedef std::function<void(int, vector<char*>* args)> ChatArgsEmiter;

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
	UdpChatService(QtUdpChat* q, ChatRoom* c, Emiter e, ArgsEmiter ae, ChatEmiter ce, ChatArgsEmiter cae);
	~UdpChatService();
	bool initService();
	void closeService();
	//发送请求报文
	void s_PostRequest(char* addr, vector<char*> args);
	void serviceDispatcher(char* buf);
	Fun fun;
	Emiter emiter;
	ArgsEmiter argsEmiter;
	ChatEmiter chatEmiter;
	ChatArgsEmiter chatArgsEmiter;

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

