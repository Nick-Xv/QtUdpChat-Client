#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QPainter>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include "mytitlebar.h"
#include "MyBorderContainer.h"
#include "IocpServer.h"

class QtUdpChat : public QWidget
{
    Q_OBJECT

public:
    QtUdpChat(QWidget *parent = Q_NULLPTR);
	~QtUdpChat();

private:
	QVBoxLayout* layout1;
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

	void paintEvent(QPaintEvent* event);
	void loadStyleSheet(const QString & sheetName);

	MyBorderContainer* myBorder;
	MyTitleBar* m_titleBar;

	IocpServer* iocpServer;

private slots:
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
	void onButtonRegistClicked();
	void onButtonBackClicked();
	void onButtonSendRegistClicked();
};
