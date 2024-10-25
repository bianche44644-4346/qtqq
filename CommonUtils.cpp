#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>
#include <QDebug>

CommonUtils::CommonUtils() {

}

// 获取圆头像
QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize) {
	// 和之前的代码是类似的
	// 如果大小为0的话，我们就使用指定的大小
	if (maskSize == QSize(0, 0)) {
		maskSize = mask.size();       // 如果为0就使用圆头像的大小
	}
	else {
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);   // 修改画家的模式
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}

// 加载样式表
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName) {
	// :/ 表示代开一个资源文件
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())  // 判断文件是否打开
	{
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}

	file.close();
}

/* 设置皮肤 */
void CommonUtils::setDefaultSkinColor(const QColor& color) {
	// 设置默认的皮肤，需要将配置文件的颜色进行重写
	// 获取配置文件的路径  返回当前应用的路径
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	// 通过QSettings类来操作配置文件，对配置文件进行一个读写操作
	QSettings settings(path, QSettings::IniFormat);

	// 通过传进来的color值进行替换原来的值
	//                 节          键    值
	// 调用对应的函数，获取传入的QColor对象的三原色
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());
}

/*获取默认颜色*/
QColor CommonUtils::getDefaultSkinColor() {
	QColor color;    // 定义返回的颜色

	// 返回默认的颜色值
	// 获取配置文件路径
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");

	// 如果当前路径的文件不存在，那么就默认将颜色设置为天蓝色，因为这个配置文件就是
	// 用来获取颜色的，如果没有的话那么就设置一个默认的
	if (!QFile::exists(path)) {
		setDefaultSkinColor(QColor(22, 154, 218));
	}
	// 使用QSettings来操作配置文件
	QSettings settings(path, QSettings::IniFormat);

	// 将配置文件中对应的颜色设置到需要返回的对象中
	color.setRed(settings.value("DefaultSkin/red").toInt());  // 转换为整数，因为文件读出来的是字符串
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}