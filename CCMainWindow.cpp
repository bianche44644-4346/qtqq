#include "CCMainWindow.h"
#include <QProxyStyle>
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QSqlQuery>
#include "RootContactItem.h"
#include "SysTray.h"
#include "CommonUtils.h"
#include "SkinWindow.h"
#include "NotifyManager.h"
#include "TalkWindowShell.h"
#include "ContactItem.h"
#include "WindowManager.h"
/*
class CustomProxyStyle : public QProxyStyle {
public:
	CustomProxyStyle();
	~CustomProxyStyle();

	// 注意此处要是const修饰的常函数，因为类中可以按照是否有const修饰对函数进行重载
	// 如果没有写，那就不是对原来类进行重写，而是重载了，就是写了自己的类，默认它还是
	// 去调用内部的函数
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* opt,
		QPainter* painter, const QWidget* widget = nullptr) const{
		// 如果属性是获取焦点显示表框，那么我们直接返回就不做操作
		if (element == PE_FrameFocusRect) {
			return;
		}
		else {  // 否则其它的操作正常进行(调用父类的这个函数)
			QProxyStyle::drawPrimitive(element, opt, painter, widget);
		}
	}
};

CustomProxyStyle::CustomProxyStyle() {};
CustomProxyStyle::~CustomProxyStyle() {};
*/

// 在获取头像的函数中对其进行初始化
QString gstrLoginHeadPath;        // 存放登录者头像地址，因为在MsgWebView中显示页面的对话信息的时候要使用
extern QString gLoginEmployeeID;  // 外部声明，定义在userLogin中

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin,QWidget *parent)
	: BasicWindow(parent),m_isAccountLogin(isAccountLogin),m_account(account)
{
	ui.setupUi(this);

	//设置窗体的风格 参数:  使用当前窗口风格和工具窗口风格的组合
	setWindowFlags(windowFlags() | Qt::Tool);

	loadStyleSheet("CCMainWindow");
	updateSearchStyle();                   /*因为后面我们设置了搜索框的颜色根据背景颜色
											 进行改变，但是样式表中已经写死，那么我们就
											 在这个位置进行一个更新，让其在刚开始的时候
											 与背景的颜色一致，如果不更新，最开始进去就
											 不和背景颜色匹配了，是使用默认设置的样式*/

	// 返回登录用户的头像路径
	setHeadPixmap(getHeadPicturePath());  // 在调用函数返回登录用户头像路径会后，此处直接设置头像
	initControl();
	initTimer();       // 计时器，实现等级改变
}

CCMainWindow::~CCMainWindow()
{}

// 重新设置用户名的触发事件
void CCMainWindow::resizeEvent(QResizeEvent * event)
{
	// 设置用户名     18位(内部使用utf-16)转换为8位
	setUserName(QString::fromLocal8Bit("微醺"));  //用户名}
	BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (ui.searchLineEdit == obj) {   // 如果搜索框是被监视的对象
		if (event->type() == QEvent::FocusIn) {  // 判断是否是获取焦点的事件
			// 事件是键盘焦点事件 -- 对输入框设置样式    -- 此处不能使用QStringLiter（可能是格式化字符串不能用）
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2, %3,100)} \
                                                           QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
                                                           QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
                                                           QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")  // 按钮按下时设置样式
				.arg(m_colorBackGround.red())
				.arg(m_colorBackGround.green())
				.arg(m_colorBackGround.blue()));
		}
		else if (event->type() == QEvent::FocusOut) {   // 没有焦点了，也进行相应的操作
			updateSearchStyle();
		}
	}
	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	// 鼠标按下事件  
	// 输入框不需要焦点的时候需要对其进行清除
	// 如果你鼠标选择的不是searchLineEdit，但是同时searchLineEdit还有焦点，那么我们就将其焦点删除
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus()) {
		ui.searchLineEdit->clearFocus();
	} //  如果你鼠标选择的不是lineEdit，但是同时lineEdit还有焦点，那么我们就将其焦点删除
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus()) {
		ui.lineEdit->clearFocus();
	}

	// 其它的就进行基础的鼠标事件处理
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	// 项目被点击，如果有子项就展开，没有就不用了
	bool bIsChild = item->data(0, Qt::UserRole).toBool();  // 返回数据为0就是根项目，子项设置为1
	if (!bIsChild) {
		item->setExpanded(!item->isExpanded());  //  参数: 判断是否是展开的，如果是展开的就不需要了，否则就展开，所以对于相应的函数进行取反  
	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	// 展开的信号发送的时候，判断其是否为根项 -- 是根项就展开
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild) {
		// dynamic_cast将一个基类对象指针转换为继承的或者派生类的指针，转换失败是空指针，所以需要进行判断
		//                                           这个项会返回itemWidget的指针，是一个QWidget的指针
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (prootItem) {
			prootItem->setExpanded(true);       // 设置箭头的动画转换为true
		}
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	// 收缩
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild) {
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (prootItem) {
			prootItem->setExpanded(false);       // 将参数设置为false
		}
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// 判断双击的是根项还是子项，如果是子项就显示聊天窗口
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {   // 如果是子项就显示窗口
	  // 保存每个项对应的id
	 //     使用数据库之后，m_groupMap不在需要，所以这里注释进行修改
	 // 	QString strGroup = m_groupMap.value(item);   // 这样就能知道其当前选择的是哪个群

		//   使用数据库之后，我们这里直接这样写就行
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());

		/*不同的群聊天窗口的显示是不一样的*/
		/* 再添加数据库之后我们这里判断添加的是什么新窗口群就不用这么麻烦了
		if (strGroup == QString::fromLocal8Bit("公司群")) {
			// 如果是公司群，那么添加的新窗口就是公司群
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), COMPANY);   // 单例构造对象
		}
		else if (strGroup == QString::fromLocal8Bit("人事部")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("市场部")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("研发部")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		}*/


	}
}

/*
  初始化控件
*/
void CCMainWindow::initControl()
{
	// 自定义风格 -- 取消我们选中某一行的时候会自带一个框框，默认会来一个黑色框
	ui.treeWidget->setStyle(new CustomProxyStyle); // 自定义类
	// 设置等级图标
	setLevelPixmap(50);
	// 设置头像 -- 添加数据库之后，我们从数据库中去读取用户对应的图片路径，然后返回的路径我们直接在构造函数中就实现了
	// setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	// 添加顶部位置的app图标
	QHBoxLayout* appUpLayout = new QHBoxLayout;
	appUpLayout->setContentsMargins(0, 0, 0, 0);  // 四个方向的间隙都设置为0(对于布局)
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));  // 皮肤

	// 设置部件之间的空隙 -- TODO： 因为觉得当前的间隙还不错就不调整了，后续有需要再进行调整
	// appUpLayout->setSpacing(2);

	// 设置布局到界面的位置,我们在设计模式中指定了块widget用来存放app，所以我们可以
	// 将我们设置好的布局设置到我们指定的widget范围内
	ui.appWidget->setLayout(appUpLayout);

	// 添加底部位置的app图标,因为我们设计模式的时候对于存放底部app的位置已经设计了
	// 布局，所以我们直接使用这个布局，在其内部添加btn控件即可
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));

	// 上面直接显示，会导致底部的控件间隙太大
	ui.bottomLayout_up->addStretch();    // 添加一个伸缩，这样右面就会被空白填充，就会变得紧凑

	// 初始化联系人
	initContactTree();

	//个性签名，安装事件过滤器，当前窗体对象监视它
	ui.lineEdit->installEventFilter(this);
	// 好友搜索，安装事件过滤器
	ui.searchLineEdit->installEventFilter(this);

	// 连接主窗体最小化按钮和关闭按钮
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]() {
		updateSearchStyle();    // 更新搜索风格
	});

	SysTray* systray = new SysTray(this);   // 设置系统托盘
}

void CCMainWindow::setUserName(const QString& username)
{
	// 设置用户名
	ui.nameLabel->adjustSize();        // 此方法可以让标签根据内容设置自己的尺寸

	// 文本过长就进行省略(就是用户名字过长，屏幕显示不下的时候，我们应该将后面的进行
	// 省略，显示三个...)

	// 调用此方法，返回的是QFontMetrics类对象，返回的不止指针，负责字体的体积和容积,
	// 参数                                 文本，  省略的模式(主要指定省略号出现的位置)，长度(当文本超过指定长度，那么后面的文本就以...显示)
	// 设置完省略文本之后会返回一个字符串
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());// 表示省略文本

	// 设置文本
	ui.nameLabel->setText(name);   // 将省略的文本设置到标签上
}

void CCMainWindow::updateSearchStyle()
{
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,100);border-bottom:1px solid rgba(%1,%2, %3,30)}\
                                             QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red())
		.arg(m_colorBackGround.green())
		.arg(m_colorBackGround.blue()));   // 颜色顺序不能反，因为是rgb，有一定顺序
}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepId)
{
	// 构造一个子项
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");

	/*
	// 添加子节点   第0列                子节点数据
	pChild->setData(0, Qt::UserRole, 1);
	// 给UserRole + 1这一项设置了唯一的识别号
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));
	*/
	// 给对应的子节点设置表示数据的时候，我们就不在需要使用地址值了，而是直接使用
	// 设置的部门id值就行
	pChild->setData(0, Qt::UserRole, 1);
	// 给UserRole + 1这一项设置了唯一的识别号
	pChild->setData(0, Qt::UserRole + 1, DepId);

	// 从数据库获取公司，部门的头像
	QPixmap groupPix;
	QSqlQuery queryPicture(QString("SELECT picture FROM table_department WHERE departmentID = %1").arg(DepId));
	// 执行sql语句拿到图片路径
	queryPicture.exec();
	queryPicture.next();   // 指向下一条，也就是第一条
	groupPix.load(queryPicture.value(0).toString());

	// 获取部门名称
	QString strDepName;
	QSqlQuery queryDepName(QString("SELECT department_name FROM table_department WHERE departmentID = %1").arg(DepId));
	queryDepName.exec();
	queryDepName.next();
	strDepName = queryDepName.value(0).toString();
	
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);  // 父节点为父控件

	// 设置部门头像需要获取圆头像 -- 用数据库改版之后
	pContactItem->setPixmap(getRoundImage(groupPix, pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);

	/* 使用数据库之后对其进行了修改，不在使用固定的头像而是使用从数据库中获取的
	用户头像进行显示
	pContactItem->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(sDeps);
	*/

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// m_groupMap.insert(pChild, sDeps);  使用数据库之后就已经不需要了
}
QString CCMainWindow::getHeadPicturePath()
{
	QString strPicturePath;

	if (!m_isAccountLogin)  // id号登录
	{
		// 去员工表中查询数据
		QSqlQuery queryPicture(QString("SELECT picture FROM table_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
		queryPicture.exec();
		queryPicture.next();

		// 直接取值
		strPicturePath = queryPicture.value(0).toString();
	}
	else  // 账号登录 
	{
		QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM table_accounts WHERE account = '%1'").arg(m_account));
		queryEmployeeID.exec();
		queryEmployeeID.next();

		int employeeID = queryEmployeeID.value(0).toInt();
		
		QSqlQuery queryPicture(QString("SELECT picture FROM table_employees WHERE employeeID = %1").arg(employeeID));
		queryPicture.exec();
		queryPicture.next();

		strPicturePath = queryPicture.value(0).toString();
	}

	gstrLoginHeadPath = strPicturePath;
	return strPicturePath;
}


// 设置等级位图
void CCMainWindow::setLevelPixmap(int level)
{
	// 初始化等级位图 -- 使用指定的等级按钮的大小来设置等级位图的大小
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);                 // 给位图设置透明

	QPainter painter(&levelPixmap);                    // 通过画家在等级位图上画画
	// 在0,4的坐标位置将等级图片画上去(是一个表示等级的图片，并不是具体的等级)
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	//等级也有不同(不同的等级显示的图片也不同)，我们使用两个变量来表示等级的个位数和十位数
	int unitNum = level % 10;    // 等级的个位数
	int tenNum = level / 10;    // 等级的十位数

	// 画具体的等级
	// 先画10位                      此路经中表示0-9的所有等级，但是我们只需要一个，
	// 所以我们需要对图片进行处理，绘制当中的一部分,使用drawPixmap另外一个重载

	// 第三个参数之后，是四个位置： 前两个参数输入绘制图片的左上角，后两个用来指定
	// 绘制图片的宽和高
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		tenNum * 6,  // 乘以6是因为准备的资源图片，每个数字之间是6个像素
		0, 6, 7);

	// 画个位,第一个参数为16，是因为个位是在十位后面，数字间隔为6像素
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		unitNum * 6, 0, 6, 7);

	// 给登记按钮设置图片
	ui.levelBtn->setIcon(levelPixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());  // 指定图标的大小和按钮一样大
}

/*
  设置头像
*/
void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;   // 设置图片
	pix.load(":/Resources/MainWindow/head_mask.png");
	ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

/*
  设置状态
*/
void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	QPixmap statusBtnPixmap(ui.stausBtn->size());   // 通过状态位图进行设置
	statusBtnPixmap.fill(Qt::transparent);          // 背景设置为透明

	QPainter painter(&statusBtnPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));

	// 给按钮设置图标
	ui.stausBtn->setIcon(statusBtnPixmap);
	ui.stausBtn->setIconSize(ui.stausBtn->size());   // 设置按钮的大小为图标大小
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	// 这个按钮我们在UI设计模式的时候没有设置，是在此函数中指定的，然后返回构造的按钮
	// 在调用方将其设置在相应的位置(利用布局等方式)
	QPushButton* btn = new QPushButton(this);   // 以当前窗体作为其父窗体
	btn->setFixedSize(20, 20);

	// 创建一个图标对象，并且设置其大小
	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	// 开始画画
	QPainter painter(&pixmap);
	// 根据用户传入的app的路径构造一个图标
	QPixmap appPixmap(appPath);
	// 参数根据按钮的宽度-图标的宽度/2得到据按钮两边的间距，也就是坐标的开始，保证对应
	// 图标在按钮的中间
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2,
		(btn->height() - appPixmap.height()) / 2,
		appPixmap);     // 在画家指定的绘图设备上进行绘制
// 对相应的按钮设置图标
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	// 给对应的按钮app指定一下对象名 -- 因为我们设计模式下没有添加，此时我们在代码中
	// 进行添加，为了以后能够操作这个控件我们也需要为其设置一个对象名
	btn->setObjectName(appName);

	// 设置属性,对相应的属性设置为true，表示有边框的属性
	btn->setProperty("hasborder", true);

	// 当点击app的时候，我们就需要进行一些相应的操作，所以链接信号槽
	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

	return btn;  // 返回构建好的按钮
}

void CCMainWindow::initTimer()
{
	// 假设过500ms就升一个等级
	QTimer* timer = new QTimer(this);
	timer->setInterval(500);    // 设置一个间隔

	// 连接信号和槽，时间到了就发送信号进行等级的升级
	// 信号一旦发射就执行匿名函数中的内容
	connect(timer, &QTimer::timeout, [this]() {  // lambda中使用this的外部变量
		static int level = 0;                    // 使用静态的变量表示等级
		level++;
		if (level == 99) level = 0;             // 目前只存在两位数的等级
		setLevelPixmap(level);                   // 设置等级头像
	});

	timer->start();   // 启动计时器
}

/*初始化联系人*/
void CCMainWindow::initContactTree()
{
	// 点击项的时候就展开或者收缩，连接展开和收缩的信号
	// 参数 第一个: 点击的是数的哪一项，以及哪一列
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	// 展开
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	// 收缩
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	// 双击
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// 根节点 -  表示存在多少个群组
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	// 设计子项展开的策略 -- 用一个箭头来实现
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	// 设置根节点的数据
	pRootGroupItem->setData(0, Qt::UserRole, 0);   // 第0列
	// 添加子项

	RootContactItem* pItem = new RootContactItem(true, ui.treeWidget);  // 父部件为树部件

	// 获取公司部门id
	// 定义查询类，指定查询语句，因为我们查询的字段是中文的，所以我们需要使用String的方法将其转换为Qt字符串的格式，和编码，否则会出现乱码
	// 就匹配不上了，这也是为什么我们使用中文名称进行登录的时候无法成功的原因，因为如果要使用中文就需要转换为Qt的编码格式否则会出错

	QSqlQuery queryCompDepId(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("公司群")));
	queryCompDepId.exec();
	queryCompDepId.first();
	int CompDep = queryCompDepId.value(0).toInt();  // 从结果集中获取到查询到的公司群id值，因为值查询的是1列，那么就直接查询第0列

	// 获取qq登录者所在的部门id(部门群号)  --  此处使用员工的id也就是qq号去表中查询结果，但是我们登录的方式有两种
	// 所以我们定义一个全局变量记录用户的id号，方便我们此处去查询使用
	// 如果用户输入的是id，那么直接读取赋值给它就行，如果输入的是名字作为账号，我们去表中查询对应的id然后记录在变量
	// 当中，我们只需要在登录验证的时候，根据不同的输入来更新这个全局变量就行
	QSqlQuery querySelfDepId(QString("SELECT departmentID FROM table_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	querySelfDepId.exec();
	querySelfDepId.first();
	int SelfDepID = querySelfDepId.value(0).toInt();  // 从结果集中获取到查询到的部门id值，因为

	// 在此处添加项，因为我们每个登陆者只会存在两个部门，所以其只需要记载两个项(群)就行
	addCompanyDeps(pRootGroupItem, CompDep);
	addCompanyDeps(pRootGroupItem, SelfDepID);

	// 将根项添加到树里面
	// 定义一个文本，将字符串SelfDepID从本地编码转换为Unicode编码
	QString strGroupName = QString::fromLocal8Bit("联系人");
	pItem->setText(strGroupName);    // 设置文本

	// 插入分组节点  添加顶级项
	ui.treeWidget->addTopLevelItem(pRootGroupItem);
	// 参数:                     根节点          列  设置的部件
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItem);   // 设置项部件

    /*根据数据库的信息来获取并且判断对应用户所在的群聊*/
	// 不同的人都会显示公司群，但是每个人又存在不同的部门，但是对于登录的人而言
	// 我们只需要显示其所在的部门群就行，没必要每个部门都显示，但是每个人都应该
	// 显示公司群，至于显示那个部门，就看数据库中你存储的是你在哪个部门了



	/*
	要结合数据库来实现这部分功能了
	// 实现双击展开，假设有4个
	QStringList sCompDeps;
	sCompDeps << QString::fromLocal8Bit("公司群");
	sCompDeps << QString::fromLocal8Bit("人事部");
	sCompDeps << QString::fromLocal8Bit("研发部");
	sCompDeps << QString::fromLocal8Bit("市场部");

	// 添加到分组项中去
	for (int nIndex = 0; nIndex < sCompDeps.length(); nIndex++) {
		addCompanyDeps(pRootGroupItem, sCompDeps.at(nIndex));
	}
	*/
}

// 当app的按钮被点击，我们进行一个处理，点击什么app，就响应什么功能
void CCMainWindow::onAppIconClicked() {
	// 皮肤app被点击之后，弹出皮肤窗口
	// 判断信号发送者对象名是否是app_skin
	if (sender()->objectName() == "app_skin") {
		SkinWindow* skinWindow = new SkinWindow;   // 创建一个skinWindow
		skinWindow->show();                        // 显示出来
	}
}
