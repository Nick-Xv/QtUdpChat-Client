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
	//本人发信息的构造
	MessageContainer(QWidget* parent, QString content, int userid, QString username);
	//收到别人的信息的构造
	MessageContainer(QWidget* parent, QString timestamp, QString content, int userid, QString username, int recordid, bool mine); 
	~MessageContainer();
	//设置信息
	void setInfo(QString timestamp, int recordid);
	//返回是否有时间戳
	bool hasTime();
	//获取消息内容
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

	QVBoxLayout* layout;//总
	QHBoxLayout* layout1;//第一层
	QHBoxLayout* layout2;//第二层

	//临时样式
	QString styleSheetTemp;

	void loadStyleSheet(const QString &sheetName);
};

