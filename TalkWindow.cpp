#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include "SendFile.h"

#include <QToolTip>
#include <QFile>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>

// 使用数据库之后参数进行了改变
TalkWindow::TalkWindow(QWidget* parent, const QString& uid/*, GroupType groupType*/)
	: QWidget(parent),
	m_talkId(uid)    // 根据传进的uid将窗口id初始化
	// m_groupType(groupType)  -- 使用数据库之后这个变量也不需要了
{
	ui.setupUi(this);

	// 在构建聊天窗口的时候，也就是我们打开了对应的窗口，我们直接在其构造函数(构造窗体
	// 的时候)将对应的映射信息添加到内部，方便我们对打开的窗体进行管理。
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);

	// 添加数据库之后
	initGroupTalkStatus();   // 在初始化控件的时候对群聊状态变量进行设置，因为初始化控件的时候要用到它
	initControl();
}

TalkWindow::~TalkWindow()
{
	// 关闭聊天窗口的时候，应该删除相关聊天窗口的资源，不释放资源会出现相应的问题，占用资源等，当然会造成内存泄露
	WindowManager::getInstance()->deleteWindowName(m_talkId);  // 参数传入的是要释放那个窗口的名字，我们有一个私有成员存放对应的窗体的名称
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	// 添加表情图片
	ui.textEdit->setFocus();    // 文本编辑器获取焦点
	// 有了焦点之后，直接添加表情,直接传入表情的序号
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);   // 在名字标签设置文本
}

QString TalkWindow::getTalkId() const
{
	return m_talkId;
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// 当双击群聊天窗口的群成员项的时候，那么就进入单人聊天
	// 判断点击的是子项还是根项，点击子项那么就显示单人聊天窗体
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {
		// 获取对应单聊窗口的名字  --  根据群组的映射值来获取
		QString peopleName = m_groupPeopleMap.value(item);   // 获取对应项的人名
		// 参数1 :  根据选中单独用户的项设置单聊窗口的id
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()/*, PTOP, peopleName*/);  // 使用数据库进行修改
	}
}

void TalkWindow::initControl()
{
	// 分裂器右边的窗体设置尺寸，两个尺寸可以用一个列表来保存
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;
	// 分裂器是水平的，那么就挨个给里面的窗口设置水平的宽度
	// 垂直的，就挨个给里面的窗口设置垂直的高度
	ui.bodySplitter->setSizes(rightWidgetSize);   // 设置尺寸的参数是一个列表

	ui.textEdit->setFontPointSize(10);            // 设置文本框输入的字体
	ui.textEdit->setFocus();                      // 设置焦点，这样用户就可以直接打开就可以输入了，不用鼠标点击之后再输入

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, &QPushButton::clicked, this, &TalkWindow::onSendBtnClicked);

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// 文件打开按钮被点击，然后触发相应的处理槽函数 --因为文件发送按钮是在我们的talkwindow窗体之下的，所以发送信号实在talkwindow中发送
	// 然后talkwindow中的槽函数将文件窗口构造出来，之后的操作就是文件窗口中的代码了
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	/*连接数据库之后，在此处判断一下当前的聊天是群聊还是单独聊天*/
	if (m_isGroupTalk) {  // 是群聊初始化群聊，否则初始化单聊
		initTalkWindow();
	}
	else {
		initPtoPTalk();
	}

	/* 使用数据库之后进行了修改
	switch (m_groupType) {
	case COMPANY:
	{
		initCompanyTalk();
		break;
	}
	case PERSONELGROUP:
	{
		initPersonelTalk();
		break;
	}
	case MARKETGROUP:
	{
		initMarketTalk();
		break;
	}
	case DEVELOPMENTGROUP:
	{
		initDevelopTalk();
		break;
	}
	default:   // 单独聊天
	{
		initPtoPTalk();        // 初始化单聊使用
		break;
	}
	}
	*/
}

// 增加数据库之后
void TalkWindow::initGroupTalkStatus()
{
	// 根据我们点击的项的窗口的部门id，在数据库中去查找，看其找到的是群聊还是单聊
	// 如果是群聊的话，就是部门id，如果是单独聊天的话就是用户id
	
	QSqlQueryModel sqlDepartmentModel;
	/* 使用我们选择的窗口的id去部门表中找，如果存在数据说明是群聊，如果没有那就是单聊*/
	QString strSql = QString("SELECT * FROM table_department WHERE departmentID = %1").arg(m_talkId.toInt());

	sqlDepartmentModel.setQuery(strSql);
	
	// 判断行号
	int rows = sqlDepartmentModel.rowCount();
	if (rows == 0) {  // 单独聊天
		m_isGroupTalk = false;
	}
	else { // 群聊
		m_isGroupTalk = true;
	}
}

/*使用数据库之后添加的函数*/
int TalkWindow::getCompDepId()
{
	QSqlQuery queryDepId(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群")));
	queryDepId.exec();    // 执行
	queryDepId.next();    // 当前的下一条就是第一条

	return queryDepId.value(0).toInt();  // 返回查到的公司id
}

/* 连接数据库之后，我们就不需要对相应的群进行初始化了。只需要有一个初始化群聊的函数
   就可以了。
void TalkWindow::initCompanyTalk()
{
	// 构造树部件的根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // 设置孩子们的指示器策略
	// 设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell高度 - shell的头高(talkwindow titlewindow的高度)
	ui.treeWidget->setFixedHeight(646);   // 设置固定高度值 talkwindowshell的窗体大小730 - talkwindow的头84的高度 = 当前指定的区域 

	int nEmployeeNum = 50;   // 当前指定一个静态的数据，员工人数为50人
	QString qsGroupName = QString::fromLocal8Bit("公司群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);      // 添加根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // 设置项目的部件

	// 展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // 封装一个函数实现
	}
}

void TalkWindow::initPersonelTalk()
{
	// 构造树部件的根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // 设置孩子们的指示器策略
	// 设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell高度 - shell的头高(talkwindow titlewindow的高度)
	ui.treeWidget->setFixedHeight(646);   // 设置固定高度值 talkwindowshell的窗体大小730 - talkwindow的头84的高度 = 当前指定的区域 

	int nEmployeeNum = 5;   // 当前指定一个静态的数据，员工人数为50人
	QString qsGroupName = QString::fromLocal8Bit("人事群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);      // 添加根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // 设置项目的部件

	// 展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // 封装一个函数实现
	}
}

void TalkWindow::initMarketTalk()
{
	// 构造树部件的根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // 设置孩子们的指示器策略
	// 设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell高度 - shell的头高(talkwindow titlewindow的高度)
	ui.treeWidget->setFixedHeight(646);   // 设置固定高度值 talkwindowshell的窗体大小730 - talkwindow的头84的高度 = 当前指定的区域 

	int nEmployeeNum = 8;   // 当前指定一个静态的数据，员工人数为50人
	QString qsGroupName = QString::fromLocal8Bit("市场群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);      // 添加根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // 设置项目的部件

	// 展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // 封装一个函数实现
	}
}

void TalkWindow::initDevelopTalk()
{
	// 构造树部件的根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // 设置孩子们的指示器策略
	// 设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell高度 - shell的头高(talkwindow titlewindow的高度)
	ui.treeWidget->setFixedHeight(646);   // 设置固定高度值 talkwindowshell的窗体大小730 - talkwindow的头84的高度 = 当前指定的区域 

	int nEmployeeNum = 30;   // 当前指定一个静态的数据，员工人数为50人
	QString qsGroupName = QString::fromLocal8Bit("研发群 %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);      // 添加根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // 设置项目的部件

	// 展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // 封装一个函数实现
	}
}
*/

/*使用数据库之后添加 -- 初始化群聊*/
void TalkWindow::initTalkWindow()
{
	// 构造树部件的根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // 设置孩子们的指示器策略
	// 设置data，用于区分根项子项
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell高度 - shell的头高(talkwindow titlewindow的高度)
	ui.treeWidget->setFixedHeight(646);   // 设置固定高度值 talkwindowshell的窗体大小730 - talkwindow的头84的高度 = 当前指定的区域 

	// 当前聊天的群组名或者是人名 -- 从数据库中查询
	QString strGroupName;
	QSqlQuery queryGroupName(QString("SELECT department_name FROM table_department WHERE departmentID = %1").arg(m_talkId));
	queryGroupName.exec();
	if (queryGroupName.next())  // 指向数据库第一条成功，也就是查询结果集中存在数据
	{
		strGroupName = queryGroupName.value(0).toString();
	}

	// 获取人数    --  其实就是从数据库中去查询(直接在员工表中查询，根据对应的部门号，查出几条记录那么就有几个人)
	// 但是如果是公司群，我们的部门表中，并没有部门号对应它，所以我们对其进行特殊的处理

	
	
	// 从数据库拿取数据，而不是写死
	// int nEmployeeNum = 30;   // 当前指定一个静态的数据，员工人数为50人

	QSqlQueryModel queryEmployeeModel;
	if (getCompDepId() == m_talkId.toInt())   // 群聊是公司群
	{
		/*查询员工的id，判断当前有多少人在群中，当然如果已经注销的账户就没有必要显示了
		  所以我们查询记录的条件就是status = 1 才行，因为=0的就被注销了 不显示*/
		queryEmployeeModel.setQuery("SELECT employeeID FROM table_employees WHERE status = 1");
	}
	else {  // 普通的群
		// 对于普通的群，我们只需要查找对应的群当中有几条记录就行了，所以要根据部门号查询，同理也要是没有注销的用户
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM table_employees WHERE status = 1 AND departmentID = %1").arg(m_talkId));
	}

	// 判断结果集中有多少行，判断出员工的数量
	int nEmployeeNum = queryEmployeeModel.rowCount();

	QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3")
		.arg(strGroupName)
		.arg(0)
		.arg(nEmployeeNum);


	/* 使用数据库进行了修改
	QString qsGroupName = QString::fromLocal8Bit("研发群 %1/%2").arg(0).arg(nEmployeeNum);
	*/
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);      // 添加根项
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // 设置项目的部件

	// 展开
	pRootItem->setExpanded(true);

	// 添加子项
	for (int i = 0; i < nEmployeeNum; i++) {
		// 添加成员子项信息 -- 不能和之前写死的一样了，因为我们添加的每一项都是员工的信息
		// addPeopleInfo(pRootItem);        // 封装一个函数实现
		// 因为我们前面已经将对应的数据都查询到了模型当中，所以我们使用模型索引从模型中读取数据
		QModelIndex modelIndex = queryEmployeeModel.index(i,0);  // 获取当前行对应的模型索引，通过模型索引获取数据
		int employeeID = queryEmployeeModel.data(modelIndex).toInt();

		// 根据员工id添加人
		addPeopleInfo(pRootItem,employeeID); // 多传入一个员工id
	}
}

// 单个人聊天，我们不需要显示人数，直接显示一张图片就行
void TalkWindow::initPtoPTalk()
{
	QImage pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png"); // 加载图像

	ui.widget->setFixedSize(pixSkin.size());    // 右边窗体为图片大小
	// pixSkin.scaled(ui.widget->size());
	 
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(QPixmap::fromImage(pixSkin));
	skinLabel->setFixedSize(ui.widget->size());
}

void TalkWindow::addPeopleInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	// 添加员工信息
	QTreeWidgetItem* pChild = new QTreeWidgetItem;   // 构造一个子项

	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");  // 加载图片

	// 添加子节点
	pChild->setData(0, Qt::UserRole, 1);
	// pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild)); 使用数据库之后进行修改，之前我们使用的是窗口的id，使用的是地址来表示id的
	pChild->setData(0, Qt::UserRole + 1,employeeID); // 添加子节点的表示是员工的id(就是添加群内部人员的id，所以使用员工号)

	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	// 从数据库中获取名，签名，头像
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;  // 查询员工信息
	// 根据传入的员工id，查询对应员工的相关数据
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM table_employees WHERE employeeID = %1").arg(employeeID));
	
	QModelIndex nameIndex, signIndex, pictureIndex;  // 使用模型索引获取数据
	nameIndex = queryInfoModel.index(0, 0);   // 0行0列的索引，用来获取对应位置的数据
	signIndex = queryInfoModel.index(0, 1);
	pictureIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();  // 名字
	strSign = queryInfoModel.data(signIndex).toString();  // 签名
	strPicturePath = queryInfoModel.data(pictureIndex).toString(); // 图片路径

	// 使用数据库之后进行了修改
	// const QPixmap image(":/Resources/MainWindow/girl.png"); //加const为了后面传参数
	QImage imageHead;   // 加载图片
	imageHead.load(strPicturePath);
	
	// 先用静态数据进行演示
	// static int i = 0; 使用数据库进行改变
	// pContactItem->setPixmap(CommonUtils::getRoundImage(image, pix1, pContactItem->getHeadLabelSize()));
	pContactItem->setPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));
	
	// vs下传入中文的时候我们使用fromLocal8Bit进行一下编码的转换，避免乱码
	// pContactItem->setUserName(QString::fromLocal8Bit("好友%1").arg(i++));  使用数据修改
	pContactItem->setUserName(strName);

	// 使用数据库进行修改
	// pContactItem->setSignName(QString::fromLocal8Bit(""));  // 设置签名为空，只是效果就不设置了
	pContactItem->setSignName(strSign);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// 为了能够识别对应的窗体，我们定义一个窗体和id的一个映射
	QString str = pContactItem->getUserName();  // 获取到用户名称
	m_groupPeopleMap.insert(pChild, str);       // 将用户名称添加到映射当中
}


void TalkWindow::onSendBtnClicked(bool clicked) {
	// 发送按钮被点击 
	// 将文本编辑器转换为纯文本，判断纯文本是否为空，也就是发送的时候文本框中有没有数据，有数据才发送
	if (ui.textEdit->toPlainText().isEmpty()) {
		// 为空进行一些提示,显示文本
		// 首先传入参数，文本的位置信息在哪里,但是QPoint获取的是相对的位置，要设置为全局的坐标
		// 全局坐标就是以桌面的左上角就不是聊天窗体的左上角了
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("发送的信息不能为空"),
			this, QRect(0, 0, 120, 100), 2000);  // 父窗体为当前的窗体，设置显示框矩形的大小,并且只显示2000ms
		return;  // 直接返回空就行，因为当前情况是没有文本的情况
	}

	// 对于有文本的情况
	// 获取文本编辑器的文档内容，获取其当前文档转换为html，因为要用它来进行网页内容显示
	QString html = ui.textEdit->document()->toHtml();   // 返回字符串，进行数据保存

	// 文本html没有字体就添加字体
	// 不是图片，同时不包含文本
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", text);
			file.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件不存在"));
			return;
		}

		if (!html.contains(fontHtml)) {
			html.replace(text, fontHtml);
		}
	}

	// 格式转换完之后，就能够追加信息了
	// 收信息窗体之后显示，那么我们发送信息的文本编辑器就不需要进行显示信息了
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();  // 因为表情和文本不一样，表情创建是动态开辟空间的，你在
										   // 文本框中删除之后，还需要清理其占用的内存，我们进行了封装
	ui.msgWidget->appendMsg(html);		   // msgWidget是进行升级之后的

}

void TalkWindow::onFileOpenBtnClicked(bool flag) {
	// 构造发送文件的对话框，然后将其进行显示
	SendFile * sendFile = new SendFile(this);  // 我们点击打开文件之后，对应就要构造文件的可视化控件，因为对应按钮实在talkwindow窗体下的，所以槽函数也应该在次窗体下
	sendFile->show();    // 显示窗口
}
