#pragma once
class Config
{
public:
	Config();
	~Config();
	//比例越大，显示越小
	//导航栏图标比例
	static const int icon_ratio = 60;
	//按钮比例
	static const int button_width_ratio = 12;
	static const int button_height_ratio = 68;
	//按钮圆角矩形
	static const int border_radius_ratio = 68 * 2;
	//文字比例
	static const int font_ratio = 80;
	//首页图标比例
	static const int img_ratio = 40;
	//最小窗口比例
	static const int window_ratio = 10;
	//导航栏高度
	static const int title_ratio = 250;

	//缓冲区大小
	static const int buffer_size = 8192;

	static int BUTTON_HEIGHT;//按钮高度
	static int BUTTON_WIDTH;//按钮宽度
	static int TITLE_HEIGHT;//标题栏高度
	static int screenWidth;//屏幕宽度
	static int screenHeight;//屏幕高度
};

