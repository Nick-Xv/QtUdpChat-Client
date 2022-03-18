#include "UdpChatService.h"

//构造
UdpChatService::UdpChatService(QtUdpChat* q, ChatRoom* c, Emiter e, ArgsEmiter ae, ChatEmiter ce, ChatArgsEmiter cae)
{
	qtUdpChat = q;
	chatRoom = c;
	emiter = e;
	argsEmiter = ae;
	chatEmiter = ce;
	chatArgsEmiter = cae;
	if (initService()) {
		DP("服务打开成功\n");
	}
	else {
		emiter(0, "服务器打开失败");
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
	fun = std::bind(&UdpChatService::serviceDispatcher, this, _1);
	iocpServer = new IocpServer(fun);
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
		DP("接收到未知数据\n");
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
	DP("收到record\n");
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	DP1("%d\n",(int)test);

	vector<char*> v;

	int i = 2;
	int beginPtr[6] = { 0 };
	int endPtr[6] = { 0 };
	int num = 0;
	char* idrecordsChar = nullptr;
	char* roomidChar = nullptr;
	char* contentChar = nullptr;
	char* useridChar = nullptr;
	char* timestampChar = nullptr;
	char* usernameChar = nullptr;
	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		DP("接收到错误数据!!!\n");
		break;
	case 1:
		//检测到有人发了一条消息
		//获取idrecords,roomid,content,userid,timestamp
		
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
			DP("接收到的数据错误\n");
			return;
		}

		idrecordsChar = new char[endPtr[0] - beginPtr[0] + 1];
		roomidChar = new char[endPtr[1] - beginPtr[1] + 1];
		contentChar = new char[endPtr[2] - beginPtr[2] + 1];
		useridChar = new char[endPtr[3] - beginPtr[3] + 1];
		timestampChar = new char[endPtr[4] - beginPtr[4] + 1];
		usernameChar = new char[endPtr[5] - beginPtr[5] + 1];

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

		chatArgsEmiter(0, &v);

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
		DP("数据库插入错误\n");
		break;
	default:
		//未知返回值
		DP("接收到未知的返回值\n");
		break;
	}
}

//收到一堆聊天记录
void UdpChatService::s_GetRecords(char* buffer) {
	vector<char*> v;

	DP("收到records\n");
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	DP1("%d\n",(int)test);

	char *idrecordsChar = nullptr, *roomidChar = nullptr,
		*contentChar = nullptr, *useridChar = nullptr,
		*timestampChar = nullptr, *usernameChar = nullptr;

	int i = 2;
	int beginPtr[120] = { 0 };
	int endPtr[120] = { 0 };
	int num = 0;

	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		DP("接收到错误数据!!!\n");
		break;
	case 1:
		//查询成功
		//获取idrecords,roomid,content,userid,timestamp,username
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
			DP("接收到空的聊天记录\n");
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
				chatArgsEmiter(1, &v);
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
		DP("数据库查询错误\n");
		break;
	default:
		//未知返回值
		DP("接收到未知的返回值\n");
		break;
	}
}

//收到用户注册ACK
void UdpChatService::s_GetRegistACK(char* buffer) {
	DP("收到registACK\n");
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	DP1("%d",(int)test);

	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		emiter(0, "数据错误!");
		break;
	case 1:
		//用户名已存在
		emiter(0, "用户名已存在!");
		break;
	case 2:
		//数据库错误
		emiter(0, "数据库错误!");
		break;
	case 3:
		//注册成功
		emiter(1, "注册成功!");
		//此处跳转到登录页面
		emiter(3, nullptr);
		break;
	default:
		//未知返回值
		emiter(0, "未知返回值!");
		break;
	}
	emiter(2, nullptr);
}

//收到检查密码ACK
void UdpChatService::s_GetSigninACK(char* buffer) {
	DP("收到signinACK\n");
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	DP1("%d\n", (int)test);

	char* roomID = nullptr;
	int userid = 0;

	//获取userid
	int i = 2;
	int beginPtr = 0;
	int endPtr = 0;
	int num = 0;
	beginPtr = i;
	while (buffer[i] != 0 || buffer[i + 1] != 0) {
		if (buffer[i] == 0) {
			endPtr = i;
		}
		i++;
	}
	char* useridString = new char[endPtr - beginPtr + 1];
	memset(useridString, 0, endPtr - beginPtr + 1);
	memcpy(useridString, &buffer[beginPtr], endPtr - beginPtr);
	//寻找替代函数
	userid = atoi(useridString);
	vector<char*> args;
	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		emiter(0, "数据错误!");
		break;
	case 1:
		//密码正确，首先检查是否有房间号
		//这里把要用的数据传过去
		args.clear();
		args.push_back(useridString);
		//DP1("%s\n", &args);
		argsEmiter(0, &args);
		break;
	case 2:
		//数据库错误
		emiter(0, "数据库错误!");
		break;
	case 3:
		//密码错误
		emiter(0, "密码错误!");
		break;
	default:
		//未知返回值
		emiter(0, "未知返回值!");
		break;
	}
	//停止转圈
	emiter(2, nullptr);
}
//收到心跳ACK
void UdpChatService::s_GetHeartbeatACK(char* buffer) {
	DP("收到heartbeatACK\n");
	//暂时没什么功能
}