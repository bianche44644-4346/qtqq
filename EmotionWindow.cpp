#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "QClickLabel.h"
#include "EmotionLabelItem.h"

#include <QStyleOption>
#include <QPainter>

const int emotionColum = 14;
const int emotionRow = 12;

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	// ���ô��ڷ��
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);          // ���õ��ر����һ�������ʱ����д�����Դ�Ļ���

	ui.setupUi(this);
	initControl();
}

EmotionWindow::~EmotionWindow()
{}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");  // ������ʽ
	// ��ӱ��飬������ÿ��λ�ö�����һ����ǩ����ǩ������ű��飬���ұ�����֧�ֵ����
	// ����������Ҫʹ���Զ��Ե���QClickLabel�����ǩ���������Ƿ���ȥ�ı���Ӧ����һ��
	// ��̬�ģ���ʱ������޷�ʵ���ˣ�������Ҫʹ��QMoive����ʵ�֣����������ٴ��Զ���һ����

	// ���ر���
	for (int row = 0; row < emotionRow; row++) {
		for (int column = 0; column < emotionColum; column++) {
			EmotionLabelItem *label = new EmotionLabelItem(this);  // ���鴰����Ϊ������
			label->setEmotionName(row * emotionColum + column);

			connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			ui.gridLayout->addWidget(label, row, column);    // ��������ӵ�������ȥ
		}
	}
}

void EmotionWindow::addEmotion(int emotionNum) {
	hide();    // ֻ�е����ʱ�����ʾ
	emit signalEmotionWindowHide();   // ������鴰�����ص��ź�
	emit signalEmotionItemClicked(emotionNum);  // ������鱻������ź�
}

void EmotionWindow::paintEvent(QPaintEvent* event) {
	// ��ȡ��ǰ�ķ��
	QStyleOption opt;
	opt.init(this);      //������г�ʼ��

	QPainter painter(this);

	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


	__super::paintEvent(event);   // ���������Ļ�ͼ
}