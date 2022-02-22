#include "UdpChatService.h"

//构造
UdpChatService::UdpChatService()
{
	if (initService()) {
		//QMessageBox::information(NULL, "信息", "服务器成功开启", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
	else {
		QMessageBox::critical(NULL, "错误", "服务器打开失败", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
}

//析构
UdpChatService::~UdpChatService()
{
	closeService();
	delete iocpServer;
}

//打开服务
bool UdpChatService::initService() {
	iocpServer = new IocpServer();

	//绑定服务信号槽
	QObject::connect(iocpServer, &IocpServer::serviceHandler,
		this, &UdpChatService::serviceDispatcher);

	return (iocpServer->serverStart());
}

//关闭服务
void UdpChatService::closeService() {
	iocpServer->serverStop();
}

//服务分配处理
void UdpChatService::serviceDispatcher(PER_IO_CONTEXT1* pIoContext, char* buff) {
	SERVICE_TYPE type = (SERVICE_TYPE)(int)buff[0];
	qDebug() << inet_ntoa(pIoContext->remoteAddr.sin_addr) << "!!!" << endl;
	switch (type) {
	case SEND_RECORD:
		s_GetRecord(pIoContext, buff);
		break;
	case SEND_RECORDS:
		s_GetRecords(pIoContext, buff);
		break;
	case SEND_HEARTBEAT_ACK:
		s_GetHeartbeatACK(pIoContext, buff);
		break;
	case SEND_REGIST_ACK:
		s_GetRegistACK(pIoContext, buff);
		break;
	case SEND_SIGNIN_ACK:
		s_GetSigninACK(pIoContext, buff);
		break;
	}
}

//发送请求报文
void UdpChatService::s_PostRequest(char* addr, char* buffer) {
	iocpServer->SendDataTo(addr, buffer);
}

//获取一条聊天记录
void UdpChatService::s_GetRecord(PER_IO_CONTEXT1* pIoContext, char* buf) {
	qDebug() << "收到record" << endl;
	emit post_record_ack(buf);
}
//获取一堆聊天记录
void UdpChatService::s_GetRecords(PER_IO_CONTEXT1* pIoContext, char* buf) {
	qDebug() << "收到records" << endl;
	emit post_records_ack(buf);
}
//用户注册tested
void UdpChatService::s_GetRegistACK(PER_IO_CONTEXT1* pIoContext, char* buf) {
	qDebug() << "收到registACK" << endl;
	emit post_regist_ack(buf);
}
//检查密码***未测试
void UdpChatService::s_GetSigninACK(PER_IO_CONTEXT1* pIoContext, char* buf) {
	qDebug() << "收到signinACK" << endl;
	emit post_signin_ack(buf);
}
//获取心跳监测答复
void UdpChatService::s_GetHeartbeatACK(PER_IO_CONTEXT1* pIoContext, char* buf) {
	qDebug() << "收到heartbeatACK" << endl;
	emit post_heartbeat_ack(buf);
}