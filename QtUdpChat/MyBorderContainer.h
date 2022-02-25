#pragma once
/*
MyBorderContainer
自定义窗口边框组件
*/
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>

enum BorderType {
	NULL_BORDER = 0,//无
	L_BORDER,//左
	R_BORDER,//右
	T_BORDER,//上
	B_BORDER,//下
	LT_BORDER,//左上
	LB_BORDER,//左下
	RT_BORDER,//右上
	RB_BORDER//右下
};

class MyBorderContainer : public QObject
{
	Q_OBJECT
public:
	MyBorderContainer(QWidget* parent,uint16_t minWindowHeight = 50,uint16_t minWindowWidth = 50, uint16_t borderSize=5);
	void setMinWindowSize(uint16_t minWindowWidth, uint16_t minWindowHeight);//
	void setBorderSize(uint16_t borderSize);
	void DrawBorder();//重新绘制

protected:
	void InitBorder();

private:
	void getLeftScaleEvent(QPoint movPoint);
	void getRightScaleEvent(QPoint movPoint);
	void getTopScaleEvent(QPoint movPoint);
	void getBottomScaleEvent(QPoint movPoint);
	void getRBScaleEvent(QPoint movPoint);
	void getRTScaleEvent(QPoint movPoint);
	void getLTScaleEvent(QPoint movPoint);
	void getLBScaleEvent(QPoint movPoint);

	class MyBorder :public QLabel {
	public:
		MyBorder(QWidget* parent, BorderType type, MyBorderContainer* outClass);
	protected:
		void mousePressEvent(QMouseEvent* ev);
		void mouseMoveEvent(QMouseEvent* ev);
		void mouseReleaseEvent(QMouseEvent* ev);
	private:
		MyBorderContainer* container;//保存外部类指针
		BorderType MyType;
		QPoint mousePointOld;
		bool KeepDrag;
	};

	uint16_t borderSize;
	uint16_t minWindowHeight;
	uint16_t minWindowWidth;

	QWidget* m_widget;
	//边框控件
	MyBorder* labelLft;
	MyBorder* labelRit;
	MyBorder* labelBot;
	MyBorder* labelTop;
	MyBorder* labelRB;
	MyBorder* labelRT;
	MyBorder* labelLB;
	MyBorder* labelLT;
};

