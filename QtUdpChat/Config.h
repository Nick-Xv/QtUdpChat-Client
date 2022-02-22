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

	//��������С
	static const int buffer_size = 8192;

	static int BUTTON_HEIGHT;//��ť�߶�
	static int BUTTON_WIDTH;//��ť���
	static int TITLE_HEIGHT;//�������߶�
	static int screenWidth;//��Ļ���
	static int screenHeight;//��Ļ�߶�
};

