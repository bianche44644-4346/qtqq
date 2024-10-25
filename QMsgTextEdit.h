#pragma once

#include <QTextEdit>

class QMsgTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();

private slots:
	// 动画改变时，响应的槽函数
	void onEmotionImageFrameChange(int frame);   // 传入一个动画的帧

public:
	// 添加表情
	void addEmotionUrl(int emotionNum);          // 参数表示传入的是哪个表情  
	void deleteAllEmotionImage();                // 删除所有的表情图片 -- 就是我们选择了表情之后，聊天框就存在一个表情，我们点击发送之后，这个表情应该在发送框中就没有了，而不是还停留在那了里

private:
	QList<QString> m_listEmotionUrl;             // 保存选中的表情
	QMap<QMovie*, QString> m_emotionMap;         // 保存电影和表情的映射关系
};
