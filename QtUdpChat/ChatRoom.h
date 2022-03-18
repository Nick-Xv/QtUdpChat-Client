#pragma once
#pragma execution_character_set("utf-8")
/*
ChatRoom�����ҽ�����
�������岼�ֺʹ������
1.չʾ����

2.��������UdpChatService���ź�
3.�����ź��ڵ����ݽ�����Ϣ��ʾ

2.�������ϵ��û���������������ͨ���źŷ���UdpChatService
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
#include <QVariant>
#include <QMetaType>

#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "UdpChatService.h"
#include "MessageContainer.h"
#include "MemberContainer.h"

#include "Config.h"

Q_DECLARE_METATYPE(QVector<char*>);

//ChatEmiter����ģ�壬����int��char*����������void
typedef std::function<void(int, char*)> ChatEmiter;
//ChatArgsEmiter����ģ�壬����int��vector<char*>����������void
typedef std::function<void(int, vector<char*>*)> ChatArgsEmiter;

using std::list;
class ChatRoom : public QWidget
{
	Q_OBJECT
public:
	ChatRoom(QWidget *parent, UdpChatService* udpChatService);
	~ChatRoom();

	void setUdpChatService(UdpChatService* u);
	void setRoomValues(int roomid, int userid, QString userName, char* addr);
	void getRecords();

	void doPostrecordAck(vector<char*>* c);
	void doPostrecordsAck(vector<char*>* c);

	//�ص��������źŷ�����
	ChatEmiter chatEmiter;
	ChatArgsEmiter chatArgsEmiter;

signals:
	void doPostrecordAckSig(vector<char*>* args);
	void doPostrecordsAckSig(vector<char*>* args);

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

	QPushButton* button_getRecords;//��ȡ�����¼��ť

	QTextEdit* edit_text;//�ı��༭��

	QString userName;//���˵��û���
	int userid;//���˵�id
	int roomid;//��ǰ�����ҵ�id

	int earliestID = 0;//��¼��ǰ���ϵ�һ����Ϣid

	//��¼��Ϣ�����ָ��
	list<MessageContainer*> messageContainerList;

	//list<MemberContainer*> memberContainerList;

	//�źŷ�������
	void signalEmiterCallback(int type, char* content);
	void signalArgsEmiterCallback(int type, vector<char*>* args);

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();

	void onButtonSendClicked();
};