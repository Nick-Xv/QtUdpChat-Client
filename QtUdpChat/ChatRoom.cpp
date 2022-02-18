#include "ChatRoom.h"

//��ʼ����Ļ��ߣ�
int ChatRoom::screenWidth = 0;
int ChatRoom::screenHeight = 0;

//���峣����������ȡ��һ�����ࣩ
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
	//��ȡ��Ļ���
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	ChatRoom::screenWidth = screen.width();
	ChatRoom::screenHeight = screen.height();
	//���㵼������С
	MyTitleBar::BUTTON_HEIGHT = screenWidth / icon_ratio;
	MyTitleBar::BUTTON_WIDTH = screenWidth / icon_ratio;
	MyTitleBar::TITLE_HEIGHT = screenWidth / icon_ratio;
	//���崰�ڴ�С
	this->resize(screenWidth / 2, screenHeight / 2);
	
	//ȥ��������
	//������С��ʱ��������������ڿ�����ʾ��ԭ����
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//���ô��ڱ���͸��
	setAttribute(Qt::WA_TranslucentBackground);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(screenWidth / window_ratio, screenWidth / window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(screenWidth / icon_ratio, screenWidth / icon_ratio));
	m_titleBar->setTitleContent("���촰", screenWidth / font_ratio);

	//this->show();

	//������󻯺ͻ�ԭ��ť����
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &ChatRoom::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &ChatRoom::onButtonRestoreClicked);
}


ChatRoom::~ChatRoom()
{
}

void ChatRoom::paintEvent(QPaintEvent* event) {
	//���ñ���ɫ
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