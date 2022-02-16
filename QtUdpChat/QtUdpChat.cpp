#include "QtUdpChat.h"

const int icon_ratio = 60;
const int button_width_ratio = 12;
const int button_height_ratio = 68;
const int border_radius_ratio = 68 * 2;
const int font_ratio = 80;
const int img_ratio = 40;
const int window_ratio = 10;
const int title_ratio = 250;

const int buffer_size = 8192;

int QtUdpChat::screenWidth = 0;
int QtUdpChat::screenHeight = 0;
int MyTitleBar::BUTTON_HEIGHT = 32;
int MyTitleBar::BUTTON_WIDTH = 32;
int MyTitleBar::TITLE_HEIGHT = 32;

QtUdpChat::QtUdpChat(QWidget *parent)
    : QWidget(parent)
{
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	QtUdpChat::screenWidth = screen.width();
	QtUdpChat::screenHeight = screen.height();

	MyTitleBar::BUTTON_HEIGHT = screenWidth / icon_ratio;
	MyTitleBar::BUTTON_WIDTH = screenWidth / icon_ratio;
	MyTitleBar::TITLE_HEIGHT = screenWidth / icon_ratio;

	qDebug() << QtUdpChat::screenWidth << QtUdpChat::screenHeight << endl;

	this->resize(screenWidth / 3, screenHeight / 2);
	//去除标题栏
	//窗口最小化时，点击任务栏窗口可以显示出原窗口
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);
	//setAttribute(Qt::WA_DeleteOnClose);
	
	//设置一个图片
	img = new QImage(":/test/resources/chat_m.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(screenWidth / img_ratio, screenWidth / img_ratio, Qt::KeepAspectRatio);
	imgLabel = new QLabel(this);
	imgLabel->setPixmap(QPixmap::fromImage(*imgscaled));
	imgLabel->setObjectName("ImgLabel");

	//设置一个标签
	roomLabel = new QLabel(this);
	roomLabel->setText("输入房间号");
	roomLabel->setObjectName("RoomLabel");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(screenWidth / font_ratio));
	styleSheetTemp.append("px;");
	roomLabel->setStyleSheet(styleSheetTemp);

	//设置一个输入栏
	roomInput = new QLineEdit(this);
	roomInput->setObjectName("RoomInput");
	roomInput->setPlaceholderText("房间号");
	roomInput->setStyleSheet(styleSheetTemp);

	//设置一个标签
	nameHint = new QLabel(this);
	nameHint->setText("输入用户名");
	nameHint->setObjectName("NameHint");
	nameHint->setStyleSheet(styleSheetTemp);

	//设置一个输入栏
	nameInput = new QLineEdit(this);
	nameInput->setPlaceholderText("Username");
	nameInput->setObjectName("NameInput");
	nameInput->setStyleSheet(styleSheetTemp);

	//设置一个标签
	pwHint = new QLabel(this);
	pwHint->setText("输入密码");
	pwHint->setObjectName("PwHint");
	pwHint->setStyleSheet(styleSheetTemp);

	//设置一个输入栏
	pwInput = new QLineEdit(this);
	pwInput->setPlaceholderText("Password");
	pwInput->setObjectName("PwInput");
	pwInput->setEchoMode(QLineEdit::Password);
	pwInput->setStyleSheet(styleSheetTemp);

	//设置一个标签
	pwCheckLabel = new QLabel(this);
	pwCheckLabel->setText("确认密码");
	pwCheckLabel->setObjectName("PwHint");
	pwCheckLabel->setVisible(false);
	pwCheckLabel->setStyleSheet(styleSheetTemp);

	//设置一个标签
	serverAddrLabel = new QLabel(this);
	serverAddrLabel->setText("服务器地址");
	serverAddrLabel->setObjectName("ServerAddrLabel");
	serverAddrLabel->setStyleSheet(styleSheetTemp);

	//设置一个输入栏
	pwCheckInput = new QLineEdit(this);
	pwCheckInput->setPlaceholderText("Confirm Password");
	pwCheckInput->setObjectName("PwInput");
	pwCheckInput->setEchoMode(QLineEdit::Password);
	pwCheckInput->setVisible(false);
	pwCheckInput->setStyleSheet(styleSheetTemp);

	//设置一个输入框
	serverAddrInput = new QLineEdit(this);
	serverAddrInput->setPlaceholderText("Server Address");
	serverAddrInput->setObjectName("ServerAddrInput");
	serverAddrInput->setStyleSheet(styleSheetTemp);

	//设置一个按钮
	buttonEnter = new QPushButton(this);
	buttonEnter->setText("进入聊天室");
	buttonEnter->setObjectName("ButtonEnter");
	styleSheetTemp = "QPushButton{font-size:";
	styleSheetTemp.append(QString::number(screenWidth / font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(screenWidth / button_width_ratio));
	styleSheetTemp.append("px;height:");
	styleSheetTemp.append(QString::number(screenWidth / button_height_ratio));
	styleSheetTemp.append("px;border-radius:");
	styleSheetTemp.append(QString::number(screenWidth / border_radius_ratio));
	styleSheetTemp.append("px;}");
	buttonEnter->setStyleSheet(styleSheetTemp);

	//设置一个按钮
	buttonRegist = new QPushButton(this);
	buttonRegist->setText("新用户注册");
	buttonRegist->setObjectName("ButtonRegist");
	buttonRegist->setStyleSheet(styleSheetTemp);

	//设置一个按钮
	/*
	buttonRecover = new QPushButton(this);
	buttonRecover->setText("密码找回");
	buttonRecover->setObjectName("ButtonRecover");
	*/

	//设置一个按钮
	buttonSendRegist = new QPushButton(this);
	buttonSendRegist->setText("注册");
	buttonSendRegist->setObjectName("ButtonSendRegist");
	buttonSendRegist->setVisible(false);
	buttonSendRegist->setStyleSheet(styleSheetTemp);
	//buttonSendRegist->setStyleSheet("height:50px;width:200px");

	//设置一个按钮
	buttonBack = new QPushButton(this);
	buttonBack->setText("返回");
	buttonBack->setObjectName("ButtonBack");
	buttonBack->setVisible(false);
	buttonBack->setStyleSheet(styleSheetTemp);

	waitLabel = new QLabel();
	waitMovie = new QMovie(":/test/resources/wait2.gif");
	waitMovie->setScaledSize(QSize(screenWidth / 100, screenWidth / 100));
	waitLabel->setMovie(waitMovie);
	//waitLabel->setStyleSheet("border:1px solid rgb(40,222,235); height:30px");
	//waitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	//布局设置
	layout1 = new QVBoxLayout(this);
	layout2 = new QHBoxLayout(this);
	layout3 = new QVBoxLayout(this);
	layout4 = new QVBoxLayout(buttonSendRegist);
	layout4->setMargin(0);
	layout4->setContentsMargins(screenWidth / title_ratio,0,0,0);
	layout4->addWidget(waitLabel);
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
	layout1->setContentsMargins(0, screenWidth / icon_ratio, 0, 0);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(screenWidth / window_ratio, screenWidth / window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(screenWidth / icon_ratio, screenWidth / icon_ratio));
	m_titleBar->setTitleContent("聊天室", screenWidth / title_ratio);
	loadStyleSheet("QtUdpChat");

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &QtUdpChat::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &QtUdpChat::onButtonRestoreClicked);
	connect(buttonRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonRegistClicked);
	connect(buttonBack, &QPushButton::clicked, this, &QtUdpChat::onButtonBackClicked);
	connect(buttonSendRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonSendRegistClicked);

	//启动完成端口服务
	udpChatService = new UdpChatService();

	//绑定服务信号槽
	//注册
	connect(udpChatService, &UdpChatService::post_regist_ack, this, &QtUdpChat::doRegistAck);
}

QtUdpChat::~QtUdpChat() {
	delete buttonEnter;
	delete imgLabel;
	delete imgscaled;
	delete img;
	delete m_titleBar;
	delete layout1;
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

void QtUdpChat::onButtonMaxClicked() {
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(FactRect);
	myBorder->DrawBorder();
}

void QtUdpChat::onButtonRestoreClicked() {
	myBorder->DrawBorder();
}

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

void QtUdpChat::onButtonSendRegistClicked() {
	addr = new char[16];
	memset(addr, 0, 16);
	QString serverAddress = serverAddrInput->text();
	if (serverAddress.length() < 8) {
		QMessageBox::critical(NULL, "错误", "服务器地址未输入", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	QByteArray temp;
	temp.append(serverAddress);
	addr = temp.data();

	buffer = new char[buffer_size];
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
}

void QtUdpChat::doRegistAck(char* buffer) {
	//输出返回值
	char test;
	memcpy(&test, &buffer[1], 1);
	qDebug() << (int)test << endl;
	
	//根据返回值进行操作
	switch ((int)test) {
	case 0:
		//数据错误
		QMessageBox::critical(NULL, "错误", "数据错误", QMessageBox::Yes, QMessageBox::Yes);
		break;
	case 1:
		//用户名已存在
		QMessageBox::critical(NULL, "错误", "用户名已存在", QMessageBox::Yes, QMessageBox::Yes);
		break;
	case 2:
		//数据库错误
		QMessageBox::critical(NULL, "错误", "数据库错误", QMessageBox::Yes, QMessageBox::Yes);
		break;
	case 3:
		//注册成功
		QMessageBox::information(NULL, "信息", "注册成功！", QMessageBox::Yes, QMessageBox::Yes);
		//此处跳转到登录页面
		onButtonBackClicked();
		break;
	default:
		//未知返回值
		QMessageBox::critical(NULL, "错误", "未知返回值错误", QMessageBox::Yes, QMessageBox::Yes);
		break;
	}
	//停止转圈
	waitMovie->stop();
	waitLabel->setVisible(false);
}