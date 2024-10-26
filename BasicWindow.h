#pragma once

#include <QDialog>
#include "TitleBar.h"

/* ���������࣬���������Ĵ��ڵĻ��� */
class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = Q_NULLPTR);
	virtual ~BasicWindow();  // ������

public:
	// ������ʽ��
	void loadStyleSheet(const QString& sheetName);

	// ��ȡԲͷ��
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));

private:
	void initBackGroundColor();                 // ��ʼ������

protected:
	void paintEvent(QPaintEvent*event) override; // ��ͼ�¼�
	void mousePressEvent(QMouseEvent* event) override;   // ����ƶ��¼�
	void mouseMoveEvent(QMouseEvent* event) override;    // ����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event) override; // ����ͷ��¼�

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);   // ��ʼ��������������һ����ť�Ĵ�С
	void setTitleBarTitle(const QString& title, const QString& icon = "");  // ���ñ��������ݣ�ָ�������������ֺ�ͼ��(Ĭ��Ϊ��)

public slots:
	void onShowClose(bool);   // ���ڹر�
	void onShowMin(bool);     // ����С��
	void onShowHide(bool);    // ��������
	void onShowNormal(bool);  // ������ʾ
	void onShowQuit(bool);    // �����˳�
	void onSignalSkinChanged(const QColor &color);      // ����Ƥ�������ı��ʱ����в���

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	QPoint m_mousePoint;         // ���λ��
	bool  m_mousePressed;        // ����Ƿ���
	QColor m_colorBackGround;    // ����ɫ
	QString m_styleName;         // ��ʽ�ļ�����
	TitleBar* _titleBar;        // ������������õ��������Զ���ı���������
};
