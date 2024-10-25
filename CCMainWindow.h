#pragma once
/*
*  获取圆图片的时候我们应该将准备的图片宽和高都相同(正方形或者接近正方形)，否则获取
   到的圆图片不好看大小的话没有一些太多的限制。
*/
#include <QtWidgets/QWidget>
#include "ui_CCMainWindow.h"
#include "BasicWindow.h"
#include <QMouseEvent>

class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	// 数据库后新增两个参数，一个是账号，另一个是判断是否是账号登录(因为我们有两种登录方式
	// false 表示account是qq号，true那么account是账号)
	CCMainWindow(QString account,bool isAccountLogin,QWidget *parent = Q_NULLPTR);
	virtual ~CCMainWindow();

private:
	void resizeEvent(QResizeEvent* event);
	// 重写事件过滤器
	bool eventFilter(QObject* obj, QEvent* event);
	void mousePressEvent(QMouseEvent* event);
	

private slots:
	void onAppIconClicked();
	// 被点击
	void onItemClicked(QTreeWidgetItem* item, int column);
	// 点击展开
	void onItemExpanded(QTreeWidgetItem* item);
	// 点击收缩
	void onItemCollapsed(QTreeWidgetItem* item);
	// 双击子项
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);

public:
	// 设置用户等级图片
	void setLevelPixmap(int level);
	// 设置头像
	void setHeadPixmap(const QString& headPath); // 传入一个路径
	// 设置在线状态  -- 传入一个状态的路径
	void setStatusMenuIcon(const QString& statusPath);
	// 添加应用部件  --  返回值是一个部件
	// 参数:  app的路径和app的名字
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);
	// 初始化计时器，用于记录等级更新
	void initTimer();
	// 初始化联系人
	void initContactTree();

private:
	// 初始化控件
	void initControl();
	// 初始化用户名
	void setUserName(const QString& username);
	// 更新搜索框的样式
	void updateSearchStyle();
	// 添加公司部门 -- 需要指定在哪个根项上面添加 第二个参数在使用数据库之后进行了修改，直接使用了部门的id
	// void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, const QString& sDeps);
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepId);

	// 从数据库获取登录用户所使用的头像的路径
	QString getHeadPicturePath();

private:
	Ui::CCMainWindowClass ui;
	// 表示哪一个聊天窗口对应的是哪一个名称

	// 在使用数据库之后，这个变量就不需要了
	// QMap<QTreeWidgetItem*, QString> m_groupMap;  // 所有分组的分组项

	// 保存用户是否为账号登录的判断以及保存用户输入的账号
	bool m_isAccountLogin;
	QString m_account;   // 登录的账号或者id号
};
