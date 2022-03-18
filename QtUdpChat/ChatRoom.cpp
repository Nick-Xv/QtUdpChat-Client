#include "ChatRoom.h"

//自定义延时函数
void ChatRoom::Sleep(int msec) {
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime) {
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
}

ChatRoom::ChatRoom(QWidget *parent, UdpChatService* udpChatService) : QWidget(parent)
{ 
	this->udpChatService = udpChatService;

	//注册QVariant
	qRegisterMetaType<QVariant>("QVariant");

	//绑定回调函数
	chatEmiter = std::bind(&ChatRoom::signalEmiterCallback, this, _1, _2);
	chatArgsEmiter = std::bind(&ChatRoom::signalArgsEmiterCallback, this, _1, _2);

	buffer = new char[Config::buffer_size];
	memset(buffer, 0, Config::buffer_size);

	//定义窗口大小
	this->resize(Config::screenWidth / 2, Config::screenHeight / 2);
	
	//去除标题栏
	//窗口最小化时，点击任务栏窗口可以显示出原窗口
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(Config::screenWidth / Config::window_ratio, Config::screenWidth / Config::window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(Config::screenWidth / Config::icon_ratio, Config::screenWidth / Config::icon_ratio));
	m_titleBar->setTitleContent("聊天窗", Config::screenWidth / Config::font_ratio);

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &ChatRoom::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &ChatRoom::onButtonRestoreClicked);

	//设置临时样式
	styleSheetTemp = "QPushButton{font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_width_ratio));
	styleSheetTemp.append("px;height:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_height_ratio));
	styleSheetTemp.append("px;border-radius:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
	styleSheetTemp.append("px;}");

	//登出按钮
	button_signout = new QPushButton(this);
	button_signout->setText("退出到登录页");
	button_signout->setObjectName("ButtonSignout");
	button_signout->setStyleSheet(styleSheetTemp);

	//设置临时样式
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_width_ratio / 2));
	styleSheetTemp.append("px;");

	//发送按钮
	button_send = new QPushButton(this);
	button_send->setText("发送");
	button_send->setObjectName("ButtonSend");
	button_send->setStyleSheet(styleSheetTemp);

	//文本编辑框
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;");
	edit_text = new QTextEdit(this);
	edit_text->setObjectName("ButtonSend");
	edit_text->setStyleSheet(styleSheetTemp);
	edit_text->setMaximumHeight(Config::screenHeight / 6);
	edit_text->setMinimumHeight(Config::screenHeight / 6);
	edit_text->setMinimumWidth(Config::screenWidth / 6);

	//左侧滚动
	scroll_left = new QScrollArea(this);
	

	//右侧滚动
	widget_right = new QWidget;
	widget_right->setContentsMargins(0, 0, 0, 0);
	layout_right_in = new QVBoxLayout(widget_right);
	layout_right_in->setMargin(0);
	button_getRecords = new QPushButton;
	button_getRecords->setText("获取聊天记录");
	layout_right_in->addWidget(button_getRecords, 0, Qt::AlignCenter);
	layout_right_in->addStretch();

	scroll_right = new QScrollArea;
	scroll_right->setStyleSheet("border:2px solid rgb(40,222,235);");
	scroll_right->setMinimumHeight(Config::screenHeight / 6);
	scroll_right->setWidgetResizable(true);
	scroll_right->setWidget(widget_right);

	//布局设置
	layout_all = new QHBoxLayout(this);
	layout_all->setContentsMargins(Config::screenWidth / Config::title_ratio, Config::screenWidth / Config::title_ratio + Config::screenWidth / Config::icon_ratio, Config::screenWidth / Config::title_ratio, Config::screenWidth / Config::title_ratio);

	layout_left = new QWidget(this);
	layout_left->setMinimumWidth(Config::screenWidth / 8);
	layout_left->setMaximumWidth(Config::screenWidth / 8);
	layout_left->setStyleSheet(styleSheetTemp);

	layout_left_in = new QVBoxLayout(layout_left);
	layout_left_in->setMargin(0);
	layout_left_in->addWidget(scroll_left, 0);
	layout_left_in->addWidget(button_signout, 0);

	layout_right = new QVBoxLayout(this);
	layout_right->setMargin(0);
	layout_right->addWidget(scroll_right);
	layout_right->addWidget(edit_text);
	layout_right->addWidget(button_send, 0, Qt::AlignRight);

	layout_all->addWidget(layout_left, 0);
	layout_all->addLayout(layout_right, 0);

	//加载qss
	loadStyleSheet("ChatRoom");

	//发送按钮绑定
	connect(button_send, &QPushButton::clicked, this, &ChatRoom::onButtonSendClicked);
	//获取聊天记录按钮绑定
	connect(button_getRecords, &QPushButton::clicked, this, &ChatRoom::getRecords);

	//界面信号槽绑定
	connect(this, &ChatRoom::doPostrecordAckSig, this, &ChatRoom::doPostrecordAck, Qt::QueuedConnection);
	connect(this, &ChatRoom::doPostrecordsAckSig, this, &ChatRoom::doPostrecordsAck, Qt::QueuedConnection);
}


ChatRoom::~ChatRoom()
{
}

void ChatRoom::paintEvent(QPaintEvent* event) {
	//设置背景色
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);
	pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.fillPath(pathBack, QBrush(QColor(23, 58, 95)));

	QWidget::paintEvent(event);
}

void ChatRoom::loadStyleSheet(const QString &sheetName) {
	QFile file(":/test/" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

void ChatRoom::onButtonMaxClicked() {
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(FactRect);
	myBorder->DrawBorder();
}

void ChatRoom::onButtonRestoreClicked() {
	myBorder->DrawBorder();
}

void ChatRoom::onButtonSendClicked() {
	QString test = edit_text->toPlainText();

	if (test.length() >= 1) {
		//发送请求
		//参数vector
		vector<char*> args;
		//参数1
		char arg1[2];
		arg1[0] = POST_RECORD;
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

		QString roomidString = QString::number(roomid);
		QString useridString = QString::number(userid);
		QString contentString = test;
		//参数4
		QByteArray temp1;
		temp1.append(roomidString);
		char* roomidChar = temp1.data();
		args.push_back(roomidChar);
		//参数5
		QByteArray temp2;
		temp2.append(useridString);
		char* useridChar = temp2.data();
		args.push_back(useridChar);
		//参数6
		QByteArray temp3;
		temp3.append(contentString);
		char* contentChar = temp3.data();
		args.push_back(contentChar);

		//传参发送
		udpChatService->s_PostRequest(addr, args);

		//前端展示
		edit_text->setText("");
		qDebug() << test << endl;
		QString contentQString(contentChar);
		MessageContainer* messageContainer = new MessageContainer(this, contentQString, atoi(useridChar), userName);
		messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout_right_in->addWidget(messageContainer, 0, Qt::AlignRight);
		//message指针插入list
		messageContainerList.push_back(messageContainer);
		//延迟50毫秒等待插入widget完毕
		Sleep(50);
		//滚动到最下面
		QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
		vScrollBar->setValue(vScrollBar->maximum());
	}
}

void ChatRoom::setRoomValues(int roomid, int userid, QString userName, char* addr) {
	this->roomid = roomid;
	this->userid = userid;
	this->userName = userName;
	this->addr = new char[16];
	memset(this->addr, 0, 16);
	memcpy(this->addr, addr, 16);
}

//处理收到一条消息
void ChatRoom::doPostrecordAck(vector<char*>* vv) {
	vector<char*> v = *vv;
	if (v.size() != 6) {
		qDebug() << "接收到错误数据!!!" << endl;
	}
	QString useridQString(v[0]);
	QString contentQString(v[1]);
	QString timestampQString(v[2]);
	QString usernameQString(v[3]);
	QString idrecordsQString(v[4]);
	QString roomidQString(v[5]);//roomid暂时没用
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();

	int idrecordsInt = idrecordsQString.toInt();
	int useridInt = useridQString.toInt();

	//如果是自己发的
	if (userid == useridInt) {
		list<MessageContainer*>::reverse_iterator iter;
		for (iter = messageContainerList.rbegin(); iter != messageContainerList.rend(); iter++) {
			MessageContainer* temp = *iter;
			if (!temp->hasTime() && temp->getContent() == contentQString) {
				temp->setInfo(timestampQString, idrecordsInt);
				break;
			}
		}
		//更新earliestID
		if (earliestID == 0 || idrecordsInt < earliestID) {
			earliestID = idrecordsInt;
		}
	}
	else {
		//前端展示
		MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, useridInt, usernameQString, idrecordsInt, false);
		messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout_right_in->addWidget(messageContainer, 0, Qt::AlignLeft);
		//message指针插入list
		messageContainerList.push_back(messageContainer);
		//延迟50毫秒等待插入widget完毕
		Sleep(50);
		//滚动到最下面
		QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
		vScrollBar->setValue(vScrollBar->maximum());
	}
	
}

//处理收到的消息记录
void ChatRoom::doPostrecordsAck(vector<char*>* vv) {
	vector<char*> v = *vv;
	if (v.size() != 6) {
		qDebug() << "接收到错误数据!!!" << endl;
	}
	QString useridQString(v[0]);
	QString contentQString(v[1]);
	QString timestampQString(v[2]);
	QString usernameQString(v[3]);
	QString idrecordsQString(v[4]);
	QString roomidQString(v[5]);//roomid暂时没用
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();

	int idrecordsInt = idrecordsQString.toInt();
	int useridInt = useridQString.toInt();

	//获取完整记录以后，进行一条消息的前端显示
	bool mine = false;
	if (this->userid == useridInt) mine = true;
	MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, useridInt, usernameQString, idrecordsInt, mine);
	messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	//如果是自己发的
	if (this->userid == useridInt) {
		layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignRight);
	}
	else {
		layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignLeft);
	}
	//message指针插入list
	messageContainerList.push_front(messageContainer);

	//更新earliestID
	if (earliestID == 0 || idrecordsInt < earliestID) {
		earliestID = idrecordsInt;
	}
}

void ChatRoom::getRecords() {
	//获取部分聊天记录
	//参数vector
	vector<char*> args;
	//参数1
	char arg1[2];
	arg1[0] = GET_RECORD;
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

	QString roomidString = QString::number(roomid);
	QString idrecordsString = QString::number(earliestID);
	//参数4
	QByteArray temp1;
	temp1.append(roomidString);
	char* roomidChar = temp1.data();
	args.push_back(roomidChar);

	//参数5
	QByteArray temp2;
	temp2.append(idrecordsString);
	char* idrecordsChar = temp2.data();
	args.push_back(idrecordsChar);

	//传参发送
	udpChatService->s_PostRequest(addr, args);
}

//回调函数，信号发生
void ChatRoom::signalEmiterCallback(int type, char* content) {

}

//带vector参数回调函数
void ChatRoom::signalArgsEmiterCallback(int type, vector<char*>* args) {
	QVector<char*> v;

	switch (type) {
	case 0:
		//收到一条聊天记录
		emit doPostrecordAckSig(args);
		break;
	case 1:
		//收到一堆聊天记录
		emit doPostrecordsAckSig(args);
		break;
	}
}

void ChatRoom::setUdpChatService(UdpChatService* u) {
	udpChatService = u;
}