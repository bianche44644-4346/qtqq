#include "BasicWindow.h"
#include "CommonUtils.h"
#include "NotifyManager.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QSqlQuery>
#include <QStyleOption>

extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget* parent)
	: QDialog(parent) {
	m_colorBackGround = CommonUtils::getDefaultSkinColor();
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));
}

BasicWindow::~BasicWindow() {}


void BasicWindow::onSignalSkinChanged(const QColor& color) {
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}

void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon) {
	_titleBar->setTitleIcon(icon);
	_titleBar->setTitleContent(title);
}

void BasicWindow::initTitleBar(ButtonType buttontype) {
	_titleBar = new TitleBar(this);
	_titleBar->setButtonType(buttontype);
	_titleBar->move(0, 0);

	connect(_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

void BasicWindow::loadStyleSheet(const QString& sheetName) {
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);

	if (file.isOpen()) {
		setStyleSheet("");//���û����������ʽ����ָ���κ���ʽ
		QString qsstyleSheet = QLatin1String(file.readAll());

		//��ȡ�û���ǰƤ����RGBֵ
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		// widget�����������ֶ�Ӧ�ռ䣬���ҿ��Ը���ָ����̬���ԣ�Ȼ��ͨ����widget��
		// ��̬����������ʽ����������ʽ��

		// �Զ�̬���Ե���ʽ�����������ڴ�����ʵ�֣���û��д�뵽css�ļ�
		// ������ͨ�������ǶԶ�̬���ԵĴ���ֱ��ƴ�ӵ���ȡ����css�ļ��ĺ��棬�ڴ����н���
		// ƴ�Ӳ�û��д��css�ļ��С�

		// ��QWidget������titleskin����������Ϊtrue��ʱ��Ż�ȥ���ú���ָ������ʽ������Ҳһ��
		qsstyleSheet += QString("QWidget[titleskin=true]\
								{background-color:rgb(%1,%2,%3);\
								border-top-left-radius:4px;}\
								QWidget[bottomskin=true]\
								{border-top:1px solid rgba(%1,%2,%3,100);\
								background-color:rgba(%1,%2,%3,50);\
								border-bottom-left-radius:4px;\
								border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);		//���ø�ʽ
		setStyleSheet(qsstyleSheet);	//Ӧ�õ���ǰ����
	}

	file.close();
}

void BasicWindow::initBackGroundColor() {
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BasicWindow::paintEvent(QPaintEvent* event) {
	initBackGroundColor();
	QDialog::paintEvent(event);
}

// src����ԭ��ͼƬ(Ů������qq���)��mask��һ���հ׵�ͼƬ(Բ�ε�ͼƬ����Ϊԭ��ͼƬ����)
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize) {
	//����ͼƬ����Ӧ�ߴ�
	if (maskSize == QSize(0, 0)) {
		//�������ͼƬ�ߴ�Ϊ0��������maskͼƬ�ĳߴ�Ϊ���ִ�С
		maskSize = mask.size();
	}
	else {
		//������������ͼƬ��maskSize��С
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//����ת�����ͼ��
	// ������������ͼƬ����֮��Ľ��  
	// ��һ������: ����ͷ��ı�ǩ�Ĵ�С(Ҳ����ͼƬ�Ĵ�С)
	// �ڶ�������: ָ��ͼƬ�ĸ�ʽ: 32λ(RGBAÿһռһ���ֽ�(0-255),������4���ֽ�)
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	// painter��������ڽ�����ͼƬ�Ͻ��л��ƣ���ʱ���ͼƬ��ʲôҲû��
	QPainter painter(&resultImage);
	// ����:  ѡ���ģʽ��ʾ: ���ģʽ���ǻ��һ��Ƶ�ʱ��ὫԭͼƬ�����Ļ��Ƴ���
	painter.setCompositionMode(QPainter::CompositionMode_Source);	//����ͼƬ����ģʽ
	// �����ڽ��ͼ(��ʱû��ͼ)�ϻ��ƣ������ͼ�ľ�������Ϊ͸��
	painter.fillRect(resultImage.rect(), Qt::transparent);
	// ����:  ѡ���ģʽ��ʾ:  ��ʱ���һ��Ƶ�ģʽ����(���������ͼƬ�������ֻ��Ʒ�ʽ)
	// ��Ŀ��ͼƬ��ԴͼƬ�ŵ�һ����������ص�����ôʹ��ԭͼƬ����һ����
	// �������ģʽ֮��
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	// ����������ĸ�ʽ֮�󣬽�mask������resultImage�ϣ�resultImage����͸����
	// ���Ա�ʾʲôҲû�У����Ե���mask֮����ôresultImage�ͱ�����м��ǿհ�
	// ��Բ��(mask��������)
	painter.drawPixmap(0, 0, mask);
	// ����: �������ģʽ: ���ģʽ�£����һ�������ͷ���غ϶���ֻ��ʾ�غϵ�
	// ���֣���������ʾԴͷ����ǲ���
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	// ��ʱ��Ů���ӵ�ͼƬ���ƽ�ȥ��ʹ��sourceIn��ģʽ���ƣ�������ֵ������Բ�ε�Ů����ͼƬ
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();

	return QPixmap::fromImage(resultImage);
}

void BasicWindow::onShowClose(bool) {
	close();
}

void BasicWindow::onShowMin(bool) {
	showMinimized();
}

void BasicWindow::onShowHide(bool) {
	hide();
}

void BasicWindow::onShowNormal(bool) {
	show();
	activateWindow();
}


void BasicWindow::onShowQuit(bool) {
	//���µ�¼״̬ -- �����˳���ʱ�򣬵�¼���û�ϵ��������״̬
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();
	QApplication::quit();
}


void BasicWindow::mouseMoveEvent(QMouseEvent* event) {
	if (m_mousePressed && (event->buttons() && Qt::LeftButton)) {
		// e->globalPos()�¼�����ʱ���ȫ�����ꡣ�������Ļ�����Ͻ�(0,0)
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void BasicWindow::mousePressEvent(QMouseEvent* event) {
	//������µ������
	if (event->button() == Qt::LeftButton) {
		// pos()�¼�������ʱ������ڴ������Ͻ�(0,0)��ƫ�ƣ���globaPos()���жԱ�
		m_mousePressed = true;	//��¼����Ѿ�������
		// �����¼���ƫ��ֵ
		m_mousePoint = event->globalPos() - pos();//��¼��ǰ����
		event->accept();     // ��ʾ�Ե�ǰ��갴���¼�֮����н���,���������޷������
	}
}

void BasicWindow::mouseReleaseEvent(QMouseEvent* event) {
	m_mousePressed = false;	//��¼����Ѿ�������
	event->accept();
}

void BasicWindow::onButtonMinClicked() {
	if (Qt::Tool == (windowFlags() & Qt::Tool)) {
		hide();
	}
	else {
		showMinimized();
	}
}

void BasicWindow::onButtonRestoreClicked() {
	QPoint windowPos;
	QSize windowSize;
	_titleBar->getRestoreInfo(windowPos, windowSize);	//��ȡ����֮ǰ����Ϣ
	setGeometry(QRect(windowPos, windowSize));			//��ԭ����
}

void BasicWindow::onButtonMaxClicked() {
	_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));//�ȱ��浱ǰ���ڵ�λ�úͿ��
	QRect desktopRect = QApplication::desktop()->availableGeometry();//��ȡ��Ļ����
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3,
		desktopRect.width() + 6, desktopRect.height() + 6);//��΢�������������С
	setGeometry(factRect);//����ȫ��
}

void BasicWindow::onButtonCloseClicked() {
	close();
}
