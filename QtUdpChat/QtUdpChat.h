#pragma once
#pragma execution_character_set("utf-8")
/*
QtUdpChat
登录界面类
1.将用户操作产生的数据通过信号发给UdpChatService
2.接收来自UdpChatService的信号并做出UI反馈
*/
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QMovie>
#include <QPixmap>
#include <QPushButton>
#include <QPainter>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <string>
#include <QVariant>
#include <QMetaType>
#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "ChatRoom.h"
#include "Config.h"

Q_DECLARE_METATYPE(QVector<char*>);

//Emiter函数模板，接受int和char*参数，返回void
typedef std::function<void(int, char*)> Emiter;
typedef std::function<void(int, vector<char*>*)> ArgsEmiter;

class QtUdpChat : public QWidget
{
    Q_OBJECT
	
public:
    QtUdpChat(QWidget *parent = Q_NULLPTR);
	~QtUdpChat();

	static int roomid;
	static int userid;

	void doSigninAck(QVariant);
	void doHeartbeatAck(vector<char*> v);

	void stopWaiting();

	void showSimpleMessageBox(int type, string title, string content);

	//回调函数：信号发生器
	Emiter signalEmiter;
	ArgsEmiter signalArgsEmiter;

signals:
	void showMessageBox(int type, char* content);
	void stopWaitingSig();
	void onButtonBackClickedSig();
	void doSigninAckSig(QVariant);

protected:
	static DWORD WINAPI _CheckHeartbeatThread(LPVOID lpParam);//心跳线程函数
	static char* hbBuffer;

private:
	QVBoxLayout* layout1;//总layout
	QHBoxLayout* layout2;//中间layout
	QVBoxLayout* layout3;//中间的中间layout;
	QVBoxLayout* layout4;//
	QVBoxLayout* layout5;//
	QLabel* waitLabel;
	QMovie* waitMovie;
	QLabel* imgLabel;
	QPushButton* buttonEnter;
	QImage* img;
	QImage* imgscaled;
	QLabel* nameHint;
	QLineEdit* nameInput;
	QLabel* pwHint;
	QLineEdit* pwInput;
	QPushButton* buttonRegist;
	QPushButton* buttonRecover;
	QLabel* roomLabel;
	QLineEdit* roomInput;
	QPushButton* buttonSendRegist;
	QPushButton* buttonBack;
	QLabel* pwCheckLabel;
	QLineEdit* pwCheckInput;
	QLabel* serverAddrLabel;
	QLineEdit* serverAddrInput;

	QString styleSheetTemp;
	void paintEvent(QPaintEvent* event);
	void loadStyleSheet(const QString & sheetName);

	MyBorderContainer* myBorder;
	MyTitleBar* m_titleBar;

	static char* addr;
	char* buffer;
	static QString addrQString;

	static UdpChatService* udpChatService;

	//聊天室界面类
	ChatRoom* chatRoom;

	//心跳线程句柄
	HANDLE* HeartbeatThreadHandle;

	//信号发生函数
	void signalEmiterCallback(int type, char* content);
	void signalArgsEmiterCallback(int type, vector<char*>* v);

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
	void onButtonRegistClicked();
	void onButtonBackClicked();
	void onButtonSendRegistClicked();
	void onButtonEnterClicked();

	void doSignout();

	void doShowMessageBox(int type, char* content);
};
