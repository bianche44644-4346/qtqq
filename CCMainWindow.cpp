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

	// ע��˴�Ҫ��const���εĳ���������Ϊ���п��԰����Ƿ���const���ζԺ�����������
	// ���û��д���ǾͲ��Ƕ�ԭ���������д�����������ˣ�����д���Լ����࣬Ĭ��������
	// ȥ�����ڲ��ĺ���
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* opt,
		QPainter* painter, const QWidget* widget = nullptr) const{
		// ��������ǻ�ȡ������ʾ�����ô����ֱ�ӷ��ؾͲ�������
		if (element == PE_FrameFocusRect) {
			return;
		}
		else {  // ���������Ĳ�����������(���ø�����������)
			QProxyStyle::drawPrimitive(element, opt, painter, widget);
		}
	}
};

CustomProxyStyle::CustomProxyStyle() {};
CustomProxyStyle::~CustomProxyStyle() {};
*/

// �ڻ�ȡͷ��ĺ����ж�����г�ʼ��
QString gstrLoginHeadPath;        // ��ŵ�¼��ͷ���ַ����Ϊ��MsgWebView����ʾҳ��ĶԻ���Ϣ��ʱ��Ҫʹ��
extern QString gLoginEmployeeID;  // �ⲿ������������userLogin��

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin,QWidget *parent)
	: BasicWindow(parent),m_isAccountLogin(isAccountLogin),m_account(account)
{
	ui.setupUi(this);

	//���ô���ķ�� ����:  ʹ�õ�ǰ���ڷ��͹��ߴ��ڷ������
	setWindowFlags(windowFlags() | Qt::Tool);

	loadStyleSheet("CCMainWindow");
	updateSearchStyle();                   /*��Ϊ�����������������������ɫ���ݱ�����ɫ
											 ���иı䣬������ʽ�����Ѿ�д������ô���Ǿ�
											 �����λ�ý���һ�����£������ڸտ�ʼ��ʱ��
											 �뱳������ɫһ�£���������£��ʼ��ȥ��
											 ���ͱ�����ɫƥ���ˣ���ʹ��Ĭ�����õ���ʽ*/

	// ���ص�¼�û���ͷ��·��
	setHeadPixmap(getHeadPicturePath());  // �ڵ��ú������ص�¼�û�ͷ��·����󣬴˴�ֱ������ͷ��
	initControl();
	initTimer();       // ��ʱ����ʵ�ֵȼ��ı�
}

CCMainWindow::~CCMainWindow()
{}

// ���������û����Ĵ����¼�
void CCMainWindow::resizeEvent(QResizeEvent * event)
{
	// �����û���     18λ(�ڲ�ʹ��utf-16)ת��Ϊ8λ
	setUserName(QString::fromLocal8Bit("΢��"));  //�û���}
	BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (ui.searchLineEdit == obj) {   // ����������Ǳ����ӵĶ���
		if (event->type() == QEvent::FocusIn) {  // �ж��Ƿ��ǻ�ȡ������¼�
			// �¼��Ǽ��̽����¼� -- �������������ʽ    -- �˴�����ʹ��QStringLiter�������Ǹ�ʽ���ַ��������ã�
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2, %3,100)} \
                                                           QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
                                                           QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
                                                           QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")  // ��ť����ʱ������ʽ
				.arg(m_colorBackGround.red())
				.arg(m_colorBackGround.green())
				.arg(m_colorBackGround.blue()));
		}
		else if (event->type() == QEvent::FocusOut) {   // û�н����ˣ�Ҳ������Ӧ�Ĳ���
			updateSearchStyle();
		}
	}
	return false;
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	// ��갴���¼�  
	// �������Ҫ�����ʱ����Ҫ����������
	// ��������ѡ��Ĳ���searchLineEdit������ͬʱsearchLineEdit���н��㣬��ô���Ǿͽ��佹��ɾ��
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus()) {
		ui.searchLineEdit->clearFocus();
	} //  ��������ѡ��Ĳ���lineEdit������ͬʱlineEdit���н��㣬��ô���Ǿͽ��佹��ɾ��
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus()) {
		ui.lineEdit->clearFocus();
	}

	// �����ľͽ��л���������¼�����
	BasicWindow::mousePressEvent(event);
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	// ��Ŀ�����������������չ����û�оͲ�����
	bool bIsChild = item->data(0, Qt::UserRole).toBool();  // ��������Ϊ0���Ǹ���Ŀ����������Ϊ1
	if (!bIsChild) {
		item->setExpanded(!item->isExpanded());  //  ����: �ж��Ƿ���չ���ģ������չ���ľͲ���Ҫ�ˣ������չ�������Զ�����Ӧ�ĺ�������ȡ��  
	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	// չ�����źŷ��͵�ʱ���ж����Ƿ�Ϊ���� -- �Ǹ����չ��
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild) {
		// dynamic_cast��һ���������ָ��ת��Ϊ�̳еĻ����������ָ�룬ת��ʧ���ǿ�ָ�룬������Ҫ�����ж�
		//                                           �����᷵��itemWidget��ָ�룬��һ��QWidget��ָ��
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (prootItem) {
			prootItem->setExpanded(true);       // ���ü�ͷ�Ķ���ת��Ϊtrue
		}
	}
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	// ����
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild) {
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (prootItem) {
			prootItem->setExpanded(false);       // ����������Ϊfalse
		}
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// �ж�˫�����Ǹ�������������������ʾ���촰��
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {   // ������������ʾ����
	  // ����ÿ�����Ӧ��id
	 //     ʹ�����ݿ�֮��m_groupMap������Ҫ����������ע�ͽ����޸�
	 // 	QString strGroup = m_groupMap.value(item);   // ��������֪���䵱ǰѡ������ĸ�Ⱥ

		//   ʹ�����ݿ�֮����������ֱ������д����
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());

		/*��ͬ��Ⱥ���촰�ڵ���ʾ�ǲ�һ����*/
		/* ��������ݿ�֮�����������ж���ӵ���ʲô�´���Ⱥ�Ͳ�����ô�鷳��
		if (strGroup == QString::fromLocal8Bit("��˾Ⱥ")) {
			// ����ǹ�˾Ⱥ����ô��ӵ��´��ھ��ǹ�˾Ⱥ
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), COMPANY);   // �����������
		}
		else if (strGroup == QString::fromLocal8Bit("���²�")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("�г���")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
		}
		else if (strGroup == QString::fromLocal8Bit("�з���")) {
			WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
		}*/


	}
}

/*
  ��ʼ���ؼ�
*/
void CCMainWindow::initControl()
{
	// �Զ����� -- ȡ������ѡ��ĳһ�е�ʱ����Դ�һ�����Ĭ�ϻ���һ����ɫ��
	ui.treeWidget->setStyle(new CustomProxyStyle); // �Զ�����
	// ���õȼ�ͼ��
	setLevelPixmap(50);
	// ����ͷ�� -- ������ݿ�֮�����Ǵ����ݿ���ȥ��ȡ�û���Ӧ��ͼƬ·����Ȼ�󷵻ص�·������ֱ���ڹ��캯���о�ʵ����
	// setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	// ��Ӷ���λ�õ�appͼ��
	QHBoxLayout* appUpLayout = new QHBoxLayout;
	appUpLayout->setContentsMargins(0, 0, 0, 0);  // �ĸ�����ļ�϶������Ϊ0(���ڲ���)
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png", "app_7"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
	appUpLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));  // Ƥ��

	// ���ò���֮��Ŀ�϶ -- TODO�� ��Ϊ���õ�ǰ�ļ�϶������Ͳ������ˣ���������Ҫ�ٽ��е���
	// appUpLayout->setSpacing(2);

	// ���ò��ֵ������λ��,���������ģʽ��ָ���˿�widget�������app���������ǿ���
	// ���������úõĲ������õ�����ָ����widget��Χ��
	ui.appWidget->setLayout(appUpLayout);

	// ��ӵײ�λ�õ�appͼ��,��Ϊ�������ģʽ��ʱ����ڴ�ŵײ�app��λ���Ѿ������
	// ���֣���������ֱ��ʹ��������֣������ڲ����btn�ؼ�����
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png", "app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));

	// ����ֱ����ʾ���ᵼ�µײ��Ŀؼ���϶̫��
	ui.bottomLayout_up->addStretch();    // ���һ����������������ͻᱻ�հ���䣬�ͻ��ý���

	// ��ʼ����ϵ��
	initContactTree();

	//����ǩ������װ�¼�����������ǰ������������
	ui.lineEdit->installEventFilter(this);
	// ������������װ�¼�������
	ui.searchLineEdit->installEventFilter(this);

	// ������������С����ť�͹رհ�ť
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]() {
		updateSearchStyle();    // �����������
	});

	SysTray* systray = new SysTray(this);   // ����ϵͳ����
}

void CCMainWindow::setUserName(const QString& username)
{
	// �����û���
	ui.nameLabel->adjustSize();        // �˷��������ñ�ǩ�������������Լ��ĳߴ�

	// �ı������ͽ���ʡ��(�����û����ֹ�������Ļ��ʾ���µ�ʱ������Ӧ�ý�����Ľ���
	// ʡ�ԣ���ʾ����...)

	// ���ô˷��������ص���QFontMetrics����󣬷��صĲ�ָֹ�룬���������������ݻ�,
	// ����                                 �ı���  ʡ�Ե�ģʽ(��Ҫָ��ʡ�Ժų��ֵ�λ��)������(���ı�����ָ�����ȣ���ô������ı�����...��ʾ)
	// ������ʡ���ı�֮��᷵��һ���ַ���
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());// ��ʾʡ���ı�

	// �����ı�
	ui.nameLabel->setText(name);   // ��ʡ�Ե��ı����õ���ǩ��
}

void CCMainWindow::updateSearchStyle()
{
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,100);border-bottom:1px solid rgba(%1,%2, %3,30)}\
                                             QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red())
		.arg(m_colorBackGround.green())
		.arg(m_colorBackGround.blue()));   // ��ɫ˳���ܷ�����Ϊ��rgb����һ��˳��
}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepId)
{
	// ����һ������
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");

	/*
	// ����ӽڵ�   ��0��                �ӽڵ�����
	pChild->setData(0, Qt::UserRole, 1);
	// ��UserRole + 1��һ��������Ψһ��ʶ���
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));
	*/
	// ����Ӧ���ӽڵ����ñ�ʾ���ݵ�ʱ�����ǾͲ�����Ҫʹ�õ�ֵַ�ˣ�����ֱ��ʹ��
	// ���õĲ���idֵ����
	pChild->setData(0, Qt::UserRole, 1);
	// ��UserRole + 1��һ��������Ψһ��ʶ���
	pChild->setData(0, Qt::UserRole + 1, DepId);

	// �����ݿ��ȡ��˾�����ŵ�ͷ��
	QPixmap groupPix;
	QSqlQuery queryPicture(QString("SELECT picture FROM table_department WHERE departmentID = %1").arg(DepId));
	// ִ��sql����õ�ͼƬ·��
	queryPicture.exec();
	queryPicture.next();   // ָ����һ����Ҳ���ǵ�һ��
	groupPix.load(queryPicture.value(0).toString());

	// ��ȡ��������
	QString strDepName;
	QSqlQuery queryDepName(QString("SELECT department_name FROM table_department WHERE departmentID = %1").arg(DepId));
	queryDepName.exec();
	queryDepName.next();
	strDepName = queryDepName.value(0).toString();
	
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);  // ���ڵ�Ϊ���ؼ�

	// ���ò���ͷ����Ҫ��ȡԲͷ�� -- �����ݿ�İ�֮��
	pContactItem->setPixmap(getRoundImage(groupPix, pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);

	/* ʹ�����ݿ�֮�����������޸ģ�����ʹ�ù̶���ͷ�����ʹ�ô����ݿ��л�ȡ��
	�û�ͷ�������ʾ
	pContactItem->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pix, pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(sDeps);
	*/

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// m_groupMap.insert(pChild, sDeps);  ʹ�����ݿ�֮����Ѿ�����Ҫ��
}
QString CCMainWindow::getHeadPicturePath()
{
	QString strPicturePath;

	if (!m_isAccountLogin)  // id�ŵ�¼
	{
		// ȥԱ�����в�ѯ����
		QSqlQuery queryPicture(QString("SELECT picture FROM table_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
		queryPicture.exec();
		queryPicture.next();

		// ֱ��ȡֵ
		strPicturePath = queryPicture.value(0).toString();
	}
	else  // �˺ŵ�¼ 
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


// ���õȼ�λͼ
void CCMainWindow::setLevelPixmap(int level)
{
	// ��ʼ���ȼ�λͼ -- ʹ��ָ���ĵȼ���ť�Ĵ�С�����õȼ�λͼ�Ĵ�С
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);                 // ��λͼ����͸��

	QPainter painter(&levelPixmap);                    // ͨ�������ڵȼ�λͼ�ϻ���
	// ��0,4������λ�ý��ȼ�ͼƬ����ȥ(��һ����ʾ�ȼ���ͼƬ�������Ǿ���ĵȼ�)
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

	//�ȼ�Ҳ�в�ͬ(��ͬ�ĵȼ���ʾ��ͼƬҲ��ͬ)������ʹ��������������ʾ�ȼ��ĸ�λ����ʮλ��
	int unitNum = level % 10;    // �ȼ��ĸ�λ��
	int tenNum = level / 10;    // �ȼ���ʮλ��

	// ������ĵȼ�
	// �Ȼ�10λ                      ��·���б�ʾ0-9�����еȼ�����������ֻ��Ҫһ����
	// ����������Ҫ��ͼƬ���д������Ƶ��е�һ����,ʹ��drawPixmap����һ������

	// ����������֮�����ĸ�λ�ã� ǰ���������������ͼƬ�����Ͻǣ�����������ָ��
	// ����ͼƬ�Ŀ�͸�
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		tenNum * 6,  // ����6����Ϊ׼������ԴͼƬ��ÿ������֮����6������
		0, 6, 7);

	// ����λ,��һ������Ϊ16������Ϊ��λ����ʮλ���棬���ּ��Ϊ6����
	painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),
		unitNum * 6, 0, 6, 7);

	// ���Ǽǰ�ť����ͼƬ
	ui.levelBtn->setIcon(levelPixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());  // ָ��ͼ��Ĵ�С�Ͱ�ťһ����
}

/*
  ����ͷ��
*/
void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;   // ����ͼƬ
	pix.load(":/Resources/MainWindow/head_mask.png");
	ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

/*
  ����״̬
*/
void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	QPixmap statusBtnPixmap(ui.stausBtn->size());   // ͨ��״̬λͼ��������
	statusBtnPixmap.fill(Qt::transparent);          // ��������Ϊ͸��

	QPainter painter(&statusBtnPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));

	// ����ť����ͼ��
	ui.stausBtn->setIcon(statusBtnPixmap);
	ui.stausBtn->setIconSize(ui.stausBtn->size());   // ���ð�ť�Ĵ�СΪͼ���С
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
	// �����ť������UI���ģʽ��ʱ��û�����ã����ڴ˺�����ָ���ģ�Ȼ�󷵻ع���İ�ť
	// �ڵ��÷�������������Ӧ��λ��(���ò��ֵȷ�ʽ)
	QPushButton* btn = new QPushButton(this);   // �Ե�ǰ������Ϊ�丸����
	btn->setFixedSize(20, 20);

	// ����һ��ͼ����󣬲����������С
	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	// ��ʼ����
	QPainter painter(&pixmap);
	// �����û������app��·������һ��ͼ��
	QPixmap appPixmap(appPath);
	// �������ݰ�ť�Ŀ��-ͼ��Ŀ��/2�õ��ݰ�ť���ߵļ�࣬Ҳ��������Ŀ�ʼ����֤��Ӧ
	// ͼ���ڰ�ť���м�
	painter.drawPixmap((btn->width() - appPixmap.width()) / 2,
		(btn->height() - appPixmap.height()) / 2,
		appPixmap);     // �ڻ���ָ���Ļ�ͼ�豸�Ͻ��л���
// ����Ӧ�İ�ť����ͼ��
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	// ����Ӧ�İ�ťappָ��һ�¶����� -- ��Ϊ�������ģʽ��û����ӣ���ʱ�����ڴ�����
	// ������ӣ�Ϊ���Ժ��ܹ���������ؼ�����Ҳ��ҪΪ������һ��������
	btn->setObjectName(appName);

	// ��������,����Ӧ����������Ϊtrue����ʾ�б߿������
	btn->setProperty("hasborder", true);

	// �����app��ʱ�����Ǿ���Ҫ����һЩ��Ӧ�Ĳ��������������źŲ�
	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

	return btn;  // ���ع����õİ�ť
}

void CCMainWindow::initTimer()
{
	// �����500ms����һ���ȼ�
	QTimer* timer = new QTimer(this);
	timer->setInterval(500);    // ����һ�����

	// �����źźͲۣ�ʱ�䵽�˾ͷ����źŽ��еȼ�������
	// �ź�һ�������ִ�����������е�����
	connect(timer, &QTimer::timeout, [this]() {  // lambda��ʹ��this���ⲿ����
		static int level = 0;                    // ʹ�þ�̬�ı�����ʾ�ȼ�
		level++;
		if (level == 99) level = 0;             // Ŀǰֻ������λ���ĵȼ�
		setLevelPixmap(level);                   // ���õȼ�ͷ��
	});

	timer->start();   // ������ʱ��
}

/*��ʼ����ϵ��*/
void CCMainWindow::initContactTree()
{
	// ������ʱ���չ����������������չ�����������ź�
	// ���� ��һ��: �������������һ��Լ���һ��
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	// չ��
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	// ����
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	// ˫��
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// ���ڵ� -  ��ʾ���ڶ��ٸ�Ⱥ��
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	// �������չ���Ĳ��� -- ��һ����ͷ��ʵ��
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	// ���ø��ڵ������
	pRootGroupItem->setData(0, Qt::UserRole, 0);   // ��0��
	// �������

	RootContactItem* pItem = new RootContactItem(true, ui.treeWidget);  // ������Ϊ������

	// ��ȡ��˾����id
	// �����ѯ�ָ࣬����ѯ��䣬��Ϊ���ǲ�ѯ���ֶ������ĵģ�����������Ҫʹ��String�ķ�������ת��ΪQt�ַ����ĸ�ʽ���ͱ��룬������������
	// ��ƥ�䲻���ˣ���Ҳ��Ϊʲô����ʹ���������ƽ��е�¼��ʱ���޷��ɹ���ԭ����Ϊ���Ҫʹ�����ľ���Ҫת��ΪQt�ı����ʽ��������

	QSqlQuery queryCompDepId(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryCompDepId.exec();
	queryCompDepId.first();
	int CompDep = queryCompDepId.value(0).toInt();  // �ӽ�����л�ȡ����ѯ���Ĺ�˾Ⱥidֵ����Ϊֵ��ѯ����1�У���ô��ֱ�Ӳ�ѯ��0��

	// ��ȡqq��¼�����ڵĲ���id(����Ⱥ��)  --  �˴�ʹ��Ա����idҲ����qq��ȥ���в�ѯ������������ǵ�¼�ķ�ʽ������
	// �������Ƕ���һ��ȫ�ֱ�����¼�û���id�ţ��������Ǵ˴�ȥ��ѯʹ��
	// ����û��������id����ôֱ�Ӷ�ȡ��ֵ�������У�����������������Ϊ�˺ţ�����ȥ���в�ѯ��Ӧ��idȻ���¼�ڱ���
	// ���У�����ֻ��Ҫ�ڵ�¼��֤��ʱ�򣬸��ݲ�ͬ���������������ȫ�ֱ�������
	QSqlQuery querySelfDepId(QString("SELECT departmentID FROM table_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
	querySelfDepId.exec();
	querySelfDepId.first();
	int SelfDepID = querySelfDepId.value(0).toInt();  // �ӽ�����л�ȡ����ѯ���Ĳ���idֵ����Ϊ

	// �ڴ˴�������Ϊ����ÿ����½��ֻ������������ţ�������ֻ��Ҫ����������(Ⱥ)����
	addCompanyDeps(pRootGroupItem, CompDep);
	addCompanyDeps(pRootGroupItem, SelfDepID);

	// ��������ӵ�������
	// ����һ���ı������ַ���SelfDepID�ӱ��ر���ת��ΪUnicode����
	QString strGroupName = QString::fromLocal8Bit("��ϵ��");
	pItem->setText(strGroupName);    // �����ı�

	// �������ڵ�  ��Ӷ�����
	ui.treeWidget->addTopLevelItem(pRootGroupItem);
	// ����:                     ���ڵ�          ��  ���õĲ���
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItem);   // �������

    /*�������ݿ����Ϣ����ȡ�����ж϶�Ӧ�û����ڵ�Ⱥ��*/
	// ��ͬ���˶�����ʾ��˾Ⱥ������ÿ�����ִ��ڲ�ͬ�Ĳ��ţ����Ƕ��ڵ�¼���˶���
	// ����ֻ��Ҫ��ʾ�����ڵĲ���Ⱥ���У�û��Ҫÿ�����Ŷ���ʾ������ÿ���˶�Ӧ��
	// ��ʾ��˾Ⱥ��������ʾ�Ǹ����ţ��Ϳ����ݿ�����洢���������ĸ�������



	/*
	Ҫ������ݿ���ʵ���ⲿ�ֹ�����
	// ʵ��˫��չ����������4��
	QStringList sCompDeps;
	sCompDeps << QString::fromLocal8Bit("��˾Ⱥ");
	sCompDeps << QString::fromLocal8Bit("���²�");
	sCompDeps << QString::fromLocal8Bit("�з���");
	sCompDeps << QString::fromLocal8Bit("�г���");

	// ��ӵ���������ȥ
	for (int nIndex = 0; nIndex < sCompDeps.length(); nIndex++) {
		addCompanyDeps(pRootGroupItem, sCompDeps.at(nIndex));
	}
	*/
}

// ��app�İ�ť����������ǽ���һ���������ʲôapp������Ӧʲô����
void CCMainWindow::onAppIconClicked() {
	// Ƥ��app�����֮�󣬵���Ƥ������
	// �ж��źŷ����߶������Ƿ���app_skin
	if (sender()->objectName() == "app_skin") {
		SkinWindow* skinWindow = new SkinWindow;   // ����һ��skinWindow
		skinWindow->show();                        // ��ʾ����
	}
}
