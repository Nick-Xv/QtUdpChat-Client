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
	//�Զ�����ʱ����
	void Sleep(int msec);
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

	QHBoxLayout* layout_all;//�ܲ���
	QWidget* layout_left;//���widget
	QWidget* widget_right;//���촰widget
	QVBoxLayout* layout_left_in;//������򲼾�
	QVBoxLayout* layout_right;//�Ҳ����򲼾�
	QVBoxLayout* layout_right_in;//�Ҳ������ڲ�����

	QScrollArea* scroll_left;//���������
	QScrollArea* scroll_right;//�Ҳ�������

	QPushButton* button_signout;//�ǳ���ť
	QPushButton* button_send;//���Ͱ�ť

	QTextEdit* edit_text;//�ı��༭��

	QString userName;//���˵��û���
	int userid;//���˵�id
	int roomid;//��ǰ�����ҵ�id

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();

	void onButtonSendClicked();

	void doPostrecordAck(char* buffer);
};