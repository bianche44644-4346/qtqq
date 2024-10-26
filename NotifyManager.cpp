#include "NotifyManager.h"
#include "CommonUtils.h"

NotifyManager* NotifyManager::instance = nullptr;   // ��ʼ��Ϊ��

NotifyManager::NotifyManager()
	:QObject(nullptr)  // ������QObject
{

}

NotifyManager::~NotifyManager()
{}

NotifyManager* NotifyManager::getInstance() {
	// ���Ϊ�վ͹���һ��instance��ָ��
	if (instance == nullptr) {
		instance = new NotifyManager;
	}
	// �����Ϊ�վ�ֱ�ӷ���
	return instance;
}

// ֪ͨ�����Ĵ���
void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color) {
	emit signalSkinChanged(color);          // ����Ŀؼ������źţ����Ұ����޸ĵ�Ƥ����ɫ

	// ����common�еķ������޸�Ƥ����Ĭ����ɫ���û��޸�����ɫ����ô����ҲҪ�������ļ�
	// �е����ݽ�����д(�������ļ��Ĳ���ʹ����common�Ľӿ�)
	CommonUtils::setDefaultSkinColor(color);
}
