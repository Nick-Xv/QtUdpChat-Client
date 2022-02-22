#include "Config.h"
#include <QDesktopWidget>
#include <QApplication>

int Config::BUTTON_HEIGHT = 0;
int Config::BUTTON_WIDTH = 0;
int Config::TITLE_HEIGHT = 0;
int Config::screenWidth = 0;
int Config::screenHeight = 0;

Config::Config()
{
	//获取屏幕宽高
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	screenWidth = screen.width();
	screenHeight = screen.height();
	//计算导航栏大小
	BUTTON_HEIGHT = screenWidth / icon_ratio;
	BUTTON_WIDTH = screenWidth / icon_ratio;
	TITLE_HEIGHT = screenWidth / icon_ratio;
}


Config::~Config()
{
}
