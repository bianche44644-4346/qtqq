#include "QMsgTextEdit.h"
#include <QMovie>
#include <QUrl>

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{

}

QMsgTextEdit::~QMsgTextEdit()
{
	deleteAllEmotionImage();     // 调用自己的接口进行删除
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	// 也就是对应的路径
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);    // 保存一下图片的名字
	const QString& flagName = QString("%1").arg(imageName);
	insertHtml(QString("<img src='%1' />").arg(flagName));    // 插入网页

	// 判断表情在映射中是否存在，如果存在那么就不做操作，如果不存在就进行添加
	if (m_listEmotionUrl.contains(imageName)) {
		return;
	}
	else {
		m_listEmotionUrl.append(imageName);
	}

	QMovie* apng = new QMovie(imageName, "apng", this);
	m_emotionMap.insert(apng, flagName);                  // 添加到表情映射当中去

	// 动图的帧发生改变的时候，执行一下槽函数
	// 数据帧改变的时候发射的信号
	connect(apng, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
	// 让动图动起来
	apng->start();
	update();            // 更新一下几何形状
}

void QMsgTextEdit::deleteAllEmotionImage()
{
	// 释放map中所有选中(构造)的表情，将其释放掉，然后将映射也进行一个清空
	for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); itor++) {
		// 直接删除掉
		delete itor.key();  // 此处只是将对应的内存进行了释放，并没有将映射中的数据也删除
	}
	m_emotionMap.clear();
}

void QMsgTextEdit::onEmotionImageFrameChange(int frame) {
	// 图片动画改变的时候进行响应
	QMovie* movie = qobject_cast<QMovie*>(sender());       // sender()是返回信号的发送者
	// 获取当前的文档,添加资源
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}