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
	//收到消息绑定
	connect(udpChatService, &UdpChatService::post_record_ack, this, &ChatRoom::doPostrecordAck);

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
		memset(buffer, 0, Config::buffer_size);
		buffer[0] = POST_RECORD;
		unsigned short id_cur = 1, id_all = 1;
		memcpy(&buffer[1], &(id_cur), sizeof(id_cur));
		memcpy(&buffer[3], &(id_all), sizeof(id_all));

		QString roomidString = QString::number(roomid);
		QString useridString = QString::number(userid);
		QString contentString = test;
		QByteArray temp1;
		temp1.append(roomidString);
		const char* roomidChar = temp1.data();
		QByteArray temp2;
		temp2.append(useridString);
		const char* useridChar = temp2.data();
		QByteArray temp3;
		temp3.append(contentString);
		const char* contentChar = temp3.data();
		memcpy(&buffer[5], roomidChar, strlen(roomidChar));
		memcpy(&buffer[5 + strlen(roomidChar) + 1], useridChar, strlen(useridChar));
		memcpy(&buffer[5 + strlen(roomidChar) + strlen(useridChar) + 2], contentChar, strlen(contentChar));

		udpChatService->s_PostRequest(addr, buffer);

		//前端展示
		edit_text->setText("");
		qDebug() << test << endl;
		QLabel* testLabel = new QLabel;
		QLabel* nameLabel = new QLabel;
		nameLabel->setText(userName);
		styleSheetTemp = "font-size:";
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio / 1.2));
		styleSheetTemp.append("px;");
		nameLabel->setStyleSheet(styleSheetTemp);
		testLabel->setText(test);
		styleSheetTemp = "background-color:orange;color:white;font-size:";
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio * 1.5));
		styleSheetTemp.append("px;border-radius:");
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
		styleSheetTemp.append("px;margin-right:");
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
		styleSheetTemp.append("px;");
		testLabel->setStyleSheet(styleSheetTemp);
		testLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		layout_right_in->addWidget(nameLabel, 0, Qt::AlignRight);
		layout_right_in->addWidget(testLabel, 0, Qt::AlignRight);
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

void ChatRoom::doPostrecordAck(char* buffer) {
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	qDebug() << (int)test << endl;

	//获取idrecords,roomid,content,userid,timestamp
	int i = 2;
	int beginPtr[5] = { 0 };
	int endPtr[5] = { 0 };
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
		qDebug() << "接收到的数据错误" << endl;
		return;
	}

	char* idrecordsChar = new char[endPtr[0] - beginPtr[0] + 1];
	char* roomidChar = new char[endPtr[1] - beginPtr[1] + 1];
	char* contentChar = new char[endPtr[2] - beginPtr[2] + 1];
	char* useridChar = new char[endPtr[3] - beginPtr[3] + 1];
	char* timestampChar = new char[endPtr[4] - beginPtr[4] + 1];

	memset(idrecordsChar, 0, endPtr[0] - beginPtr[0] + 1);
	memset(roomidChar, 0, endPtr[1] - beginPtr[1] + 1);
	memset(contentChar, 0, endPtr[2] - beginPtr[2] + 1);
	memset(useridChar, 0, endPtr[3] - beginPtr[3] + 1);
	memset(timestampChar, 0, endPtr[4] - beginPtr[4] + 1);

	memcpy(idrecordsChar, &buffer[beginPtr[0]], endPtr[0] - beginPtr[0]);
	memcpy(roomidChar, &buffer[beginPtr[1]], endPtr[1] - beginPtr[1]);
	memcpy(contentChar, &buffer[beginPtr[2]], endPtr[2] - beginPtr[2]);
	memcpy(useridChar, &buffer[beginPtr[3]], endPtr[3] - beginPtr[3]);
	memcpy(timestampChar, &buffer[beginPtr[4]], endPtr[4] - beginPtr[4]);

	QString useridQString(useridChar);
	QString contentQString(contentChar);
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
	QLabel* testLabel = new QLabel;
	QLabel* nameLabel = new QLabel;
	//根据返回值进行操作
	switch ((int)test) { 
	case 0:
		//数据错误
		qDebug() << "接收到错误数据!!!" << endl;
		break;
	case 1:
		//检测到有人发了一条消息
		//前端展示
		nameLabel->setText(useridQString);
		styleSheetTemp = "font-size:";
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio / 1.2));
		styleSheetTemp.append("px;");
		nameLabel->setStyleSheet(styleSheetTemp);
		testLabel->setText(contentQString);
		styleSheetTemp = "background-color:blue;color:white;font-size:";
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio * 1.5));
		styleSheetTemp.append("px;border-radius:");
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
		styleSheetTemp.append("px;margin-right:");
		styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
		styleSheetTemp.append("px;");
		testLabel->setStyleSheet(styleSheetTemp);
		testLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		layout_right_in->addWidget(nameLabel, 0, Qt::AlignLeft);
		layout_right_in->addWidget(testLabel, 0, Qt::AlignLeft);
		//延迟50毫秒等待插入widget完毕
		Sleep(50);
		//滚动到最下面
		vScrollBar->setValue(vScrollBar->maximum());
		break;
	case 2:
		//数据库错误
		qDebug() << "数据库插入错误" << endl;
		break;
	default:
		//未知返回值
		qDebug() << "接收到未知的返回值" << endl;
		break;
	}
}