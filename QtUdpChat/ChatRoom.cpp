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
	//�յ���Ϣ��
	connect(udpChatService, &UdpChatService::post_record_ack, this, &ChatRoom::doPostrecordAck);
	//�յ���Ϣ��¼��
	connect(udpChatService, &UdpChatService::post_records_ack, this, &ChatRoom::doPostrecordsAck);
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
		memset(buffer, 0, Config::buffer_size);
		buffer[0] = POST_RECORD;
		unsigned short id_cur = 1, id_all = 1;
		memcpy(&buffer[1], &(id_cur), sizeof(id_cur));
		memcpy(&buffer[3], &(id_all), sizeof(id_all));

		QString roomidString = QString::number(roomid);
		QString useridString = QString::number(userid);
		QString contentString = test;
		QByteArray temp1;
		temp1.append(roomidString);
		const char* roomidChar = temp1.data();
		QByteArray temp2;
		temp2.append(useridString);
		const char* useridChar = temp2.data();
		QByteArray temp3;
		temp3.append(contentString);
		const char* contentChar = temp3.data();
		memcpy(&buffer[5], roomidChar, strlen(roomidChar));
		memcpy(&buffer[5 + strlen(roomidChar) + 1], useridChar, strlen(useridChar));
		memcpy(&buffer[5 + strlen(roomidChar) + strlen(useridChar) + 2], contentChar, strlen(contentChar));

		udpChatService->s_PostRequest(addr, buffer);

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

void ChatRoom::doPostrecordAck(char* buffer) {
	//�������ֵ
	char test;
	memcpy(&test, &buffer[1], 1);
	qDebug() << (int)test << endl;

	//��ȡidrecords,roomid,content,userid,timestamp
	int i = 2;
	int beginPtr[6] = { 0 };
	int endPtr[6] = { 0 };
	int num = 0;
	beginPtr[num] = i;
	while (buffer[i] != 0 || buffer[i + 1] != 0) {
		if (buffer[i] == 0) {
			endPtr[num] = i;
			beginPtr[++num] = i + 1;
		}
		i++;
	}
	endPtr[num] = i;

	//���ݲ�����ֱ�ӷ���false
	if (beginPtr[1] == 0) {
		qDebug() << "���յ������ݴ���" << endl;
		return;
	}

	char* idrecordsChar = new char[endPtr[0] - beginPtr[0] + 1];
	char* roomidChar = new char[endPtr[1] - beginPtr[1] + 1];
	char* contentChar = new char[endPtr[2] - beginPtr[2] + 1];
	char* useridChar = new char[endPtr[3] - beginPtr[3] + 1];
	char* timestampChar = new char[endPtr[4] - beginPtr[4] + 1];
	char* usernameChar = new char[endPtr[5] - beginPtr[5] + 1];

	memset(idrecordsChar, 0, endPtr[0] - beginPtr[0] + 1);
	memset(roomidChar, 0, endPtr[1] - beginPtr[1] + 1);
	memset(contentChar, 0, endPtr[2] - beginPtr[2] + 1);
	memset(useridChar, 0, endPtr[3] - beginPtr[3] + 1);
	memset(timestampChar, 0, endPtr[4] - beginPtr[4] + 1);
	memset(usernameChar, 0, endPtr[5] - beginPtr[5] + 1);

	memcpy(idrecordsChar, &buffer[beginPtr[0]], endPtr[0] - beginPtr[0]);
	memcpy(roomidChar, &buffer[beginPtr[1]], endPtr[1] - beginPtr[1]);
	memcpy(contentChar, &buffer[beginPtr[2]], endPtr[2] - beginPtr[2]);
	memcpy(useridChar, &buffer[beginPtr[3]], endPtr[3] - beginPtr[3]);
	memcpy(timestampChar, &buffer[beginPtr[4]], endPtr[4] - beginPtr[4]);
	memcpy(usernameChar, &buffer[beginPtr[5]], endPtr[5] - beginPtr[5]);

	QString useridQString(useridChar);
	QString contentQString(contentChar);
	QString timestampQString(timestampChar);
	QString usernameQString(usernameChar);
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
	//���ݷ���ֵ���в���
	switch ((int)test) { 
	case 0:
		//���ݴ���
		qDebug() << "���յ���������!!!" << endl;
		break;
	case 1:
		//��⵽���˷���һ����Ϣ
		//������Լ�����
		if (userid == atoi(useridChar)) {
			
			list<MessageContainer*>::reverse_iterator iter;
			for (iter = messageContainerList.rbegin(); iter != messageContainerList.rend(); iter++) {
				MessageContainer* temp = *iter;
				if (!temp->hasTime() && temp->getContent() == contentQString) {
					temp->setInfo(timestampQString, atoi(idrecordsChar));
					break;
				}
			}
			//����earliestID
			if (earliestID == 0 || atoi(idrecordsChar) < earliestID) {
				earliestID = atoi(idrecordsChar);
			}

		}
		else {
			//ǰ��չʾ
			MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, atoi(useridChar), usernameQString, atoi(idrecordsChar), false);
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
		break;
	case 2:
		//���ݿ����
		qDebug() << "���ݿ�������" << endl;
		break;
	default:
		//δ֪����ֵ
		qDebug() << "���յ�δ֪�ķ���ֵ" << endl;
		break;
	}
}

//�����յ�����Ϣ��¼
void ChatRoom::doPostrecordsAck(char* buffer) {
	//�������ֵ
	char test;
	memcpy(&test, &buffer[1], 1);
	qDebug() << (int)test << endl;

	//��ȡidrecords,roomid,content,userid,timestamp,username
	int i = 2;
	int beginPtr[120] = { 0 };
	int endPtr[120] = { 0 };
	int num = 0;
	beginPtr[num] = i;
	while (buffer[i] != 0 || buffer[i + 1] != 0) {
		if (buffer[i] == 0) {
			endPtr[num] = i;
			beginPtr[++num] = i + 1;
		}
		i++;
	}
	endPtr[num] = i;

	//û���յ������¼������Ϣ
	if (beginPtr[1] == 0) {
		qDebug() << "���յ��յ������¼" << endl;
		return;
	}

	char *idrecordsChar = nullptr, *roomidChar = nullptr, 
		*contentChar = nullptr, *useridChar = nullptr, 
		*timestampChar = nullptr, *usernameChar = nullptr;

	QString useridQString(useridChar);
	QString contentQString(contentChar);
	QScrollBar* vScrollBar = scroll_right->verticalScrollBar();
	QLabel* testLabel = new QLabel;
	QLabel* nameLabel = new QLabel;
	//���ݷ���ֵ���в���
	switch ((int)test) {
	case 0:
		//���ݴ���
		qDebug() << "���յ���������!!!" << endl;
		break;
	case 1:
		//��ѯ�ɹ�
		for (int i = 0; i <= num; i++) {
			int j = i % 6;
			switch (j) {
			case 0:
				if (idrecordsChar != nullptr) delete idrecordsChar;
				idrecordsChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(idrecordsChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(idrecordsChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 1:
				if (roomidChar != nullptr) delete roomidChar;
				roomidChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(roomidChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(roomidChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 2:
				if (contentChar != nullptr) delete contentChar;
				contentChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(contentChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(contentChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 3:
				if (useridChar != nullptr) delete useridChar;
				useridChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(useridChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(useridChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 4:
				if (timestampChar != nullptr) delete timestampChar;
				timestampChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(timestampChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(timestampChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				break;
			case 5:
				if (usernameChar != nullptr) delete usernameChar;
				usernameChar = new char[endPtr[i] - beginPtr[i] + 1];
				memset(usernameChar, 0, endPtr[i] - beginPtr[i] + 1);
				memcpy(usernameChar, &buffer[beginPtr[i]], endPtr[i] - beginPtr[i]);
				//��ȡ������¼�Ժ󣬽���һ����Ϣ��ǰ����ʾ
				QString contentQString(contentChar);
				QString usernameQString(usernameChar);
				QString timestampQString(timestampChar);
				bool mine = false;
				if (this->userid == atoi(useridChar)) mine = true;
				MessageContainer* messageContainer = new MessageContainer(this, timestampQString, contentQString, atoi(useridChar), usernameQString, atoi(idrecordsChar), mine); 
				messageContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
				//������Լ�����
				if (this->userid == atoi(useridChar)) {
					layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignRight);
				}
				else {
					layout_right_in->insertWidget(2, messageContainer, 0, Qt::AlignLeft);
				}
				//messageָ�����list
				messageContainerList.push_front(messageContainer);

				//����earliestID
				if (earliestID == 0 || atoi(idrecordsChar) < earliestID) {
					earliestID = atoi(idrecordsChar);
				}
				break;
			}
		}
		break;
	case 2:
		//���ݿ����
		qDebug() << "���ݿ��ѯ����" << endl;
		break;
	default:
		//δ֪����ֵ
		qDebug() << "���յ�δ֪�ķ���ֵ" << endl;
		break;
	}
}

void ChatRoom::getRecords() {
	//��ȡ���������¼
	memset(buffer, 0, Config::buffer_size);
	buffer[0] = GET_RECORD;
	unsigned short id_cur = 1, id_all = 1;
	memcpy(&buffer[1], &(id_cur), sizeof(id_cur));
	memcpy(&buffer[3], &(id_all), sizeof(id_all));

	QString roomidString = QString::number(roomid);
	QString idrecordsString = QString::number(earliestID);
	QByteArray temp1;
	temp1.append(roomidString);
	const char* roomidChar = temp1.data();
	QByteArray temp2;
	temp2.append(idrecordsString);
	const char* idrecordsChar = temp2.data();
	memcpy(&buffer[5], roomidChar, strlen(roomidChar));
	memcpy(&buffer[5 + strlen(roomidChar) + 1], idrecordsChar, strlen(idrecordsChar));
	udpChatService->s_PostRequest(addr, buffer);
}