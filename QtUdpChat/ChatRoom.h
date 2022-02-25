#pragma once
#pragma execution_character_set("utf-8")
/*
ChatRoom聊天室界面类
包含窗体布局和窗体组件
1.展示窗体

2.接收来自UdpChatService的信号
3.根据信号内的数据进行信息显示

2.将窗体上的用户操作产生的数据通过信号发给UdpChatService
*/
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
#include <list>
#include <vector>
#include <algorithm>

#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "MessageContainer.h"
#include "MemberContainer.h"

#include "Config.h"

using std::list;
class ChatRoom : public QWidget
{
	Q_OBJECT
public:
	ChatRoom(QWidget *parent, UdpChatService* udpChatService);
	~ChatRoom();

	void setRoomValues(int roomid, int userid, QString userName, char* addr);
	void getRecords();

	void doPostrecordAck(vector<char*> c);
	void doPostrecordsAck(vector<char*> c);

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

	QPushButton* button_getRecords;//获取聊天记录按钮

	QTextEdit* edit_text;//文本编辑框

	QString userName;//本人的用户名
	int userid;//本人的id
	int roomid;//当前聊天室的id

	int earliestID = 0;//记录当前最老的一条消息id

	//记录消息组件的指针
	list<MessageContainer*> messageContainerList;

	//list<MemberContainer*> memberContainerList;

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();

	void onButtonSendClicked();
};