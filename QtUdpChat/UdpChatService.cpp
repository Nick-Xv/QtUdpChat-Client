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

//心跳线程函数
//每10秒发送一次心跳报文
DWORD WINAPI UdpChatService::_CheckHeartbeatThread(LPVOID lpParam) {
	UdpChatService* pParam = static_cast<UdpChatService*>(lpParam);
	//每10秒发送一次
	while (1) {
		Sleep(10000);
		//发送SENDTO
		memset(buffer, 0, buffer_size);
		buffer[0] = POST_REGIST;

		unsigned short id_cur = 1, id_all = 1;
		memcpy(&buffer[1], &(id_cur), sizeof(id_cur));
		memcpy(&buffer[3], &(id_all), sizeof(id_all));
		QString usernameString = nameInput->text();
		QString passwordString = pwInput->text();
		QString passwordCheckString = pwCheckInput->text();
		if (usernameString.length() <= 1) {
			//用户名太短了
			QMessageBox::critical(NULL, "错误", "用户名太短了", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}

		if (passwordString != passwordCheckString) {
			QMessageBox::critical(NULL, "错误", "两个密码不一致", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		QByteArray temp1;
		temp1.append(usernameString);
		const char* username = temp1.data();
		QByteArray temp2;
		temp2.append(passwordString);
		const char* password = temp2.data();
		memcpy(&buffer[5], username, strlen(username));
		memcpy(&buffer[5 + strlen(username) + 1], password, strlen(password));

		//显示转圈
		waitMovie->start();
		waitLabel->setVisible(true);
		//发送请求
		udpChatService->s_PostRequest(addr, buffer);
		//
	}
	return 0;
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