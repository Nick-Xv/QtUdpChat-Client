#include "MessageContainer.h"

//���˷���Ϣ�Ĺ���
MessageContainer::MessageContainer(QWidget* parent, QString content, int userid, QString username)
{
	layout = new QVBoxLayout(this);
	layout1 = new QHBoxLayout;
	layout2 = new QHBoxLayout;
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	//����
	layout1->setAlignment(Qt::AlignRight);
	layout2->setAlignment(Qt::AlignRight);
	//��ʼ��tag
	nameTag = new QLabel;
	contentTag = new QLabel;
	timestampTag = new QLabel;
	iconTag = new QLabel;

	//����timestampTag
	timestampTag->setObjectName("TimestampTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_font_ratio));
	styleSheetTemp.append("px;margin-right:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_margin));
	styleSheetTemp.append("px;");
	timestampTag->setStyleSheet(styleSheetTemp);
	timestampTag->setText("");
	timestampTag->setVisible(false);
	//����nameTag
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

	//����waitLabel
	waitLabel = new QLabel;
	waitMovie = new QMovie(":/test/resources/wait2.gif");
	waitMovie->setScaledSize(QSize(Config::screenWidth / Config::waitmovie_ratio, Config::screenWidth / Config::waitmovie_ratio));
	waitLabel->setMovie(waitMovie);
	waitMovie->start();

	//����contentTag
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

	//����iconTag
	iconTag->setObjectName("IconTag");
	img = new QImage(":/test/resources/Ĭ��ͷ��ICON.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(Config::screenWidth / Config::img_ratio, Config::screenWidth / Config::img_ratio, Qt::KeepAspectRatio);
	iconTag->setPixmap(QPixmap::fromImage(*imgscaled));

	//layout2
	layout2->addWidget(waitLabel);
	layout2->addWidget(contentTag);
	layout2->addWidget(iconTag);

	loadStyleSheet("MessageContainer");
}

//�յ����˵���Ϣ�Ĺ���
MessageContainer::MessageContainer(QWidget* parent, QString timestamp, QString content, int userid, QString username, int recordid, bool mine)
{
	//û�еȴ���ʾ
	waitLabel = nullptr;
	waitMovie = nullptr;
	//
	layout = new QVBoxLayout(this);
	layout1 = new QHBoxLayout;
	layout2 = new QHBoxLayout;
	layout->addLayout(layout1);
	layout->addLayout(layout2);

	//������Լ�����
	if (mine) {
		//����
		layout1->setAlignment(Qt::AlignRight);
		layout2->setAlignment(Qt::AlignRight);
	}
	else {
		//����
		layout1->setAlignment(Qt::AlignLeft);
		layout2->setAlignment(Qt::AlignLeft);
	}
	//��ʼ��tag
	nameTag = new QLabel;
	contentTag = new QLabel;
	timestampTag = new QLabel;
	iconTag = new QLabel;

	//����timestampTag
	timestampTag->setObjectName("TimestampTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::timestamp_font_ratio));
	//������Լ�����
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
	//����nameTag
	nameTag->setObjectName("NameTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::nametag_font_ratio));
	//������Լ�����
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
	//������Լ�����
	if (mine) {
		layout1->addWidget(timestampTag);
		layout1->addWidget(nameTag);
	}
	else {
		layout1->addWidget(nameTag);
		layout1->addWidget(timestampTag);
	}

	//����contentTag
	contentTag->setObjectName("ContentTag");
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::content_font_ratio));
	//������Լ�����
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

	//����iconTag
	iconTag->setObjectName("IconTag");
	img = new QImage(":/test/resources/Ĭ��ͷ��ICON.png");
	imgscaled = new QImage();
	*imgscaled = img->scaled(Config::screenWidth / Config::img_ratio, Config::screenWidth / Config::img_ratio, Qt::KeepAspectRatio);
	iconTag->setPixmap(QPixmap::fromImage(*imgscaled));

	//layout2
	//������Լ�����
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

//��������
void MessageContainer::setInfo(QString timestamp, int recordid) {
	timestampTag->setVisible(true);
	timestampTag->setText(timestamp);
	this->recordid = recordid;
	//���صȴ�ͼ��
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