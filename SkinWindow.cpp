#include "SkinWindow.h"
#include "NotifyManager.h"
#include "QClickLabel.h"


SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	loadStyleSheet("SkinWindow");  // ����SkinWindow����ʽ��
	setAttribute(Qt::WA_DeleteOnClose);   // �������ԣ��������յ��ر��¼���ʱ��ͻ���йر�
	initControl();
}

SkinWindow::~SkinWindow()
{}

/*
  ��ʼ���ؼ�
*/
void SkinWindow::initControl()
{
	// �����ǿ���ѡ���Ƥ����ɫ������QColor��
	QList<QColor> colorList = {  // 3��4����ɫ
		QColor(22,154,218),QColor(40,138,221), QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),  QColor(89,92,160), QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77), QColor(240,188,189),
	};

	// ��ʼ���ؼ� -- 3��4�еı�ǩ(��ǩ�д�Ŷ����ɫ)
	// �˴��ı�ǩ����Ҫ���֮�������Ӧ��Ȼ�������ɫ���޸ģ�����������ͨ�ı�ǩ��û��
	// �취���е����Ӧ�ģ���ʱ��������Ҫ��дһ��QLabel���࣬�����ܹ���Ӧ���
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 4; col++) {
			QClickLabel* label = new QClickLabel(this);   // ����һ�����Ա�����ı�ǩ
			label->setCursor(Qt::PointingHandCursor);     // ������ֵ���״
			// ���֮�����źţ������ź����
			// �����ǵ����ǩ��ѡ����Ӧ�ı�ǩ��ʱ�򣬶�Ӧ�Ĵ��嶼Ӧ�øı���ɫ������
			// ���ǵ��֮�����еĴ��嶼Ӧ���յ��ź�

			// ʹ��lambda���ʽ��ֱ��д�ۺ���,ͬʱʵ�ַŸ��������
			connect(label, &QClickLabel::clicked, [row, col, colorList]() {
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + col) // �û����������ɫ
				);
			});

			label->setFixedSize(84, 84);     // �ƶ���ǩ��С

			//���ñ�ǩ����ɫ
			QPalette palette;
			palette.setColor(QPalette::Background, colorList.at(row * 4 + col));
			label->setAutoFillBackground(true);  // �Զ���䱳����ɫ
			label->setPalette(palette);

			// ��ӵ����ֹ���������ȥ
			ui.gridLayout->addWidget(label, row, col);  // ��դ��Ķ�Ӧλ�����label
		}
	}

	// ���ӵ��Ƥ�����ڵİ�ťʵ��Ч���������ź���۵�����
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

void SkinWindow::onShowClose() {
	// ֱ�ӵ��ô��ڵ�close���йر�
	close();
}

