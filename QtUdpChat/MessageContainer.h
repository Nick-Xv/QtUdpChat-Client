#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QImage>
#include <QDebug>

#include "Config.h"
class MessageContainer : public QWidget {
	Q_OBJECT
public:
	//���˷���Ϣ�Ĺ���
	MessageContainer(QWidget* parent, QString content, int userid, QString username);
	//�յ����˵���Ϣ�Ĺ���
	MessageContainer(QWidget* parent, QString timestamp, QString content, int userid, QString username, int recordid, bool mine); 
	~MessageContainer();
	//������Ϣ
	void setInfo(QString timestamp, int recordid);
	//�����Ƿ���ʱ���
	bool hasTime();
	//��ȡ��Ϣ����
	QString getContent();

	int recordid = 0;
	int userid = 0;

private:
	QLabel* nameTag;
	QLabel* contentTag;
	QLabel* timestampTag;
	QLabel* iconTag;
	QLabel* waitLabel;
	QMovie* waitMovie;
	QImage* img;
	QImage* imgscaled;

	QVBoxLayout* layout;//��
	QHBoxLayout* layout1;//��һ��
	QHBoxLayout* layout2;//�ڶ���

	//��ʱ��ʽ
	QString styleSheetTemp;

	void loadStyleSheet(const QString &sheetName);
};

