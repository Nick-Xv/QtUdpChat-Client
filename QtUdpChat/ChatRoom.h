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
	//������
	UdpChatService* udpChatService;
	//��������ַ�ʹ��仺��
	char* addr;
	char* buffer;
	//�߿�͵�����
	MyBorderContainer* myBorder;
	MyTitleBar* m_titleBar;
	//��ʱ��ʽ
	QString styleSheetTemp;
	//��ͼ
	void paintEvent(QPaintEvent* event);
	//������ʽ
	void loadStyleSheet(const QString & sheetName);

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
};

