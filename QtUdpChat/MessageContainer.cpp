#include "MessageContainer.h"

//本人发信息的构造
MessageContainer::MessageContainer(QWidget* parent, QString content, int userid, QString username)
{
	layout = new QVBoxLayout(this);
	layout1 = new QHBoxLayout;
	layout2 = new QHBoxLayout;
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	//靠右
	layout1->setAlignment(Qt::AlignRight);
	layout2->setAlignment(Qt::AlignRight);
	//初始化tag
	nameTag = new QLabel;
	contentTag = new QLabel;
	timestampTag = new QLabel;
	iconTag = new QLabel;

	//设置timestampTag
	timestampTag->setObjectName("TimestampTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_font_ratio));
	styleSheetTemp.append("px;margin-right:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_margin));
	styleSheetTemp.append("px;");
	timestampTag->setStyleSheet(styleSheetTemp);
	timestampTag->setText("");
	timestampTag->setVisible(false);
	//设置nameTag
	nameTag->setObjectName("NameTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::nametag_font_ratio));
	styleSheetTemp.append("px;margin-right:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::nametag_margin));
	styleSheetTemp.append("px;");
	nameTag->setStyleSheet(styleSheetTemp);
	nameTag->setText(username);
	//layout1
	layout1->addWidget(timestampTag);
	layout1->addWidget(nameTag);

	//设置waitLabel
	waitLabel = new QLabel;
	waitMovie = new QMovie(":/test/resources/wait2.gif");
	waitMovie->setScaledSize(QSize(Config::screenWidth / Config::waitmovie_ratio, Config::screenWidth / Config::waitmovie_ratio));
	waitLabel->setMovie(waitMovie);
	waitMovie->start();

	//设置contentTag
	contentTag->setObjectName("ContentTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::content_font_ratio));
	styleSheetTemp.append("px;margin-right:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::content_margin));
	styleSheetTemp.append("px;");
	contentTag->setStyleSheet(styleSheetTemp);
	contentTag->setText(content);
	contentTag->setMaximumWidth(Config::screenWidth / Config::content_width_ratio);
	contentTag->setWordWrap(true);

	//设置iconTag
	iconTag->setObjectName("IconTag");
	img = new QImage(":/test/resources/默认头像ICON.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(Config::screenWidth / Config::img_ratio, Config::screenWidth / Config::img_ratio, Qt::KeepAspectRatio);
	iconTag->setPixmap(QPixmap::fromImage(*imgscaled));

	//layout2
	layout2->addWidget(waitLabel);
	layout2->addWidget(contentTag);
	layout2->addWidget(iconTag);

	loadStyleSheet("MessageContainer");
}

//收到别人的信息的构造
MessageContainer::MessageContainer(QWidget* parent, QString timestamp, QString content, int userid, QString username, int recordid, bool mine)
{
	//没有等待提示
	waitLabel = nullptr;
	waitMovie = nullptr;
	//
	layout = new QVBoxLayout(this);
	layout1 = new QHBoxLayout;
	layout2 = new QHBoxLayout;
	layout->addLayout(layout1);
	layout->addLayout(layout2);

	//如果是自己发的
	if (mine) {
		//靠右
		layout1->setAlignment(Qt::AlignRight);
		layout2->setAlignment(Qt::AlignRight);
	}
	else {
		//靠左
		layout1->setAlignment(Qt::AlignLeft);
		layout2->setAlignment(Qt::AlignLeft);
	}
	//初始化tag
	nameTag = new QLabel;
	contentTag = new QLabel;
	timestampTag = new QLabel;
	iconTag = new QLabel;

	//设置timestampTag
	timestampTag->setObjectName("TimestampTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_font_ratio));
	//如果是自己发的
	if (mine) {
		styleSheetTemp.append("px;margin-right:");
	}
	else {
		styleSheetTemp.append("px;margin-left:");
	}
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_margin));
	styleSheetTemp.append("px;");
	timestampTag->setStyleSheet(styleSheetTemp);
	timestampTag->setText(timestamp);
	//设置nameTag
	nameTag->setObjectName("NameTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::nametag_font_ratio));
	//如果是自己发的
	if (mine) {
		styleSheetTemp.append("px;margin-right:");
	}
	else {
		styleSheetTemp.append("px;margin-left:");
	}
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::nametag_margin));
	styleSheetTemp.append("px;");
	nameTag->setStyleSheet(styleSheetTemp);
	nameTag->setText(username);
	//layout1
	//如果是自己发的
	if (mine) {
		layout1->addWidget(timestampTag);
		layout1->addWidget(nameTag);
	}
	else {
		layout1->addWidget(nameTag);
		layout1->addWidget(timestampTag);
	}

	//设置contentTag
	contentTag->setObjectName("ContentTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::content_font_ratio));
	//如果是自己发的
	if (mine) {
		styleSheetTemp.append("px;margin-right:");
	}
	else {
		styleSheetTemp.append("px;margin-left:");
	}
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::content_margin));
	styleSheetTemp.append("px;");
	contentTag->setStyleSheet(styleSheetTemp);
	contentTag->setText(content);
	contentTag->setMaximumWidth(Config::screenWidth / Config::content_width_ratio);
	contentTag->setWordWrap(true);

	//设置iconTag
	iconTag->setObjectName("IconTag");
	img = new QImage(":/test/resources/默认头像ICON.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(Config::screenWidth / Config::img_ratio, Config::screenWidth / Config::img_ratio, Qt::KeepAspectRatio);
	iconTag->setPixmap(QPixmap::fromImage(*imgscaled));

	//layout2
	//如果是自己发的
	if (mine) {
		layout2->addWidget(contentTag);
		layout2->addWidget(iconTag);
	}
	else {
		layout2->addWidget(iconTag);
		layout2->addWidget(contentTag);
	}
	loadStyleSheet("MessageContainer");
}


MessageContainer::~MessageContainer()
{

}

//设置属性
void MessageContainer::setInfo(QString timestamp, int recordid) {
	timestampTag->setVisible(true);
	timestampTag->setText(timestamp);
	this->recordid = recordid;
	//隐藏等待图标
	waitMovie->stop();
	waitLabel->setVisible(false);
}

void MessageContainer::loadStyleSheet(const QString &sheetName) {
	QFile file(":/test/" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

bool MessageContainer::hasTime() {
	if (timestampTag == nullptr || timestampTag->text() == "") {
		return false;
	}
	else {
		return true;
	}
}

QString MessageContainer::getContent() {
	return contentTag->text();
}