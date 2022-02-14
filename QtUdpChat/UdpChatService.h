#pragma once
#pragma execution_character_set("utf-8")
#include "iocpserver.h"
#include <winsock2.h>
#include <QMessageBox>
#include <map>
#include <utility>
using std::pair;
using std::map;
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

class UdpChatService : public QObject
{
	Q_OBJECT
public:
	UdpChatService();
	~UdpChatService();
	bool initService();
	void closeService();
	//发送请求报文
	void s_PostRequest(char* addr, char* buffer);

protected:
	static DWORD WINAPI _CheckHeartbeatThread(LPVOID lpParam);//心跳线程函数

public slots:
void serviceDispatcher(PER_IO_CONTEXT1* pIoContext, char* buf);

signals:
void post_regist_ack(char* buffer);

private:
	IocpServer* iocpServer;

	HANDLE* HeartbeatThreadHandle;

	//获取一条聊天记录
	void s_GetRecord(PER_IO_CONTEXT1* pIoContext, char* buf);
	//获取一堆聊天记录
	void s_GetRecords(PER_IO_CONTEXT1* pIoContext, char* buf);
	//用户注册tested
	void s_GetRegistACK(PER_IO_CONTEXT1* pIoContext, char* buf);
	//检查密码***未测试
	void s_GetSigninACK(PER_IO_CONTEXT1* pIoContext, char* buf);
	//获取心跳监测答复
	void s_GetHeartbeatACK(PER_IO_CONTEXT1* pIoContext, char* buf);
};

