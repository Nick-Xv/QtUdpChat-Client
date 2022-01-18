#include "QtUdpChat.h"

QtUdpChat::QtUdpChat(QWidget *parent)
    : QWidget(parent)
{
	this->resize(500,500);
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
	nameHint = new QLabel(this);
	nameHint->setText("输入用户名            ");
	nameHint->setObjectName("NameHint");

	//设置一个输入栏
	nameInput = new QLineEdit(this);
	nameInput->setPlaceholderText("Username");
	nameInput->setObjectName("NameInput");

	//设置一个标签
	pwHint = new QLabel(this);
	pwHint->setText("输入密码              ");
	pwHint->setObjectName("PwHint");

	//设置一个输入栏
	pwInput = new QLineEdit(this);
	pwInput->setPlaceholderText("Password");
	pwInput->setObjectName("PwInput");
	pwInput->setEchoMode(QLineEdit::Password);

	//设置一个按钮
	buttonEnter = new QPushButton(this);
	buttonEnter->setText("进入聊天室");
	buttonEnter->setObjectName("ButtonEnter");

	//设置一个按钮
	buttonRegist = new QPushButton(this);
	buttonRegist->setText("新用户注册");
	buttonRegist->setObjectName("ButtonRegist");

	//设置一个按钮
	buttonRecover = new QPushButton(this);
	buttonRecover->setText("密码找回");
	buttonRecover->setObjectName("ButtonRecover");

	//布局设置
	layout1 = new QVBoxLayout(this);
	layout1->addStretch();
	layout1->addWidget(imgLabel, 0, Qt::AlignCenter);
	layout1->addWidget(nameHint, 0, Qt::AlignCenter);
	layout1->addWidget(nameInput, 0, Qt::AlignCenter);
	layout1->addWidget(pwHint, 0, Qt::AlignCenter);
	layout1->addWidget(pwInput, 0, Qt::AlignCenter);
	layout1->addWidget(buttonEnter, 0, Qt::AlignCenter);
	layout1->addStretch();
	layout1->addWidget(buttonRegist, 0, Qt::AlignRight);
	layout1->addWidget(buttonRecover, 0, Qt::AlignRight);
	layout1->setContentsMargins(0, 32, 0, 0);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(buttonEnter->width() + 10, buttonEnter->width() + 10);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(32, 32));
	//m_titleBar->move(0, 0);

	loadStyleSheet("QtUdpChat");

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &QtUdpChat::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &QtUdpChat::onButtonRestoreClicked);
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