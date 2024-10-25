#include "NotifyManager.h"
#include "CommonUtils.h"

NotifyManager* NotifyManager::instance = nullptr;   // 初始化为空

NotifyManager::NotifyManager()
	:QObject(nullptr)  // 基类是QObject
{

}

NotifyManager::~NotifyManager()
{}

NotifyManager* NotifyManager::getInstance() {
	// 如果为空就构造一下instance的指针
	if (instance == nullptr) {
		instance = new NotifyManager;
	}
	// 如果不为空就直接返回
	return instance;
}

// 通知其它的窗口
void NotifyManager::notifyOtherWindowChangeSkin(const QColor& color) {
	emit signalSkinChanged(color);          // 给别的控件发送信号，并且包括修改的皮肤颜色

	// 调用common中的方法，修改皮肤的默认颜色，用户修改了颜色，那么我们也要将配置文件
	// 中的数据进行重写(对配置文件的操作使用类common的接口)
	CommonUtils::setDefaultSkinColor(color);
}
