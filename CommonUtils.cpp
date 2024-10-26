#include "CommonUtils.h"
#include <QPainter>
#include <QFile>
#include <QWidget>
#include <QApplication>
#include <QSettings>
#include <QDebug>

CommonUtils::CommonUtils() {

}

// ��ȡԲͷ��
QPixmap CommonUtils::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize) {
	// ��֮ǰ�Ĵ��������Ƶ�
	// �����СΪ0�Ļ������Ǿ�ʹ��ָ���Ĵ�С
	if (maskSize == QSize(0, 0)) {
		maskSize = mask.size();       // ���Ϊ0��ʹ��Բͷ��Ĵ�С
	}
	else {
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);   // �޸Ļ��ҵ�ģʽ
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}

// ������ʽ��
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& sheetName) {
	// :/ ��ʾ����һ����Դ�ļ�
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())  // �ж��ļ��Ƿ��
	{
		widget->setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());
		widget->setStyleSheet(qsstyleSheet);
	}

	file.close();
}

/* ����Ƥ�� */
void CommonUtils::setDefaultSkinColor(const QColor& color) {
	// ����Ĭ�ϵ�Ƥ������Ҫ�������ļ�����ɫ������д
	// ��ȡ�����ļ���·��  ���ص�ǰӦ�õ�·��
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");
	// ͨ��QSettings�������������ļ����������ļ�����һ����д����
	QSettings settings(path, QSettings::IniFormat);

	// ͨ����������colorֵ�����滻ԭ����ֵ
	//                 ��          ��    ֵ
	// ���ö�Ӧ�ĺ�������ȡ�����QColor�������ԭɫ
	settings.setValue("DefaultSkin/red", color.red());
	settings.setValue("DefaultSkin/green", color.green());
	settings.setValue("DefaultSkin/blue", color.blue());
}

/*��ȡĬ����ɫ*/
QColor CommonUtils::getDefaultSkinColor() {
	QColor color;    // ���巵�ص���ɫ

	// ����Ĭ�ϵ���ɫֵ
	// ��ȡ�����ļ�·��
	const QString&& path = QApplication::applicationDirPath() + "/" + QString("tradeprintinfo.ini");

	// �����ǰ·�����ļ������ڣ���ô��Ĭ�Ͻ���ɫ����Ϊ����ɫ����Ϊ��������ļ�����
	// ������ȡ��ɫ�ģ����û�еĻ���ô������һ��Ĭ�ϵ�
	if (!QFile::exists(path)) {
		setDefaultSkinColor(QColor(22, 154, 218));
	}
	// ʹ��QSettings�����������ļ�
	QSettings settings(path, QSettings::IniFormat);

	// �������ļ��ж�Ӧ����ɫ���õ���Ҫ���صĶ�����
	color.setRed(settings.value("DefaultSkin/red").toInt());  // ת��Ϊ��������Ϊ�ļ������������ַ���
	color.setGreen(settings.value("DefaultSkin/green").toInt());
	color.setBlue(settings.value("DefaultSkin/blue").toInt());

	return color;
}