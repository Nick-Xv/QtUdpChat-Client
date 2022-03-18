#include "QtUdpChat.h"

//释放指针宏
#define RELEASE(x) {if((x)!=nullptr){delete(x);(x)=nullptr;}}
//释放句柄宏
#define RELEASE_HANDLE(x) {if((x)!=nullptr&&(x)!=INVALID_HANDLE_VALUE){CloseHandle(x);(x)=nullptr;}}

int QtUdpChat::roomid = 0;
int QtUdpChat::userid = 0;
char* QtUdpChat::hbBuffer = new char[Config::buffer_size];
char* QtUdpChat::addr = new char[16];
QString QtUdpChat::addrQString = "";
UdpChatService* QtUdpChat::udpChatService = nullptr;

QtUdpChat::QtUdpChat(QWidget *parent)
    : QWidget(parent)
{
	//注册QVariant
	qRegisterMetaType<QVariant>("QVariant");

	//心跳句柄
	HeartbeatThreadHandle = new HANDLE;
	*HeartbeatThreadHandle = INVALID_HANDLE_VALUE;

	this->resize(Config::screenWidth / 3, Config::screenHeight / 2);
	//去除标题栏
	//窗口最小化时，点击任务栏窗口可以显示出原窗口
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);
	//setAttribute(Qt::WA_DeleteOnClose);
	
	//软件图片
	img = new QImage(":/test/resources/chat_m.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(Config::screenWidth / Config::img_ratio, Config::screenWidth / Config::img_ratio, Qt::KeepAspectRatio);
	imgLabel = new QLabel(this);
	imgLabel->setPixmap(QPixmap::fromImage(*imgscaled));
	imgLabel->setObjectName("ImgLabel");

	//房间号标签
	roomLabel = new QLabel(this);
	roomLabel->setText("输入房间号");
	roomLabel->setObjectName("RoomLabel");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;");
	roomLabel->setStyleSheet(styleSheetTemp);

	//房间号输入栏
	roomInput = new QLineEdit(this);
	roomInput->setObjectName("RoomInput");
	roomInput->setPlaceholderText("房间号");
	roomInput->setStyleSheet(styleSheetTemp);

	//用户名标签
	nameHint = new QLabel(this);
	nameHint->setText("输入用户名");
	nameHint->setObjectName("NameHint");
	nameHint->setStyleSheet(styleSheetTemp);

	//用户名输入栏
	nameInput = new QLineEdit(this);
	nameInput->setPlaceholderText("Username");
	nameInput->setObjectName("NameInput");
	nameInput->setStyleSheet(styleSheetTemp);

	//密码标签
	pwHint = new QLabel(this);
	pwHint->setText("输入密码");
	pwHint->setObjectName("PwHint");
	pwHint->setStyleSheet(styleSheetTemp);

	//密码输入栏
	pwInput = new QLineEdit(this);
	pwInput->setPlaceholderText("Password");
	pwInput->setObjectName("PwInput");
	pwInput->setEchoMode(QLineEdit::Password);
	pwInput->setStyleSheet(styleSheetTemp);

	//确认密码标签
	pwCheckLabel = new QLabel(this);
	pwCheckLabel->setText("确认密码");
	pwCheckLabel->setObjectName("PwHint");
	pwCheckLabel->setVisible(false);
	pwCheckLabel->setStyleSheet(styleSheetTemp);

	//服务器地址标签
	serverAddrLabel = new QLabel(this);
	serverAddrLabel->setText("服务器地址");
	serverAddrLabel->setObjectName("ServerAddrLabel");
	serverAddrLabel->setStyleSheet(styleSheetTemp);

	//确认密码输入栏
	pwCheckInput = new QLineEdit(this);
	pwCheckInput->setPlaceholderText("Confirm Password");
	pwCheckInput->setObjectName("PwInput");
	pwCheckInput->setEchoMode(QLineEdit::Password);
	pwCheckInput->setVisible(false);
	pwCheckInput->setStyleSheet(styleSheetTemp);

	//服务器地址输入框
	serverAddrInput = new QLineEdit(this);
	serverAddrInput->setPlaceholderText("Server Address");
	serverAddrInput->setObjectName("ServerAddrInput");
	serverAddrInput->setStyleSheet(styleSheetTemp);

	//进入聊天室按钮
	buttonEnter = new QPushButton(this);
	buttonEnter->setText("进入聊天室");
	buttonEnter->setObjectName("ButtonEnter");
	styleSheetTemp = "QPushButton{font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_width_ratio));
	styleSheetTemp.append("px;height:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_height_ratio));
	styleSheetTemp.append("px;border-radius:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
	styleSheetTemp.append("px;}");
	buttonEnter->setStyleSheet(styleSheetTemp);

	//新用户注册按钮
	buttonRegist = new QPushButton(this);
	buttonRegist->setText("新用户注册");
	buttonRegist->setObjectName("ButtonRegist");
	buttonRegist->setStyleSheet(styleSheetTemp);

	//注册按钮
	buttonSendRegist = new QPushButton(this);
	buttonSendRegist->setText("注册");
	buttonSendRegist->setObjectName("ButtonSendRegist");
	buttonSendRegist->setVisible(false);
	buttonSendRegist->setStyleSheet(styleSheetTemp);
	//buttonSendRegist->setStyleSheet("height:50px;width:200px");

	//返回按钮
	buttonBack = new QPushButton(this);
	buttonBack->setText("返回");
	buttonBack->setObjectName("ButtonBack");
	buttonBack->setVisible(false);
	buttonBack->setStyleSheet(styleSheetTemp);

	waitLabel = new QLabel();
	waitMovie = new QMovie(":/test/resources/wait2.gif");
	waitMovie->setScaledSize(QSize(Config::screenWidth / Config::waitmovie_ratio, Config::screenWidth / Config::waitmovie_ratio));
	waitLabel->setMovie(waitMovie);
	//waitLabel->setStyleSheet("border:1px solid rgb(40,222,235); height:30px");
	//waitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	//布局设置
	layout1 = new QVBoxLayout(this);
	layout2 = new QHBoxLayout(this);
	layout3 = new QVBoxLayout(this);
	layout4 = new QVBoxLayout(buttonSendRegist);
	layout4->setMargin(0);
	layout4->setContentsMargins(Config::screenWidth / Config::title_ratio,0,0,0);
	layout4->addWidget(waitLabel);
	layout5 = new QVBoxLayout(buttonEnter);
	layout5->setMargin(0);
	layout5->setContentsMargins(Config::screenWidth / Config::title_ratio, 0, 0, 0);
	layout5->addWidget(waitLabel);
	layout1->addWidget(serverAddrLabel, 0, Qt::AlignLeft);
	layout1->addWidget(serverAddrInput, 0, Qt::AlignLeft);
	layout1->addStretch();
	layout3->addWidget(imgLabel, 0, Qt::AlignCenter);
	layout3->addWidget(roomLabel, 0, Qt::AlignLeft);
	layout3->addWidget(roomInput, 0, Qt::AlignLeft);
	layout3->addWidget(nameHint, 0, Qt::AlignLeft);
	layout3->addWidget(nameInput, 0, Qt::AlignLeft);
	layout3->addWidget(pwHint, 0, Qt::AlignLeft);
	layout3->addWidget(pwInput, 0, Qt::AlignLeft);
	layout3->addWidget(pwCheckLabel, 0, Qt::AlignLeft);
	layout3->addWidget(pwCheckInput, 0, Qt::AlignLeft);
	layout3->addWidget(buttonEnter, 0, Qt::AlignCenter);
	layout3->addWidget(buttonRegist, 0, Qt::AlignCenter);
	layout3->addWidget(buttonSendRegist, 0, Qt::AlignCenter);
	layout3->addWidget(buttonBack, 0, Qt::AlignCenter);
	layout2->addStretch();
	layout2->addLayout(layout3);
	layout2->addStretch();
	layout1->addLayout(layout2);
	layout1->addStretch();
	//layout1->addWidget(buttonRecover, 0, Qt::AlignRight);
	layout1->setContentsMargins(0, Config::screenWidth / Config::icon_ratio, 0, 0);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(Config::screenWidth / Config::window_ratio, Config::screenWidth / Config::window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(Config::screenWidth / Config::icon_ratio, Config::screenWidth / Config::icon_ratio));
	m_titleBar->setTitleContent("聊天室", Config::screenWidth / Config::font_ratio);
	loadStyleSheet("QtUdpChat");

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &QtUdpChat::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &QtUdpChat::onButtonRestoreClicked);
	connect(buttonRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonRegistClicked);
	connect(buttonBack, &QPushButton::clicked, this, &QtUdpChat::onButtonBackClicked);
	connect(buttonSendRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonSendRegistClicked);
	connect(buttonEnter, &QPushButton::clicked, this, &QtUdpChat::onButtonEnterClicked);

	//加载聊天室界面
	chatRoom = new ChatRoom(Q_NULLPTR, udpChatService);

	//绑定回调函数
	signalEmiter = std::bind(&QtUdpChat::signalEmiterCallback, this, _1, _2);
	signalArgsEmiter = std::bind(&QtUdpChat::signalArgsEmiterCallback, this, _1, _2);

	//启动完成端口服务
	udpChatService = new UdpChatService(this, chatRoom, signalEmiter, signalArgsEmiter, chatRoom->chatEmiter, chatRoom->chatArgsEmiter);

	chatRoom->setUdpChatService(udpChatService);

	//界面信号槽
	connect(this, &QtUdpChat::showMessageBox, this, &QtUdpChat::doShowMessageBox, Qt::QueuedConnection);
	connect(this, &QtUdpChat::stopWaitingSig, this, &QtUdpChat::stopWaiting, Qt::QueuedConnection);
	connect(this, &QtUdpChat::onButtonBackClickedSig, this, &QtUdpChat::onButtonBackClicked, Qt::QueuedConnection);
	connect(this, &QtUdpChat::doSigninAckSig, this, &QtUdpChat::doSigninAck, Qt::QueuedConnection);
}

QtUdpChat::~QtUdpChat() {
	delete buttonEnter;
	delete imgLabel;
	delete imgscaled;
	delete img;
	delete m_titleBar;
	delete layout1;
	RELEASE_HANDLE(*HeartbeatThreadHandle);
	RELEASE(HeartbeatThreadHandle);
}

void QtUdpChat::paintEvent(QPaintEvent* event){
	//设置背景色
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);
	pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.fillPath(pathBack, QBrush(QColor(23, 58, 95)));

	QWidget::paintEvent(event);
}

void QtUdpChat::loadStyleSheet(const QString &sheetName) {
	QFile file(":/test/" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

//窗口最大化按钮
void QtUdpChat::onButtonMaxClicked() {
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(FactRect);
	myBorder->DrawBorder();
}

//窗口还原按钮
void QtUdpChat::onButtonRestoreClicked() {
	myBorder->DrawBorder();
}

//切换注册页面按钮
void QtUdpChat::onButtonRegistClicked() {
	roomLabel->setVisible(false);
	roomInput->setVisible(false);
	buttonRegist->setVisible(false);
	buttonEnter->setVisible(false);

	pwCheckInput->setVisible(true);
	pwCheckLabel->setVisible(true);
	buttonBack->setVisible(true);
	buttonSendRegist->setVisible(true);
}

//返回按钮
void QtUdpChat::onButtonBackClicked() {
	roomLabel->setVisible(true);
	roomInput->setVisible(true);
	buttonRegist->setVisible(true);
	buttonEnter->setVisible(true);

	pwCheckInput->setVisible(false);
	pwCheckLabel->setVisible(false);
	buttonBack->setVisible(false);
	buttonSendRegist->setVisible(false);
}

//发送注册请求按钮
void QtUdpChat::onButtonSendRegistClicked() {
	//参数vector
	vector<char*> args;

	memset(addr, 0, 16);
	QString serverAddress = serverAddrInput->text();
	if (serverAddress.length() < 8) {
		showSimpleMessageBox(0, "错误", "服务器地址未输入!");
		return;
	}
	QByteArray temp;
	temp.append(serverAddress);
	addr = temp.data();

	//参数1
	char arg1[2];
	arg1[0] = POST_REGIST;
	arg1[1] = 0;
	args.push_back(arg1);

	//参数2.3
	unsigned short id_cur = 1, id_all = 1;
	char arg2[2];
	arg2[0] = id_cur;
	arg2[1] = 0;
	args.push_back(arg2);
	char arg3[2];
	arg3[0] = id_all;
	arg3[1] = 0;
	args.push_back(arg3);

	QString usernameString = nameInput->text();
	QString passwordString = pwInput->text();
	QString passwordCheckString = pwCheckInput->text();
	if (usernameString.length() <= 1) {
		//用户名太短了
		showSimpleMessageBox(0, "错误", "用户名太短了!");
		return;
	}

	if (passwordString != passwordCheckString) {
		showSimpleMessageBox(0, "错误", "密码不一致!");
		return;
	}

	//参数4
	QByteArray temp1;
	temp1.append(usernameString);
	char* username = temp1.data();
	args.push_back(username);

	//参数5
	QByteArray temp2;
	temp2.append(passwordString);
	char* password = temp2.data();
	args.push_back(password);

	//显示转圈
	waitMovie->start();
	waitLabel->setVisible(true);
	//发送请求
	udpChatService->s_PostRequest(addr, args);
}

//用户登录按钮
void QtUdpChat::onButtonEnterClicked() {
	//参数vector
	vector<char*> args;

	QtUdpChat::addrQString = serverAddrInput->text();
	memset(addr, 0, 16);
	QString serverAddress = serverAddrInput->text();
	if (serverAddress.length() < 8) {
		showSimpleMessageBox(0, "错误", "服务器地址未输入!");
		return;
	}
	QByteArray temp;
	temp.append(serverAddress);
	addr = temp.data();

	//参数1
	char arg1[2];
	arg1[0] = CHECK_PASSWORD;
	arg1[1] = 0;
	args.push_back(arg1);

	//参数2.3
	unsigned short id_cur = 1, id_all = 1;
	char arg2[2];
	arg2[0] = id_cur;
	arg2[1] = 0;
	args.push_back(arg2);
	char arg3[2];
	arg3[0] = id_all;
	arg3[1] = 0;
	args.push_back(arg3);

	QString usernameString = nameInput->text();
	QString passwordString = pwInput->text();
	if (usernameString.length() <= 1) {
		//用户名太短了
		showSimpleMessageBox(0, "错误", "用户名太短了!");
		return;
	}

	//参数4
	QByteArray temp1;
	temp1.append(usernameString);
	char* username = temp1.data();
	args.push_back(username);

	//参数5
	QByteArray temp2;
	temp2.append(passwordString);
	char* password = temp2.data();
	args.push_back(password);

	//显示转圈
	waitMovie->start();
	waitLabel->setVisible(true);
	//发送请求
	udpChatService->s_PostRequest(addr, args);
}

//退出登录
void QtUdpChat::doSignout() {
	RELEASE_HANDLE(*HeartbeatThreadHandle);
	RELEASE(HeartbeatThreadHandle);
}

//接收到登录ACK
void QtUdpChat::doSigninAck(QVariant qv) {
	//密码正确，首先检查是否有房间号
	QString roomID;
	roomID = roomInput->text();
	if (roomID.length() < 1) {
		showSimpleMessageBox(0, "错误", "请输入房间号!");
		return;
	}
	roomid = roomID.toInt();
	if (roomid <= 0 || roomid >= 1001) {
		showSimpleMessageBox(0, "错误", "请输入正确的房间号1-1000!");
		return;
	}
	QVector<char*> v = qv.value<QVector<char*>>();
	qDebug() << v.size() << endl;
	//获取userid
	userid = atoi(v[0]);
	//检查userid是否正常
	if (userid == 0) {//不正常
		showSimpleMessageBox(0, "错误", "获取用户ID失败!");
		return;
	}
	//可以继续
	showSimpleMessageBox(1, "信息", "登录成功!");
	//进入聊天室页面
	QString serverAddress = serverAddrInput->text();
	QByteArray temp;
	temp.append(serverAddress);
	addr = temp.data();
	chatRoom->setRoomValues(roomid, userid, nameInput->text(), addr); 
	chatRoom->getRecords();
	chatRoom->show();
	this->hide();
	//打开心跳发送线程
	*HeartbeatThreadHandle = ::CreateThread(0, 0, _CheckHeartbeatThread, this, 0, nullptr);
}

//接收到心跳ACK
//暂时没什么用
void QtUdpChat::doHeartbeatAck(vector<char*> v) {
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	qDebug() << "心跳" << (int)test << endl;
}

//心跳线程函数
//每10秒发送一次心跳报文
DWORD WINAPI QtUdpChat::_CheckHeartbeatThread(LPVOID lpParam) {
	//每10秒发送一次
	do {
		//参数vector
		vector<char*> args;

		//参数1
		char arg1[2];
		arg1[0] = CHECK_HEARTBEAT;
		arg1[1] = 0;
		args.push_back(arg1);

		//参数2.3
		unsigned short id_cur = 1, id_all = 1;
		char arg2[2];
		arg2[0] = id_cur;
		arg2[1] = 0;
		args.push_back(arg2);
		char arg3[2];
		arg3[0] = id_all;
		arg3[1] = 0;
		args.push_back(arg3);

		QString useridString = QString::number(userid);
		QString roomidString = QString::number(roomid);

		//参数4
		QByteArray temp1;
		temp1.append(useridString);
		char* useridChar = temp1.data();
		args.push_back(useridChar);

		//参数5
		QByteArray temp2;
		temp2.append(roomidString);
		char* roomidChar = temp2.data();
		args.push_back(roomidChar);

		//传参发送
		memset(addr, 0, 16);
		QString serverAddress = addrQString;
		QByteArray temp;
		temp.append(serverAddress);
		addr = temp.data();
		udpChatService->s_PostRequest(addr, args);

		//停10秒
		Sleep(10000);
	} 
	while (1);
	return 0;
}

void QtUdpChat::stopWaiting() {
	//停止转圈
	waitMovie->stop();
	waitLabel->setVisible(false);
}

void QtUdpChat::showSimpleMessageBox(int type, string title, string content) {
	switch (type) {
	case 0://错误
		QMessageBox::critical(NULL, QString::fromStdString(title), QString::fromStdString(content), QMessageBox::Yes, QMessageBox::Yes);
		break;
	case 1://信息
		QMessageBox::information(NULL, QString::fromStdString(title), QString::fromStdString(content), QMessageBox::Yes, QMessageBox::Yes);
		break;
	}
}

//信号发生回调函数
//供UdpChatService类调用，发生GUI信号
void QtUdpChat::signalEmiterCallback(int type, char* content) {
	switch (type) {
	case 0:
		//错误弹窗
		emit showMessageBox(0, content);
		break;
	case 1:
		//信息弹窗
		emit showMessageBox(1, content);
		break;
	case 2:
		//停止转圈
		emit stopWaitingSig();
		break;
	case 3:
		//注册成功返回
		emit onButtonBackClickedSig();
		break;
	default:
		break;
	}
}

//带参数信号发生回调函数
void QtUdpChat::signalArgsEmiterCallback(int type, vector<char*>* vv) {
	QVector<char*> qv;
	vector<char*> v = *vv;
	for (int i = 0; i < v.size(); i++) {
		qv.push_back(v[i]);
	}
	QVariant var;
	var.setValue(qv);
	switch (type) {
	case 0:
		//登录成功，传一个userid
		emit doSigninAckSig(var);
		break;
	}
}

void QtUdpChat::doShowMessageBox(int type, char* content) {
	switch (type) {
	case 0://错误
		QMessageBox::critical(NULL, "错误", content, QMessageBox::Yes, QMessageBox::Yes);
		break;
	case 1://信息
		QMessageBox::information(NULL, "信息", content, QMessageBox::Yes, QMessageBox::Yes);
		break;
	}
}