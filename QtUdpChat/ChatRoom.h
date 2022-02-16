#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QWidget>
#include <QApplication>
#include <QDesktopWidget>

#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "QtUdpChat.h"
class ChatRoom : public QWidget
{
	Q_OBJECT
public:
	ChatRoom(QWidget *parent = Q_NULLPTR);
	~ChatRoom();

	static int screenWidth;
	static int screenHeight;

private:
	//服务类
	UdpChatService* udpChatService;
	//服务器地址和传输缓存
	char* addr;
	char* buffer;
	//边框和导航栏
	MyBorderContainer* myBorder;
	MyTitleBar* m_titleBar;
	//临时样式
	QString styleSheetTemp;
	//绘图
	void paintEvent(QPaintEvent* event);
	//加载样式
	void loadStyleSheet(const QString & sheetName);

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
};

