#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QTextEdit>
#include <QDebug>
#include <QScrollBar>
#include <QTime>

#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "MessageContainer.h"

#include "Config.h"
class ChatRoom : public QWidget
{
	Q_OBJECT
public:
	ChatRoom(QWidget *parent, UdpChatService* udpChatService);
	~ChatRoom();

	void setRoomValues(int roomid, int userid, QString userName, char* addr);

private:
	//自定义延时函数
	void Sleep(int msec);
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

	QHBoxLayout* layout_all;//总布局
	QWidget* layout_left;//左侧widget
	QWidget* widget_right;//聊天窗widget
	QVBoxLayout* layout_left_in;//左侧竖向布局
	QVBoxLayout* layout_right;//右侧竖向布局
	QVBoxLayout* layout_right_in;//右侧竖向内部布局

	QScrollArea* scroll_left;//左侧滚动组件
	QScrollArea* scroll_right;//右侧滚动组件

	QPushButton* button_signout;//登出按钮
	QPushButton* button_send;//发送按钮

	QTextEdit* edit_text;//文本编辑框

	QString userName;//本人的用户名
	int userid;//本人的id
	int roomid;//当前聊天室的id

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();

	void onButtonSendClicked();

	void doPostrecordAck(char* buffer);
};