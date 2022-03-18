#include "ChatRoom.h"

//�Զ�����ʱ����
void ChatRoom::Sleep(int msec) {
	QTime dieTime = QTime::currentTime().addMSecs(msec);
	while (QTime::currentTime() < dieTime) {
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
}

ChatRoom::ChatRoom(QWidget *parent, UdpChatService* udpChatService) : QWidget(parent)
{ 
	this->udpChatService = udpChatService;

	//ע��QVariant
	qRegisterMetaType<QVariant>("QVariant");

	//�󶨻ص�����
	chatEmiter = std::bind(&ChatRoom::signalEmiterCallback, this, _1, _2);
	chatArgsEmiter = std::bind(&ChatRoom::signalArgsEmiterCallback, this, _1, _2);

	buffer = new char[Config::buffer_size];
	memset(buffer, 0, Config::buffer_size);

	//���崰�ڴ�С
	this->resize(Config::screenWidth / 2, Config::screenHeight / 2);
	
	//ȥ��������
	//������С��ʱ��������������ڿ�����ʾ��ԭ����
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	//���ô��ڱ���͸��
	setAttribute(Qt::WA_TranslucentBackground);

	myBorder = new MyBorderContainer(this);
	myBorder->setMinWindowSize(Config::screenWidth / Config::window_ratio, Config::screenWidth / Config::window_ratio);

	m_titleBar = new MyTitleBar(this, MIN_MAX_BUTTON, "QtUdpChat", ":/test/resources/chat.png", QSize(Config::screenWidth / Config::icon_ratio, Config::screenWidth / Config::icon_ratio));
	m_titleBar->setTitleContent("���촰", Config::screenWidth / Config::font_ratio);

	//������󻯺ͻ�ԭ��ť����
	connect(m_titleBar, &MyTitleBar::signalButtonMaxClicked, this, &ChatRoom::onButtonMaxClicked);
	connect(m_titleBar, &MyTitleBar::signalButtonRestoreClicked, this, &ChatRoom::onButtonRestoreClicked);

	//������ʱ��ʽ
	styleSheetTemp = "QPushButton{font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_width_ratio));
	styleSheetTemp.append("px;height:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_height_ratio));
	styleSheetTemp.append("px;border-radius:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::border_radius_ratio));
	styleSheetTemp.append("px;}");

	//�ǳ���ť
	button_signout = new QPushButton(this);
	button_signout->setText("�˳�����¼ҳ");
	button_signout->setObjectName("ButtonSignout");
	button_signout->setStyleSheet(styleSheetTemp);

	//������ʱ��ʽ
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;width:");
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::button_width_ratio / 2));
	styleSheetTemp.append("px;");

	//���Ͱ�ť
	button_send = new QPushButton(this);
	button_send->setText("����");
	button_send->setObjectName("ButtonSend");
	button_send->setStyleSheet(styleSheetTemp);

	//�ı��༭��
	styleSheetTemp = "font-size:";
	styleSheetTemp.append(QString::number(Config::screenWidth / Config::font_ratio));
	styleSheetTemp.append("px;");
	edit_text = new QTextEdit(this);
	edit_text->setObjectName("ButtonSend");
	edit_text->setStyleSheet(styleSheetTemp);
	edit_text->setMaximumHeight(Config::screenHeight / 6);
	edit_text->setMinimumHeight(Config::screenHeight / 6);
	edit_text->setMinimumWidth(Config::screenWidth / 6);

	//������
	scroll_left = new QScrollArea(this);
	

	//�Ҳ����
	widget_right = new QWidget;
	widget_right->setContentsMargins(0, 0, 0, 0);
	layout_right_in = new QVBoxLayout(widget_right);
	layout_right_in->setMargin(0);
	button_getRecords = new QPushButton;
	button_getRecords->setText("��ȡ�����¼");
	layout_right_in->addWidget(button_getRecords, 0, Qt::AlignCenter);
	layout_right_in->addStretch();

	scroll_right = new QScrollArea;
	scroll_right->setStyleSheet("border:2px solid rgb(40,222,235);");
	scroll_right->setMinimumHeight(Config::screenHeight / 6);
	scroll_right->setWidgetResizable(true);
	scroll_right->setWidget(widget_right);

	//��������
	layout_all = new QHBoxLayout(this);
	layout_all->setContentsMargins(Config::screenWidth / Config::title_ratio, Config::screenWidth / Config::title_ratio + Config::screenWidth / Config::icon_ratio, Config::screenWidth / Config::title_ratio, Config::screenWidth / Config::title_ratio);

	layout_left = new QWidget(this);
	layout_left->setMinimumWidth(Config::screenWidth / 8);
	layout_left->setMaximumWidth(Config::screenWidth / 8);
	layout_left->setStyleSheet(styleSheetTemp);

	layout_left_in = new QVBoxLayout(layout_left);
	layout_left_in->setMargin(0);
	layout_left_in->addWidget(scroll_left, 0);
	layout_left_in->addWidget(button_signout, 0);

	layout_right = new QVBoxLayout(this);
	layout_right->setMargin(0);
	layout_right->addWidget(scroll_right);
	layout_right->addWidget(edit_text);
	layout_right->addWidget(button_send, 0, Qt::AlignRight);

	layout_all->addWidget(layout_left, 0);
	layout_all->addLayout(layout_right, 0);

	//����qss
	loadStyleSheet("ChatRoom");

	//���Ͱ�ť��
	connect(button_send, &QPushButton::clicked, this, &ChatRoom::onButtonSendClicked);
	//��ȡ�����¼��ť��
	connect(button_getRecords, &QPushButton::clicked, this, &ChatRoom::getRecords);

	//�����źŲ۰�
	connect(this, &ChatRoom::doPostrecordAckSig, this, &ChatRoom::doPostrecordAck, Qt::QueuedConnection);
	connect(this, &ChatRoom::doPostrecordsAckSig, this, &ChatRoom::doPostrecordsAck, Qt::QueuedConnection);
}


ChatRoom::~ChatRoom()
{
}

void ChatRoom::paintEvent(QPaintEvent* event) {
	//���ñ���ɫ
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);
	pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.fillPath(pathBack, QBrush(QColor(23, 58, 95)));

	QWidget::paintEvent(event);
}

void ChatRoom::loadStyleSheet(const QString &sheetName) {
	QFile file(":/test/" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

void ChatRoom::onButtonMaxClicked() {
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(FactRect);
	myBorder->DrawBorder();
}

void ChatRoom::onButtonRestoreClicked() {
	myBorder->DrawBorder();
}

void ChatRoom::onButtonSendClicked() {
	QString test = edit_text->toPlainText();

	if (test.length() >= 1) {
		//��������
		//����vector
		vector<char*> args;
		//����1
		char arg1[2];
		arg1[0] = POST_RECORD;
		arg1[1] = 0;
		args.push_back(arg1);
		//����2.3
		unsigned short id_cur = 1, id_all = 1;
		char arg2[2];
		arg2[0] = id_cur;
		arg2[1] = 0;
		args.push_back(arg2);
		char arg3[2];
		arg3[0] = id_all;
		arg3[1] = 0;
		args.push_back(arg3);

		QString roomidString = QString::number(roomid);
		QString useridString = QString::number(userid);
		QString contentString = test;
		//����4
		QByteArray temp1;
		temp1.append(roomidString);
		char* roomidChar = temp1.data();
		args.push_back(roomidChar);
		//����5
		QByteArray temp2;
		temp2.append(useridString);
		char* useridChar = temp2.data();
		args.push_back(useridChar);
		//����6
		QByteArray temp3;
		temp3.append(contentString);
		char* contentChar = temp3.data();
		args.push_back(contentChar);

		//���η���
		udpChatService->s_PostRequest(addr, args);

		//ǰ��չʾ
		edit_text->setText("");
		qDebug() << test << endl;
		QString contentQString(contentChar);
		MessageContainer* messageContainer = new MessageContainer(this, contentQString, atoi(useridChar), userName);
		messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout_right_in->addWidget(messageContainer, 0, Qt::AlignRight);
		//messageָ�����list
		messageContainerList.push_back(messageContainer);
		//�ӳ�50����ȴ�����widget���
		Sleep(50);
		//������������
		QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
		vScrollBar->setValue(vScrollBar->maximum());
	}
}

void ChatRoom::setRoomValues(int roomid, int userid, QString userName, char* addr) {
	this->roomid = roomid;
	this->userid = userid;
	this->userName = userName;
	this->addr = new char[16];
	memset(this->addr, 0, 16);
	memcpy(this->addr, addr, 16);
}

//�����յ�һ����Ϣ
void ChatRoom::doPostrecordAck(vector<char*>* vv) {
	vector<char*> v = *vv;
	if (v.size() != 6) {
		qDebug() << "���յ���������!!!" << endl;
	}
	QString useridQString(v[0]);
	QString contentQString(v[1]);
	QString timestampQString(v[2]);
	QString usernameQString(v[3]);
	QString idrecordsQString(v[4]);
	QString roomidQString(v[5]);//roomid��ʱû��
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();

	int idrecordsInt = idrecordsQString.toInt();
	int useridInt = useridQString.toInt();

	//������Լ�����
	if (userid == useridInt) {
		list<MessageContainer*>::reverse_iterator iter;
		for (iter = messageContainerList.rbegin(); iter != messageContainerList.rend(); iter++) {
			MessageContainer* temp = *iter;
			if (!temp->hasTime() && temp->getContent() == contentQString) {
				temp->setInfo(timestampQString, idrecordsInt);
				break;
			}
		}
		//����earliestID
		if (earliestID == 0 || idrecordsInt < earliestID) {
			earliestID = idrecordsInt;
		}
	}
	else {
		//ǰ��չʾ
		MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, useridInt, usernameQString, idrecordsInt, false);
		messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		layout_right_in->addWidget(messageContainer, 0, Qt::AlignLeft);
		//messageָ�����list
		messageContainerList.push_back(messageContainer);
		//�ӳ�50����ȴ�����widget���
		Sleep(50);
		//������������
		QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
		vScrollBar->setValue(vScrollBar->maximum());
	}
	
}

//�����յ�����Ϣ��¼
void ChatRoom::doPostrecordsAck(vector<char*>* vv) {
	vector<char*> v = *vv;
	if (v.size() != 6) {
		qDebug() << "���յ���������!!!" << endl;
	}
	QString useridQString(v[0]);
	QString contentQString(v[1]);
	QString timestampQString(v[2]);
	QString usernameQString(v[3]);
	QString idrecordsQString(v[4]);
	QString roomidQString(v[5]);//roomid��ʱû��
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();

	int idrecordsInt = idrecordsQString.toInt();
	int useridInt = useridQString.toInt();

	//��ȡ������¼�Ժ󣬽���һ����Ϣ��ǰ����ʾ
	bool mine = false;
	if (this->userid == useridInt) mine = true;
	MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, useridInt, usernameQString, idrecordsInt, mine);
	messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	//������Լ�����
	if (this->userid == useridInt) {
		layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignRight);
	}
	else {
		layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignLeft);
	}
	//messageָ�����list
	messageContainerList.push_front(messageContainer);

	//����earliestID
	if (earliestID == 0 || idrecordsInt < earliestID) {
		earliestID = idrecordsInt;
	}
}

void ChatRoom::getRecords() {
	//��ȡ���������¼
	//����vector
	vector<char*> args;
	//����1
	char arg1[2];
	arg1[0] = GET_RECORD;
	arg1[1] = 0;
	args.push_back(arg1);
	//����2.3
	unsigned short id_cur = 1, id_all = 1;
	char arg2[2];
	arg2[0] = id_cur;
	arg2[1] = 0;
	args.push_back(arg2);
	char arg3[2];
	arg3[0] = id_all;
	arg3[1] = 0;
	args.push_back(arg3);

	QString roomidString = QString::number(roomid);
	QString idrecordsString = QString::number(earliestID);
	//����4
	QByteArray temp1;
	temp1.append(roomidString);
	char* roomidChar = temp1.data();
	args.push_back(roomidChar);

	//����5
	QByteArray temp2;
	temp2.append(idrecordsString);
	char* idrecordsChar = temp2.data();
	args.push_back(idrecordsChar);

	//���η���
	udpChatService->s_PostRequest(addr, args);
}

//�ص��������źŷ���
void ChatRoom::signalEmiterCallback(int type, char* content) {

}

//��vector�����ص�����
void ChatRoom::signalArgsEmiterCallback(int type, vector<char*>* args) {
	QVector<char*> v;

	switch (type) {
	case 0:
		//�յ�һ�������¼
		emit doPostrecordAckSig(args);
		break;
	case 1:
		//�յ�һ�������¼
		emit doPostrecordsAckSig(args);
		break;
	}
}

void ChatRoom::setUdpChatService(UdpChatService* u) {
	udpChatService = u;
}