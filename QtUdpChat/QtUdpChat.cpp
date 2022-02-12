﻿#include "QtUdpChat.h"

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
	if (screenWidth > 2000) {
		MyTitleBar::BUTTON_HEIGHT = 64;
		MyTitleBar::BUTTON_WIDTH = 64;
		MyTitleBar::TITLE_HEIGHT = 64;
	}
	qDebug() << QtUdpChat::screenWidth << QtUdpChat::screenHeight << endl;

	this->resize(screenWidth/3, screenHeight/2);
	//去除标题栏
	//窗口最小化时，点击任务栏窗口可以显示出原窗口
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);
	//setAttribute(Qt::WA_DeleteOnClose);
	
	//设置一个图片
	img = new QImage(":/test/resources/chat_m.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(48,48,Qt::KeepAspectRatio);
	imgLabel = new QLabel(this);
	imgLabel->setPixmap(QPixmap::fromImage(*imgscaled));
	imgLabel->setObjectName("ImgLabel");

	//设置一个标签
	roomLabel = new QLabel(this);
	roomLabel->setText("输入房间号");
	roomLabel->setObjectName("RoomLabel");

	//设置一个输入栏
	roomInput = new QLineEdit(this);
	roomInput->setObjectName("RoomInput");
	roomInput->setPlaceholderText("房间号");

	//设置一个标签
	nameHint = new QLabel(this);
	nameHint->setText("输入用户名");
	nameHint->setObjectName("NameHint");

	//设置一个输入栏
	nameInput = new QLineEdit(this);
	nameInput->setPlaceholderText("Username");
	nameInput->setObjectName("NameInput");

	//设置一个标签
	pwHint = new QLabel(this);
	pwHint->setText("输入密码");
	pwHint->setObjectName("PwHint");

	//设置一个输入栏
	pwInput = new QLineEdit(this);
	pwInput->setPlaceholderText("Password");
	pwInput->setObjectName("PwInput");
	pwInput->setEchoMode(QLineEdit::Password);

	//设置一个标签
	pwCheckLabel = new QLabel(this);
	pwCheckLabel->setText("确认密码");
	pwCheckLabel->setObjectName("PwHint");
	pwCheckLabel->setVisible(false);

	//设置一个输入栏
	pwCheckInput = new QLineEdit(this);
	pwCheckInput->setPlaceholderText("Confirm Password");
	pwCheckInput->setObjectName("PwInput");
	pwCheckInput->setEchoMode(QLineEdit::Password);
	pwCheckInput->setVisible(false);

	//设置一个按钮
	buttonEnter = new QPushButton(this);
	buttonEnter->setText("进入聊天室");
	buttonEnter->setObjectName("ButtonEnter");

	//设置一个按钮
	buttonRegist = new QPushButton(this);
	buttonRegist->setText("新用户注册");
	buttonRegist->setObjectName("ButtonRegist");

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

	//设置一个按钮
	buttonBack = new QPushButton(this);
	buttonBack->setText("返回");
	buttonBack->setObjectName("ButtonBack");
	buttonBack->setVisible(false);

	//布局设置
	layout1 = new QVBoxLayout(this);
	layout1->addStretch();
	layout1->addWidget(imgLabel, 0, Qt::AlignCenter);
	layout1->addWidget(roomLabel, 0, Qt::AlignCenter);
	layout1->addWidget(roomInput, 0, Qt::AlignCenter);
	layout1->addWidget(nameHint, 0, Qt::AlignCenter);
	layout1->addWidget(nameInput, 0, Qt::AlignCenter);
	layout1->addWidget(pwHint, 0, Qt::AlignCenter);
	layout1->addWidget(pwInput, 0, Qt::AlignCenter);
	layout1->addWidget(pwCheckLabel, 0, Qt::AlignCenter);
	layout1->addWidget(pwCheckInput, 0, Qt::AlignCenter);
	layout1->addWidget(buttonEnter, 0, Qt::AlignCenter);
	layout1->addWidget(buttonRegist, 0, Qt::AlignCenter);
	layout1->addWidget(buttonSendRegist, 0, Qt::AlignCenter);
	layout1->addWidget(buttonBack, 0, Qt::AlignCenter);
	layout1->addStretch();
	//layout1->addWidget(buttonRecover, 0, Qt::AlignRight);
	layout1->setContentsMargins(0, 32, 0, 0);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(buttonEnter->width() + 10, buttonEnter->width() + 10);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(32, 32));
	//m_titleBar->move(0, 0);

	if (screenWidth > 2000) {
		loadStyleSheet("QtUdpChat-Big");
	}
	else loadStyleSheet("QtUdpChat");

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &QtUdpChat::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &QtUdpChat::onButtonRestoreClicked);
	connect(buttonRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonRegistClicked);
	connect(buttonBack, &QPushButton::clicked, this, &QtUdpChat::onButtonBackClicked);
	connect(buttonSendRegist, &QPushButton::clicked, this, &QtUdpChat::onButtonSendRegistClicked);

	//启动完成端口服务
	udpChatService = new UdpChatService();
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

}