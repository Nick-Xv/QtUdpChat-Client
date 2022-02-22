#pragma once
#pragma execution_character_set("utf-8")
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
#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "ChatRoom.h"

#include "Config.h"

class QtUdpChat : public QWidget
{
    Q_OBJECT
	
public:
    QtUdpChat(QWidget *parent = Q_NULLPTR);
	~QtUdpChat();

	static int roomid;
	static int userid;

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

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
	void onButtonRegistClicked();
	void onButtonBackClicked();
	void onButtonSendRegistClicked();
	void onButtonEnterClicked();

	void doRegistAck(char* buffer);
	void doSigninAck(char* buffer);
	void doHeartbeatAck(char* buffer);

	void doSignout();
};
