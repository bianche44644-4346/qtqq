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
		setStyleSheet("");//调用基类的设置样式表，不指定任何样式
		QString qsstyleSheet = QLatin1String(file.readAll());

		//获取用户当前皮肤的RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		// widget部件用于区分对应空间，并且可以给其指定动态属性，然后通过给widget的
		// 动态属性设置样式来增加其样式。

		// 对动态属性的样式设置我们是在代码中实现，并没有写入到css文件
		// 这里是通过将我们对动态属性的处理直接拼接到读取到的css文件的后面，在代码中进行
		// 拼接并没有写到css文件中。

		// 当QWidget部件的titleskin的属性设置为true的时候才会去设置后面指定的样式，下面也一样
		qsstyleSheet += QString("QWidget[titleskin=true]\
								{background-color:rgb(%1,%2,%3);\
								border-top-left-radius:4px;}\
								QWidget[bottomskin=true]\
								{border-top:1px solid rgba(%1,%2,%3,100);\
								background-color:rgba(%1,%2,%3,50);\
								border-bottom-left-radius:4px;\
								border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);		//设置格式
		setStyleSheet(qsstyleSheet);	//应用到当前窗口
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

// src就是原来图片(女孩或者qq企鹅)，mask是一个空白的图片(圆形的图片，作为原来图片框子)
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize) {
	//遮罩图片自适应尺寸
	if (maskSize == QSize(0, 0)) {
		//如果遮罩图片尺寸为0，就设置mask图片的尺寸为遮罩大小
		maskSize = mask.size();
	}
	else {
		//否则，缩放遮罩图片到maskSize大小
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//保存转换后的图像
	// 用来保存两个图片叠加之后的结果  
	// 第一个参数: 放置头像的标签的大小(也就是图片的大小)
	// 第二个参数: 指定图片的格式: 32位(RGBA每一占一个字节(0-255),所以是4个字节)
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	// painter这个画家在结果这个图片上进行绘制，此时这个图片上什么也没有
	QPainter painter(&resultImage);
	// 参数:  选择的模式表示: 这个模式就是画家绘制的时候会将原图片完整的绘制出来
	painter.setCompositionMode(QPainter::CompositionMode_Source);	//设置图片叠加模式
	// 画家在结果图(此时没有图)上绘制，将结果图的矩形设置为透明
	painter.fillRect(resultImage.rect(), Qt::transparent);
	// 参数:  选择的模式表示:  此时画家绘制的模式就是(如果有两张图片就是这种绘制方式)
	// 将目标图片与源图片放到一起，如果两者重叠，那么使用原图片的那一部分
	// 设置这个模式之后。
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	// 设置了上面的格式之后，将mask绘制在resultImage上，resultImage都是透明的
	// 所以表示什么也没有，所以导入mask之后，那么resultImage就变成了中间是空白
	// 的圆形(mask是这样的)
	painter.drawPixmap(0, 0, mask);
	// 参数: 设置这个模式: 这个模式下，画家会以两个头像重合而且只显示重合的
	// 部分，并且是显示源头像的那部分
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	// 此时将女孩子的图片绘制进去，使用sourceIn的模式绘制，这样就值会留下圆形的女孩子图片
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
	//更新登录状态 -- 程序退出的时候，登录的用户系那是离线状态
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();
	QApplication::quit();
}


void BasicWindow::mouseMoveEvent(QMouseEvent* event) {
	if (m_mousePressed && (event->buttons() && Qt::LeftButton)) {
		// e->globalPos()事件发生时候的全局坐标。相对于屏幕的左上角(0,0)
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

void BasicWindow::mousePressEvent(QMouseEvent* event) {
	//如果按下的是左键
	if (event->button() == Qt::LeftButton) {
		// pos()事件发生的时候相对于窗口左上角(0,0)的偏移，与globaPos()进行对比
		m_mousePressed = true;	//记录鼠标已经被按下
		// 作差记录鼠标偏移值
		m_mousePoint = event->globalPos() - pos();//记录当前坐标
		event->accept();     // 表示对当前鼠标按下事件之后进行接收,否则窗体是无法点击的
	}
}

void BasicWindow::mouseReleaseEvent(QMouseEvent* event) {
	m_mousePressed = false;	//记录鼠标已经被按下
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
	_titleBar->getRestoreInfo(windowPos, windowSize);	//获取窗口之前的信息
	setGeometry(QRect(windowPos, windowSize));			//还原窗口
}

void BasicWindow::onButtonMaxClicked() {
	_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));//先保存当前窗口的位置和宽高
	QRect desktopRect = QApplication::desktop()->availableGeometry();//获取屏幕矩形
	QRect factRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3,
		desktopRect.width() + 6, desktopRect.height() + 6);//稍微调整矩形区域大小
	setGeometry(factRect);//设置全屏
}

void BasicWindow::onButtonCloseClicked() {
	close();
}
