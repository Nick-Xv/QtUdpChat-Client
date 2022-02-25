#include "UdpChatService.h"
#include "ChatRoom.h"
#include "QtUdpChat.h"

//构造
UdpChatService::UdpChatService(QtUdpChat* q, ChatRoom* c)
{
	void* test;
	test = reinterpret_cast<void*>();
	//传回调函数指针
	CALLBACKFun(&UdpChatService::serviceDispatcher);

	qtUdpChat = q;
	chatRoom = c;
	if (initService()) {
		cout << "服务打开成功" << endl;
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
	return (iocpServer->serverStart());
}

//关闭服务
void UdpChatService::closeService() {
	iocpServer->serverStop();
}

//服务分配处理
void UdpChatService::serviceDispatcher(char* buff) {
	SERVICE_TYPE type = (SERVICE_TYPE)(int)buff[0];
	switch (type) {
	case SEND_RECORD:
		s_GetRecord(buff);
		break;
	case SEND_RECORDS:
		s_GetRecords(buff);
		break;
	case SEND_HEARTBEAT_ACK:
		s_GetHeartbeatACK(buff);
		break;
	case SEND_REGIST_ACK:
		s_GetRegistACK(buff);
		break;
	case SEND_SIGNIN_ACK:
		s_GetSigninACK(buff);
		break;
	default:
		cout << "接收到未知数据" << endl;
	}
}

//发送请求报文
void UdpChatService::s_PostRequest(char* addr, vector<char*> args) {
	//根据收到的参数vector构造buffer
	char buffer[Config::buffer_size];
	memset(&buffer[0], 0, Config::buffer_size);

	vector<char*>::iterator iter;
	int ptr = 0;
	for (iter = args.begin(); iter != args.end(); iter++) {
		memcpy(&buffer[ptr], *iter, strlen(*iter));
		ptr += strlen(*iter) + 1;
	}

	iocpServer->SendDataTo(addr, buffer);
}

//收到一条聊天记录
void UdpChatService::s_GetRecord(char* buffer) {
	cout << "收到record" << endl;
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	cout << (int)test << endl;

	vector<char*> v;

	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		cout << "接收到错误数据!!!" << endl;
		break;
	case 1:
		//检测到有人发了一条消息
		//获取idrecords,roomid,content,userid,timestamp
		int i = 2;
		int beginPtr[6] = { 0 };
		int endPtr[6] = { 0 };
		int num = 0;
		beginPtr[num] = i;
		while (buffer[i] != 0 || buffer[i + 1] != 0) {
			if (buffer[i] == 0) {
				endPtr[num] = i;
				beginPtr[++num] = i + 1;
			}
			i++;
		}
		endPtr[num] = i;

		//数据不完整直接返回false
		if (beginPtr[1] == 0) {
			cout << "接收到的数据错误" << endl;
			return;
		}

		char* idrecordsChar = new char[endPtr[0] - beginPtr[0] + 1];
		char* roomidChar = new char[endPtr[1] - beginPtr[1] + 1];
		char* contentChar = new char[endPtr[2] - beginPtr[2] + 1];
		char* useridChar = new char[endPtr[3] - beginPtr[3] + 1];
		char* timestampChar = new char[endPtr[4] - beginPtr[4] + 1];
		char* usernameChar = new char[endPtr[5] - beginPtr[5] + 1];

		memset(idrecordsChar, 0, endPtr[0] - beginPtr[0] + 1);
		memset(roomidChar, 0, endPtr[1] - beginPtr[1] + 1);
		memset(contentChar, 0, endPtr[2] - beginPtr[2] + 1);
		memset(useridChar, 0, endPtr[3] - beginPtr[3] + 1);
		memset(timestampChar, 0, endPtr[4] - beginPtr[4] + 1);
		memset(usernameChar, 0, endPtr[5] - beginPtr[5] + 1);

		memcpy(idrecordsChar, &buffer[beginPtr[0]], endPtr[0] - beginPtr[0]);
		memcpy(roomidChar, &buffer[beginPtr[1]], endPtr[1] - beginPtr[1]);
		memcpy(contentChar, &buffer[beginPtr[2]], endPtr[2] - beginPtr[2]);
		memcpy(useridChar, &buffer[beginPtr[3]], endPtr[3] - beginPtr[3]);
		memcpy(timestampChar, &buffer[beginPtr[4]], endPtr[4] - beginPtr[4]);
		memcpy(usernameChar, &buffer[beginPtr[5]], endPtr[5] - beginPtr[5]);

		v.push_back(useridChar);
		v.push_back(contentChar);
		v.push_back(timestampChar);
		v.push_back(usernameChar);
		v.push_back(idrecordsChar);
		v.push_back(roomidChar);

		chatRoom->doPostrecordAck(v);

		v.clear();
		delete idrecordsChar;
		delete roomidChar;
		delete contentChar;
		delete useridChar;
		delete timestampChar;
		delete usernameChar;
		break;
	case 2:
		//数据库错误
		cout << "数据库插入错误" << endl;
		break;
	default:
		//未知返回值
		cout << "接收到未知的返回值" << endl;
		break;
	}
}
//收到一堆聊天记录
void UdpChatService::s_GetRecords(char* buffer) {
	vector<char*> v;

	cout << "收到records" << endl;
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	cout << (int)test << endl;

	char *idrecordsChar = nullptr, *roomidChar = nullptr,
		*contentChar = nullptr, *useridChar = nullptr,
		*timestampChar = nullptr, *usernameChar = nullptr;

	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		cout << "接收到错误数据!!!" << endl;
		break;
	case 1:
		//查询成功
		//获取idrecords,roomid,content,userid,timestamp,username
		int i = 2;
		int beginPtr[120] = { 0 };
		int endPtr[120] = { 0 };
		int num = 0;
		beginPtr[num] = i;
		while (buffer[i] != 0 || buffer[i + 1] != 0) {
			if (buffer[i] == 0) {
				endPtr[num] = i;
				beginPtr[++num] = i + 1;
			}
			i++;
		}
		endPtr[num] = i;

		//没有收到聊天记录具体信息
		if (beginPtr[1] == 0) {
			cout << "接收到空的聊天记录" << endl;
			return;
		}

		for (int i = 0; i <= num; i++) {
			int j = i % 6;
			switch (j) {
			case 0:
				if (idrecordsChar != nullptr) delete idrecordsChar;
				idrecordsChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(idrecordsChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(idrecordsChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 1:
				if (roomidChar != nullptr) delete roomidChar;
				roomidChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(roomidChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(roomidChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 2:
				if (contentChar != nullptr) delete contentChar;
				contentChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(contentChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(contentChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 3:
				if (useridChar != nullptr) delete useridChar;
				useridChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(useridChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(useridChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 4:
				if (timestampChar != nullptr) delete timestampChar;
				timestampChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(timestampChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(timestampChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 5:
				if (usernameChar != nullptr) delete usernameChar;
				usernameChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(usernameChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(usernameChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				
				v.push_back(useridChar);
				v.push_back(contentChar);
				v.push_back(timestampChar);
				v.push_back(usernameChar);
				v.push_back(idrecordsChar);
				v.push_back(roomidChar);

				chatRoom->doPostrecordsAck(v);

				v.clear();
				delete idrecordsChar;
				delete roomidChar;
				delete contentChar;
				delete useridChar;
				delete timestampChar;
				delete usernameChar;
				break;
			}
		}
		break;
	case 2:
		//数据库错误
		cout << "数据库查询错误" << endl;
		break;
	default:
		//未知返回值
		cout << "接收到未知的返回值" << endl;
		break;
	}
}
//收到用户注册ACK
void UdpChatService::s_GetRegistACK(char* buffer) {
	cout << "收到registACK" << endl;
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	cout << (int)test << endl;

	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		qtUdpChat->showSimpleMessageBox(0, "错误", "数据错误!");
		break;
	case 1:
		//用户名已存在
		qtUdpChat->showSimpleMessageBox(0, "错误", "用户名已存在!");
		break;
	case 2:
		//数据库错误
		qtUdpChat->showSimpleMessageBox(0, "错误", "数据库错误!");
		break;
	case 3:
		//注册成功
		qtUdpChat->showSimpleMessageBox(1, "信息", "注册成功!");
		//此处跳转到登录页面
		//
		//
		break;
	default:
		//未知返回值
		qtUdpChat->showSimpleMessageBox(0, "错误", "未知返回值!");
		break;
	}
	qtUdpChat->stopWaiting();
}
//收到检查密码ACK
void UdpChatService::s_GetSigninACK(char* buffer) {
	cout << "收到signinACK" << endl;
	
}
//收到心跳ACK
void UdpChatService::s_GetHeartbeatACK(char* buffer) {
	cout << "收到heartbeatACK" << endl;
	
}