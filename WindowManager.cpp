#include "WindowManager.h"
#include "TalkWindowItem.h"
#include "TalkWindow.h"
#include  <QSqlQueryModel>

// 单例模式，创建全局静态对象
Q_GLOBAL_STATIC(WindowManager, theInstance)

WindowManager::WindowManager()
	:QObject(nullptr),
	m_talkwindowshell(nullptr)
{
}

WindowManager::~WindowManager()
{}

QWidget* WindowManager::findWindowName(const QString & qsWindowName)
{
	// 根据传入的窗体名字，在我们创建的映射中查找，如果找到就返回对应的窗体，如果没有那么就返回nullptr
	if (m_windowMap.contains(qsWindowName)) {
		return m_windowMap.value(qsWindowName);
	}

	return nullptr;
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	// 直接从映射中进行删除
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	// 如果映射中没有对应的窗口名，那么就进行添加
	if (!m_windowMap.contains(qsWindowName)) {
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{
	return theInstance();    // 返回当前的实例
}

// 添加数据库之后，这个函数进行了变化，也就是对应的参数
void WindowManager::addNewTalkWindow(const QString& uid/*, GroupType groupType, const QString& strPeople*/)
{
	// 添加新的聊天窗口,当你点击到一个项的时候，TalkWindowShell必须先构造出来，否则应该先进行构造
	if (m_talkwindowshell == nullptr) {
		m_talkwindowshell = new TalkWindowShell;    // 没有就构造
		// 如果这个窗口进行关闭的时候，我们也将这个指针也进行销毁
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* object) {
			m_talkwindowshell = nullptr;     // 关闭之后，将创建的设置为空
		});
	}

	// 连接数据库之后，如果是单独聊天就表示对应员工的id号，群聊就是群号
	QWidget* widget = findWindowName(uid);  // 根据唯一的键进行查找

	// 如果widget没有，那么我们就进行创建，如果有说明我们已经打开过了，所以就直接对相应的窗口进行系那是就可以了
	if (!widget) {
		m_strCreatingTalkId = uid;

		// 连接数据库之后TalkWindow的参数也发生了变化
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid/*, groupType)*/);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkwindow);

		m_strCreatingTalkId = "";  // 用完的话设置为空

 		// 数据库之后的修改
		// 判断是群聊还是单聊
		QSqlQueryModel sqlDepModel;   // 数据库查询模型 -- 用来判断查询结果是否有记录

		// 如何判断是群聊还是单聊，我们拿着uid去部门群中去查询，如果可以找到那么
		// 就是群聊
		QString strSql = QString("SELECT department_name,sign FROM table_department WHERE departmentID = %1").arg(uid.toInt());
		sqlDepModel.setQuery(strSql);  // 给查询模型设置sql语句
		int rows = sqlDepModel.rowCount();  // 判断查询的有几行，如果==0，那就是单独聊天

		QString strWindowName, strMsgLabel;

		if (rows == 0) {  // 单聊
			// 获取用户的签名
			QString sql = QString("SELECT  employee_name,employee_sign FROM table_employees WHERE employeeID = %1").arg(uid);
			sqlDepModel.setQuery(sql);   // 设置查询语句
		}

		// 定义模型索引，因为要从模型中取数据
		QModelIndex indexDepIndex, signIndex;  // 部门索引，签名索引
		indexDepIndex = sqlDepModel.index(0, 0);  // 部门索引在模型的第0行第0列
		signIndex = sqlDepModel.index(0, 1);

		// 设置窗口名字，模型调用data方法，根据模型索引返回数据
		strWindowName = sqlDepModel.data(signIndex).toString();
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

        // 设置窗口
		talkwindow->setWindowName(strWindowName);  // 窗口名称
		talkWindowItem->setMsgLabelContent(strMsgLabel);  // 联系人文本显示

		// 添加聊天窗口 -- 根据uid来判断是单聊还是群聊来添加对应的窗口
		m_talkwindowshell->addTalkWindow(talkwindow, talkWindowItem, uid);

		/* 在连接了数据库之后这里也不需要了
		switch (groupType) {
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("在公司"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("公司群"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("在人事"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("人事群"));
			break;
		}
		case MARKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("在市场"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("市场群"));
			break;
		}
		case DEVELOPMENTGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("在开发"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("开发群"));
			break;
		}
		case PTOP:
		{
			talkwindow->setWindowName(QStringLiteral(""));    // 单聊不设置窗口名
			talkWindowItem->setMsgLabelContent(strPeople);
			break;
		}
		default:
			break;
		}
		m_talkwindowshell->addTalkWindow(talkwindow, talkWindowItem, groupType);
		*/
	}
	else {
		// 设置左侧当前的聊天窗口
		// 获取链表项的值
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);    // 返回列表项
		item->setSelected(true);  // 将当前项设置为被选中

		// 设置右侧当前的聊天窗口
		// 有效，就将对应的设置为当前的聊天窗
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();            // 对窗体进行显示
	m_talkwindowshell->activateWindow();  // 设置为当前的活动窗体
}

TalkWindowShell * WindowManager::getTalkWindowShell() const
{
	return m_talkwindowshell;
}

QString WindowManager::getCreatingTalkId()
{
	return m_strCreatingTalkId;
}
