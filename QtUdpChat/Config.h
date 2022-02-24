#pragma once
class Config
{
public:
	Config();
	~Config();
	//����Խ����ʾԽС
	//������ͼ�����
	static const int icon_ratio = 60;
	//��ť����
	static const int button_width_ratio = 12;
	static const int button_height_ratio = 68;
	//��ťԲ�Ǿ���
	static const int border_radius_ratio = 68 * 2;
	//���ֱ���
	static const int font_ratio = 80;
	//��ҳͼ�����
	static const int img_ratio = 40;
	//��С���ڱ���
	static const int window_ratio = 10;
	//�������߶�
	static const int title_ratio = 250;
	//timestamp���ִ�С
	static const int timestamp_font_ratio = 100;
	//timestamp margin
	static const int timestamp_margin = 68 * 2;
	//nametag margin
	static const int nametag_margin = 68 * 2;
	//nametag ���ִ�С
	static const int nametag_font_ratio = 80;
	//content���ִ�С
	static const int content_font_ratio = 80;
	//content margin
	static const int content_margin = 68 * 2;

	//waitmovie
	static const int waitmovie_ratio = 100;

	//content_width_ratio
	static const int content_width_ratio = 4;

	//��������С
	static const int buffer_size = 8192;

	static int BUTTON_HEIGHT;//��ť�߶�
	static int BUTTON_WIDTH;//��ť���
	static int TITLE_HEIGHT;//�������߶�
	static int screenWidth;//��Ļ���
	static int screenHeight;//��Ļ�߶�
};

