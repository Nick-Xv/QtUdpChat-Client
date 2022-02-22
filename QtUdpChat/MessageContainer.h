#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QLabel>
class MessageContainer : public QWidget {
	Q_OBJECT
public:
	MessageContainer(QWidget* parent);
	~MessageContainer();

private:
	//QLabel* nameTag;
	//QLabel* contentTag;
};

