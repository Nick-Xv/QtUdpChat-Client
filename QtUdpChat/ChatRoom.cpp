#include "ChatRoom.h"

//初始化屏幕宽高？
int ChatRoom::screenWidth = 0;
int ChatRoom::screenHeight = 0;

//定义常量（可以提取到一个新类）
const int icon_ratio = 60;
const int button_width_ratio = 12;
const int button_height_ratio = 68;
const int border_radius_ratio = 68 * 2;
const int font_ratio = 80;
const int img_ratio = 40;
const int window_ratio = 10;
const int title_ratio = 250;

const int buffer_size = 8192;

ChatRoom::ChatRoom(QWidget *parent) : QWidget(parent)
{ 
	//获取屏幕宽高
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	ChatRoom::screenWidth = screen.width();
	ChatRoom::screenHeight = screen.height();
	//计算导航栏大小
	MyTitleBar::BUTTON_HEIGHT = screenWidth / icon_ratio;
	MyTitleBar::BUTTON_WIDTH = screenWidth / icon_ratio;
	MyTitleBar::TITLE_HEIGHT = screenWidth / icon_ratio;
	//定义窗口大小
	this->resize(screenWidth / 2, screenHeight / 2);
	
	//去除标题栏
	//窗口最小化时，点击任务栏窗口可以显示出原窗口
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//设置窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(screenWidth / window_ratio, screenWidth / window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(screenWidth / icon_ratio, screenWidth / icon_ratio));
	m_titleBar->setTitleContent("聊天窗", screenWidth / font_ratio);

	//this->show();

	//按下最大化和还原按钮触发
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &ChatRoom::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &ChatRoom::onButtonRestoreClicked);
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