#include "TitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFile>

#define BUTTON_HEIGHT 27                       // ��ť�߶�
#define BUTTON_WIDTH  27					   // ��ť���
#define TITLE_HEIGHT  27                       // �������߶�
/*�����ñ������Ŀ������Ϊ���С����Ϊ�ڿ���ϸ�����������Ӧ*/

/*
   �Զ��������:   �ڹ����������ʱ�����Ǹ�����������������һЩĬ��ֵ
   m_isPressed: false			 Ĭ�ϲ������
   m_buttonType: MIN_MAX_BUTTON  Ĭ��ʹ�õ�ǰ���ְ�ť���
*/
TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
	, m_isPressed(false)
	, m_buttonType(MIN_MAX_BUTTON)
{
	// �ڹ��캯�����У���ʼ���ؼ�
	initControl();
	// ��ʼ���źźͲ۵�����
	initConnections();
	// ������ʽ��  ������ʽ�����������ΪTitle������ʽ��д���ļ����У�ʹ����ʽ��.cssд���ļ���
	// ���Ǵ˴���������Title.css���ļ���ȥ
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{

}

/*
*  ��ʼ���ؼ�
**/
void TitleBar::initControl() {
	m_pIcon = new QLabel(this);            // ��ʼ������ͼ��ؼ�
	m_pTitleContent = new QLabel(this);    // ��ʼ�������ı��ؼ�

	m_pButtonMin = new QPushButton(this);  // ��ʼ����С����ť
	m_pButtonRestore = new QPushButton(this);  // ��ʼ���ָ����ǰ״̬�İ�ť
	m_pButtonMax = new QPushButton(this);      // ��ʼ����󻯰�ť
	m_pButtonClose = new QPushButton(this);    // ��ʼ���رհ�ť

	// ���ú���������ÿ����ť�Ĺ̶���С�����Ǵ���һ��QSize�Ķ������������Ѿ�ʹ�ú궨����
	// ��ť�Ĵ�С�ˣ���������ֱ�Ӵ���Ϳ�����
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	// ����Ӧ�Ķ������ö����������ö�Ӧ�ĺ���
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	// �������������ò���
	QHBoxLayout* myLayout = new QHBoxLayout(this);              // �Ա���������ˮƽ����

	// �ڲ�����������  ˮƽ���֣��������˳������ң���Ȼ������һ���ϵ�
	myLayout->addWidget(m_pIcon);                    // ��������ͼ��
	myLayout->addWidget(m_pTitleContent);            // ֮����ӱ�������
	myLayout->addWidget(m_pButtonMin);
	myLayout->addWidget(m_pButtonRestore);
	myLayout->addWidget(m_pButtonMax);
	myLayout->addWidget(m_pButtonClose);

	//  �����ֱ��������������ҵļ�϶
	myLayout->setContentsMargins(5, 0, 0, 0);         // ����ˮƽ���ֵļ�϶
	myLayout->setSpacing(0);                          // ���ò����ﲿ����ļ�϶

	/* ���ñ������ĳߴ����: ��һ������: �ں����ϣ��˴�����Ϊ����(����������Ӧ)
	*                        �ڶ�������: �������ϣ��˴�����Ϊ�̶�(���������������Ȳ���)
	*/
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);                     // ʹ�����ǹ̶��ĺ꣬�趨����Ĺ̶��߶�
	setWindowFlags(Qt::FramelessWindowHint);          // ���ô������ʽΪ�ޱ߿�ķ��
}

/*
	��ʼ���ź����
*/
void TitleBar::initConnections() {
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));   // �˴���this�ͱ�ʾ���Ǳ�����
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}


/*
  ���ñ�������ͼ��

  ��Ҫ����һ���ַ���ָ����·��������ͼ�����ڵ�·��
*/
void TitleBar::setTitleIcon(const QString& filePath) {
	QPixmap titleIcon(filePath);                                // ����һ��QPixmap���ݴ����ͼ��·������ͼ���С

	m_pIcon->setFixedSize(titleIcon.size());                    // ��������ͼ�����Ĺ̶���С(�����ǸղŴ�����ͼ���СΪ׼)
	m_pIcon->setPixmap(titleIcon);                              // Ȼ�������ָ���ı������ؼ�����ͼ��
}

/*
  ���ñ���������
*/
void TitleBar::setTitleContent(const QString& titleContent) {
	m_pTitleContent->setText(titleContent);                         // �ڶ�Ӧ��λ�����ñ���������
	m_titleContent = titleContent;                                  // ����������ݸ���
}

/*
  ���ñ������ĳ���   -- ���������촰�ڵ�ʱ����±������ĳ���
*/
void TitleBar::setTitleWidth(int width) {
	setFixedWidth(width);                                             // ���ñ������ĳ���
}

/*
  ���ñ�������ť����
*/
void TitleBar::setButtonType(ButtonType buttonType) {
	m_buttonType = buttonType;                                          // �԰�ť�����ͽ��м�¼

	switch (buttonType) {
	case MIN_BUTTON:
	{
		m_pButtonRestore->setVisible(false);                            // ��ť����Ϊ��ֻ����С�ĺ͹رյģ���ô�ͽ��������Ϊ���ɼ��;���
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
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
	}
	break;
	default:
		break;
	}
}

/*
  ���洰�����ǰ�����ڵ�λ�úʹ�С
*/
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size) {
	m_MaxRestorePoint = point;                       // �����ص�λ�ã���¼�����Ƕ���ı�����
	m_MaxRestoreSize = size;
}

/*
  ��ȡ�������ǰ���ڵ�λ���Լ���С
*/
void TitleBar::getRestoreInfo(QPoint& point, QSize& size) {
	point = m_MaxRestorePoint;
	size = m_MaxRestoreSize;
}

/*
  ���Ʊ����� -- ���ڵı����������κεĸı䣬���ᴥ���ػ���(���Ҳһ��)
*/
void TitleBar::paintEvent(QPaintEvent* event) {
	// ���ñ���ɫ
	QPainter painter(this);                                 // ��ȡһ������
	QPainterPath pathBack;                                  // ��ͼ·�����������ñ���
	pathBack.setFillRule(Qt::WindingFill);                  // ����������

	//  ���Բ�Ǿ��ε���ͼ·��    λ��   ��      ��        x��y�Ļ���
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); // ����һ��ƽ����ӳ��ת��

	/*
	   ����������죬������Ҫ�Ա����������������ô�С������ж��Ƿ�������?
	   ͨ���Ƚϵ�ǰ�Ŀ�Ⱥ��丸�����Ŀ�ȱȽϣ��������ȣ���ô�ͽ��б仯��
	   parentWidget()���ظ�����

	   ���Ҵ�����󻯺���С���������仯
	*/
	if (width() != parentWidget()->width()) {
		setFixedWidth(parentWidget()->width());
	}

	QWidget::paintEvent(event);   //�û��ദ��ʣ�����
}

/*
   ˫����Ӧʱ�䣬��Ҫʵ��˫���������������С���Ĳ���
*/
void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {

	// ֻ�д�����󻯣���С����ť����Ч
	if (m_buttonType == MIN_MAX_BUTTON) {
		if (m_pButtonMax->isVisible()) {            // ��󻯿���˵����û�д������״̬��������˵���������״̬
			onButtonMaxClicked();
		}
		else {
			onButtonRestoreClicked();
		}
	}

	return QWidget::mouseDoubleClickEvent(event);     // Ĭ�ϵĴ������
}

/*
  ͨ����갴�£�����ƶ�������ͷ��¼�ʵ���϶��������ﵽ�ƶ�����Ч��
*/
void TitleBar::mousePressEvent(QMouseEvent* event) {
	if (m_buttonType == MIN_MAX_BUTTON) {                  // ����С���ť�������ʱ��
		// �ڴ�����󻯵�״̬�½�ֹ�϶�����
		// ���������������ʱ�򣬾���Ҫ�ƶ����ڣ���ô��Ȼ�ƶ����ڣ���ô��������ʱ����ʵ��û�б�Ҫ�ƶ�
		// ������������ֻ�д�����󻯰�ť����(Ҳ���Ǵ���û�����)���Ż�������갴�£��Ż��к�������
		if (m_pButtonMax->isVisible()) {                   // ��󻯰�ť��������ô���Ǵ������״̬
			m_isPressed = true;
			m_startMovePos = event->globalPos();           //  ������갴���¼�������ʱ��������ڵ�ȫ��λ��
		}
	}
	else {
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
	// ����ֻ�е���갴�µ�ʱ�����ǲ���Ҫ�����ƶ����д���,û�а��²������������
	if (m_isPressed) {
		QPoint movePos = event->globalPos() - m_startMovePos;    // �¼�������ʱ������λ��-��꿪ʼ��λ�ã��Ϳ��Ի�����Ҫ�ƶ�����
		QPoint widgetPos = parentWidget()->pos();                  // ��ȡ������ǰ���ڵ�λ�ã�ͨ�����ظ��������ص�ǰ��λ��
		m_startMovePos = event->globalPos();                     // ���¿�ʼ�ƶ���λ��

		/* �Դ��ڽ����ƶ� �ƶ���x����ͨ����ǰλ�õ�x����+��Ҫ�ƶ���x���꣬y����һ��*/
		parentWidget()->move(widgetPos.x() + movePos.x(), widgetPos.y() + movePos.y());
	}

	// ����Ĭ�ϵ��¼�����
	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
	m_isPressed = false;                          // ����ͷž�û���ٰ�����
	return QWidget::mouseReleaseEvent(event);
}

/*
  ������ʽ��
*/
void TitleBar::loadStyleSheet(const QString& sheetName) {
	// ���е���ʽ����Ϊ�ַ����Ƚ϶࣬�������ǽ���д�뵽�ļ����������Ǵ��ļ����ж�ȡ
	QFile file(":/Resources/QSS/" + sheetName + ".css");    // ƴ��css���ļ���

	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();       // ������������ʽ����б���
		// ���ļ��е���ʽ��ȫ���Ͻ��� ʹ�����������Ŀ�����俪��СһЩ
		styleSheet += QLatin1String(file.readAll());    // һ����ȫ����ȡ�ļ��е����ݣ�Ȼ����ӵ��������ʽ����
		this->setStyleSheet(styleSheet);                // ������ʽ��
	}
}

/*
  ��ť��Ӧ�Ĳۺ���ʵ��
*/
void TitleBar::onButtonMinClicked() {
	emit signalButtonMinClicked();      // ����һ���źţ���С���İ�ť�����
}

void TitleBar::onButtonRestoreClicked() {
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);

	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked() {
	m_pButtonRestore->setVisible(true);
	m_pButtonMax->setVisible(false);

	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked() {
	emit signalButtonCloseClicked();
}
