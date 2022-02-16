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
}


ChatRoom::~ChatRoom()
{
}
