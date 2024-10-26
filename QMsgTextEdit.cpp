#include "QMsgTextEdit.h"
#include <QMovie>
#include <QUrl>

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{

}

QMsgTextEdit::~QMsgTextEdit()
{
	deleteAllEmotionImage();     // �����Լ��Ľӿڽ���ɾ��
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	// Ҳ���Ƕ�Ӧ��·��
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);    // ����һ��ͼƬ������
	const QString& flagName = QString("%1").arg(imageName);
	insertHtml(QString("<img src='%1' />").arg(flagName));    // ������ҳ

	// �жϱ�����ӳ�����Ƿ���ڣ����������ô�Ͳ�����������������ھͽ������
	if (m_listEmotionUrl.contains(imageName)) {
		return;
	}
	else {
		m_listEmotionUrl.append(imageName);
	}

	QMovie* apng = new QMovie(imageName, "apng", this);
	m_emotionMap.insert(apng, flagName);                  // ��ӵ�����ӳ�䵱��ȥ

	// ��ͼ��֡�����ı��ʱ��ִ��һ�²ۺ���
	// ����֡�ı��ʱ������ź�
	connect(apng, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));
	// �ö�ͼ������
	apng->start();
	update();            // ����һ�¼�����״
}

void QMsgTextEdit::deleteAllEmotionImage()
{
	// �ͷ�map������ѡ��(����)�ı��飬�����ͷŵ���Ȼ��ӳ��Ҳ����һ�����
	for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); itor++) {
		// ֱ��ɾ����
		delete itor.key();  // �˴�ֻ�ǽ���Ӧ���ڴ�������ͷţ���û�н�ӳ���е�����Ҳɾ��
	}
	m_emotionMap.clear();
}

void QMsgTextEdit::onEmotionImageFrameChange(int frame) {
	// ͼƬ�����ı��ʱ�������Ӧ
	QMovie* movie = qobject_cast<QMovie*>(sender());       // sender()�Ƿ����źŵķ�����
	// ��ȡ��ǰ���ĵ�,�����Դ
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}