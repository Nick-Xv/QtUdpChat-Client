#include "mytitlebar.h"

MyTitleBar::MyTitleBar(QWidget* parent, ButtonType type, QString title, QString iconPath, QSize iconSize):QWidget(parent),
										m_colorR(153),
										m_colorG(153),
										m_colorB(153),
										m_isPressed(false),
										m_buttonType(MIN_BUTTON),
										m_windowBorderWidth(0),
										m_isTransparent(false){
	//init
	initControl();
	initConnections();
	//load qss
	loadStyleSheet("MyTitle");

	this->setButtonType(type);
	this->setTitleContent(title);
	this->setTitleIcon(iconPath, iconSize);
	this->parent = parent;
}

MyTitleBar::~MyTitleBar() {
	delete m_pButtonClose;
	delete m_pButtonMax;
	delete m_pButtonMin;
	delete m_pButtonRestore;
	delete m_pIcon;
	delete mylayout;
}

//initcontrol
void MyTitleBar::initControl() {
	m_pIcon = new QLabel;
	m_pTitleContent = new QLabel;
	m_pButtonMin = new QPushButton;
	m_pButtonRestore = new QPushButton;
	m_pButtonMax = new QPushButton;
	m_pButtonClose = new QPushButton;

	//设置按钮大小
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH,BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	//设置按钮图片
	QIcon icon1,icon2,icon3,icon4;
	icon1.addFile(tr(":/test/resources/min.png"));
	icon2.addFile(tr(":/test/resources/max.png"));
	icon3.addFile(tr(":/test/resources/restore.png"));
	icon4.addFile(tr(":/test/resources/close.png"));
	m_pButtonMax->setIcon(icon2);
	m_pButtonMin->setIcon(icon1);
	m_pButtonRestore->setIcon(icon3);
	m_pButtonClose->setIcon(icon4);

	//设置对象名，用于qss
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	//设置提示
	m_pButtonMin->setToolTip("最小化");
	m_pButtonRestore->setToolTip("还原");
	m_pButtonMax->setToolTip("最大化");
	m_pButtonClose->setToolTip("关闭");

	mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);
	mylayout->addWidget(m_pTitleContent);
	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5, 0, 0, 0);//边距
	mylayout->setSpacing(0);//间距
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);//伸缩策略
	this->setFixedHeight(TITLE_HEIGHT);//高度
	this->setWindowFlags(Qt::FramelessWindowHint);//无边框
}

//绑定信号槽
void MyTitleBar::initConnections() {
	connect(m_pButtonMin, &QPushButton::clicked, this, &MyTitleBar::onButtonMinClicked);
	connect(m_pButtonMax, &QPushButton::clicked, this, &MyTitleBar::onButtonMaxClicked);
	connect(m_pButtonClose, &QPushButton::clicked, this, &MyTitleBar::onButtonCloseClicked);
	connect(m_pButtonRestore, &QPushButton::clicked, this, &MyTitleBar::onButtonRestoreClicked);
}

//设置标题栏背景色
//在paintEvent事件中绘制
//在构造函数中给出默认值，可以外部设置颜色改变
void MyTitleBar::setBackgroundColor(int r, int g, int b, bool isTransparent) {
	m_colorR = r;
	m_colorG = g;
	m_colorB = b;
	m_isTransparent = isTransparent;
	//重新绘制(调用paintEvent)
	update();
}

//设置标题栏图标
void MyTitleBar::setTitleIcon(QString filePath, QSize IconSize) {
	QPixmap titleIcon(filePath);
	m_pIcon->setPixmap(titleIcon.scaled(IconSize));
	//m_pIcon->setText("!!!!!!");
	//m_pIcon->setStyleSheet("border:2px solid blue;");
}

//设置标题内容
void MyTitleBar::setTitleContent(QString titleContent, int titleFontSize) {
	//设置标题字体
	QFont font = m_pTitleContent->font();
	font.setPixelSize(titleFontSize);
	m_pTitleContent->setFont(font);
	//设置标题内容
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;

	m_pTitleContent->setStyleSheet("color:rgb(225,225,225);margin-left:10;");
}

//设置标题栏长度
void MyTitleBar::setTitleWidth(int width) {
	this->setFixedWidth(width);
}

//设置标题栏按钮类型
void MyTitleBar::setButtonType(ButtonType buttonType) {
	m_buttonType = buttonType;

	switch (buttonType) {
	case MIN_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
	}
	break;
	case MIN_MAX_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
	}
	break;
	case ONLY_CLOSE_BUTTON:
	{
		m_pButtonMin->setVisible(false);
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
	}
	break;
	default:
		break;
}
}

//设置标题滚动
void MyTitleBar::setTitleRoll() {
	connect(&m_titleRollTimer,&QTimer::timeout, this, &MyTitleBar::onRollTitle);
	m_titleRollTimer.start(200);
}

//设置窗口边框宽度
void MyTitleBar::setWindowBorderWidth(int borderWidth) {
	m_windowBorderWidth = borderWidth;
}

//保存窗口最大化前窗口的位置以及大小
void MyTitleBar::saveRestoreInfo(const QPoint point, const QSize size) {
	m_restorePos = point;
	m_restoreSize = size;
	qDebug() << "保存了" << size.width() << endl;
}

//获取窗口最大化前窗口的位置以及大小
void MyTitleBar::getRestoreInfo(QPoint& point, QSize& size) {
	point = m_restorePos;
	size = m_restoreSize;
	qDebug() << point.rx() << point.ry() << endl;
	if (m_pButtonRestore->isVisible() && m_isPressed) {
		point = m_mousePos;
		point.setX(point.rx() - (m_restoreSize.width() / 2));
		qDebug() << point.rx()<<point.ry() << endl;
	}
}

void MyTitleBar::paintEvent(QPaintEvent* event) {
	if (!m_isTransparent) {//是否设置标题透明
		//设置背景色
		QPainter painter(this);
		QPainterPath pathBack;
		pathBack.setFillRule(Qt::WindingFill);//非零弯曲规则
		pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));
		//painter.drawRect(QRect(QPoint(m_pButtonMax->pos()), QPoint(m_pButtonMax->pos().rx()+10, m_pButtonMax->pos().ry() + 10)));
	}

	//同步窗口长度变化
	if (this->width() != (this->parentWidget()->width() - m_windowBorderWidth)) {
		this->setFixedWidth(this->parentWidget()->width() - m_windowBorderWidth);
	}
	QWidget::paintEvent(event);
}

//双击响应，标题栏最大化和还原
void MyTitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
	//只有存在最大化，还原的按钮时双击才有效
	if (m_buttonType == MIN_MAX_BUTTON) {
		//通过最大化按钮的状态判断当前窗口是处于最大化还是原始大小
		//或者通过单独设置变量来表示当前窗口状态
		if (m_pButtonMax->isVisible()) {
			onButtonMaxClicked();
		}
		else {
			onButtonRestoreClicked();
		}
	}
	QWidget::mouseDoubleClickEvent(event);//???
}

//以下通过mousePressEvent,mouseMoveEvent,mouseReleaseEvent三个事件实现了鼠标拖动标题
void MyTitleBar::mousePressEvent(QMouseEvent* event) {
	if (m_buttonType == MIN_MAX_BUTTON) {
		//窗口最大化的时候不能拖动窗口
		if (m_pButtonMax->isVisible()) {
			m_isPressed = true;
			m_startMovePos = event->globalPos();
		}
		else {
			m_isPressed = true;
			m_startMovePos = event->globalPos();
		}
	}
	else {
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
	QWidget::mousePressEvent(event);//???
}

void MyTitleBar::mouseMoveEvent(QMouseEvent* event) {
	if (m_isPressed) {
		if (m_pButtonRestore->isVisible()&&(event->globalPos() - m_startMovePos).manhattanLength()>3) {
			m_mousePos = event->globalPos();
			onButtonRestoreClicked();
		}
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = this->parentWidget()->pos();
		m_startMovePos = event->globalPos();
		this->parentWidget()->move(widgetPos.x() + movePoint.x(),
			widgetPos.y() + movePoint.y());
	}
	QWidget::mouseMoveEvent(event);
}

void MyTitleBar::mouseReleaseEvent(QMouseEvent* event) {
	if (m_isPressed) {
		m_isPressed = false;
	}
	QWidget::mouseReleaseEvent(event);
}

//加载本地样式文件
void MyTitleBar::loadStyleSheet(const QString &sheetName) {
	QFile file(":/test/"+ sheetName+".qss");
	file.open(QFile::ReadOnly);
	if (file.isOpen()) {
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

//按钮响应槽
void MyTitleBar::onButtonMinClicked() {
	parent->showMinimized();
}

void MyTitleBar::onButtonRestoreClicked() {
	QPoint windowPos;
	QSize windowSize;
	this->getRestoreInfo(windowPos, windowSize);
	this->parentWidget()->setGeometry(QRect(windowPos, windowSize));
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void MyTitleBar::onButtonMaxClicked() {
	m_pButtonMax->setVisible(false);
	m_pButtonRestore->setVisible(true);
	saveRestoreInfo(parent->pos(), QSize(parent->width(), parent->height()));
	emit signalButtonMaxClicked();
}

void MyTitleBar::onButtonCloseClicked() {
	parent->close();
}

//标题栏滚动效果
void MyTitleBar::onRollTitle() {
	static int nPos = 0;
	QString titleContent = m_titleContent;
	//当截取的位置比字符串长时，从头开始;
	if (nPos > titleContent.length()) {
		nPos = 0;
	}
	m_pTitleContent->setText(titleContent.mid(nPos));
	nPos++;
}