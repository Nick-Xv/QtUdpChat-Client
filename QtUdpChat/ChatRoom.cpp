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
}


ChatRoom::~ChatRoom()
{
}
