#pragma once
/* �Զ�������� */

#include <QWidget>
#include <QLabel>
#include <QPushButton>

/*
   ʹ��ö��ֵ��ʾ�������Լ�����Ĵ��ڵļ��ַ�񣬵�ȻҲ�����Զ�����
*/
enum ButtonType              // ��ť����ģʽ��ö��
{
	MIN_BUTTON = 0,          // ��С���Լ��رհ�ť
	MIN_MAX_BUTTON,          // ��С��������Լ��رհ�ť
	ONLY_CLOSE_BUTTON        // ֻ�йرհ�ť
};


// �Զ��������
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent = Q_NULLPTR);  // Ҫ���ݸ������ָ�룬Ĭ��Ϊ0����ʾ��������˭
	~TitleBar();

	// ���ñ�������ͼ�꣨�������Ͻ��Ǹ�qq�� 
	void setTitleIcon(const QString &filePath);
	// ���ñ�����������
	void setTitleContent(const QString& titleContext);
	// ���ñ������Ŀ��
	void setTitleWidth(int width);
	// ���ñ������İ�ť����
	void setButtonType(ButtonType buttonType);

	// ���洰�����ǰ��λ���Լ���С�������������֮���ٱ��ȥ
	void saveRestoreInfo(const QPoint& point, const QSize& size);
	// ��ȡ���֮ǰ����Ĵ���״̬���ݣ��Ա��ڻָ�
	void getRestoreInfo(QPoint& point, QSize& size);

private:
	void paintEvent(QPaintEvent* event);							// �����ͼ�¼�
	void mouseDoubleClickEvent(QMouseEvent *event);               // ��������˫���¼�
	void mousePressEvent(QMouseEvent* event);                        // ������갴���¼���
	void mouseMoveEvent(QMouseEvent* event);                        // ��������ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);                     // ��������ͷ��¼�

	/*
	   ����������С������󻯣��رյȵȶ��ǰ�ť����ʵ���ǿؼ�����Ҫ��ʼ��
	*/
	void initControl();              // ��ʼ���ؼ�
	void initConnections();          // ��ʼ���ź���۵�����
	void loadStyleSheet(const QString& sheetName);  // ������ʽ��(���ÿؼ���һ�ַ�ʽ)

signals:
	/*
	   ��������ť�������ʱ�򴥷����ź�
	*/
	void signalButtonMinClicked();      // ��С����ť�����
	void signalButtonRestoreClicked();  // ��󻯰�ť��ԭ�����
	void signalButtonMaxClicked();	    // ��󻯰�ť�����
	void signalButtonCloseClicked();    // �رհ�ť�����

private slots:
	/*
	   ��Ӧ�źŵĲ۷���
	*/
	void onButtonMinClicked();
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
	void onButtonCloseClicked();

private:
	/*
	  ����һЩ����������������Ҫ��ʾ�Ŀؼ�
	*/
	// ��ǩ
	QLabel* m_pIcon;           // ����������ͼ��
	QLabel* m_pTitleContent;   // ������������

	// ��ť
	QPushButton* m_pButtonMin;     // ��С����ť
	QPushButton* m_pButtonMax;     // ��󻯰�ť
	QPushButton* m_pButtonRestore; // ��󻯻ָ���ť
	QPushButton* m_pButtonClose;   // �رհ�ť		

	// �������֮ǰ���ڵĵ�λ�úʹ�С����¼���ݷ��㻹ԭ
	QPoint m_MaxRestorePoint;  // ���֮ǰ��λ��                        
	QSize m_MaxRestoreSize;    // ���֮ǰ�Ĵ�С

	bool m_isPressed;          // �����жϱ������Ƿ񱻵��
	QPoint m_startMovePos;     // ��¼��ʼ�ƶ���λ��

	QString m_titleContent;   // ����������
	ButtonType m_buttonType;  // ���������ڵİ�ť����
};
