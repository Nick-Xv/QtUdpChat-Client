#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

#include "Config.h"

//按钮种类
enum ButtonType {
	MIN_BUTTON = 0,//最小化和关闭
	MIN_MAX_BUTTON,//全都有
	ONLY_CLOSE_BUTTON//只有关闭
};

class MyTitleBar : public QWidget {
	Q_OBJECT
public:
	MyTitleBar(QWidget* parent, ButtonType type, QString title, QString iconPath, QSize iconSize);
	~MyTitleBar();

	//设置标题栏背景色，以及是否设置标题栏背景色透明
	void setBackgroundColor(int r, int g, int b, bool isTrans = false);
	//设置标题栏图标
	void setTitleIcon(QString filePath, QSize IconSize = QSize(25,25));
	//设置标题内容
	void setTitleContent(QString text, int titleFontSize = 9);
	//设置标题栏长度
	void setTitleWidth(int width);
	//设置标题栏按钮类型
	void setButtonType(ButtonType buttonType);
	//设置标题栏中的标题是否会滚动
	void setTitleRoll();
	//设置窗口边框宽度
	void setWindowBorderWidth(int borderWidth);

	//保存，获取，最大化前窗口的位置以及大小
	void saveRestoreInfo(const QPoint point, const QSize size);
	void getRestoreInfo(QPoint& point, QSize& size);

private:
	void paintEvent(QPaintEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	QHBoxLayout* mylayout;

	//init widget
	void initControl();
	//绑定信号槽
	void initConnections();
	//加载样式文件
	void loadStyleSheet(const QString& sheetName);

signals:
	//按钮触发信号
	void signalButtonMinClicked();
	void signalButtonRestoreClicked();
	void signalButtonMaxClicked();
	void signalButtonCloseClicked();

private slots:
	//按钮触发槽
	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();
	void onRollTitle();

private:
	QLabel* m_pIcon;//标题栏的图标
	QLabel* m_pTitleContent;//标题栏内容
	QPushButton* m_pButtonMin;//最小化按钮
	QPushButton* m_pButtonMax;//最大化按钮
	QPushButton* m_pButtonRestore;//最大化还原按钮
	QPushButton* m_pButtonClose;//关闭按钮

	QWidget* parent;//保存父窗口的指针

	//标题栏背景色
	int m_colorR;
	int m_colorG;
	int m_colorB;

	//最大化，最小化变量
	QPoint m_restorePos;
	QSize m_restoreSize;
	QPoint m_mousePos;

	//移动窗口的变量
	bool m_isPressed;
	QPoint m_startMovePos;

	//标题栏滚动效果时钟
	QTimer m_titleRollTimer;
	//标题栏内容
	QString m_titleContent;

	//按钮类型
	ButtonType  m_buttonType;
	//窗口边框宽度
	int m_windowBorderWidth;
	//标题栏是否透明
	bool m_isTransparent;
};
