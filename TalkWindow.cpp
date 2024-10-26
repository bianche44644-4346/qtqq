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

// ʹ�����ݿ�֮����������˸ı�
TalkWindow::TalkWindow(QWidget* parent, const QString& uid/*, GroupType groupType*/)
	: QWidget(parent),
	m_talkId(uid)    // ���ݴ�����uid������id��ʼ��
	// m_groupType(groupType)  -- ʹ�����ݿ�֮���������Ҳ����Ҫ��
{
	ui.setupUi(this);

	// �ڹ������촰�ڵ�ʱ��Ҳ�������Ǵ��˶�Ӧ�Ĵ��ڣ�����ֱ�����乹�캯��(���촰��
	// ��ʱ��)����Ӧ��ӳ����Ϣ��ӵ��ڲ����������ǶԴ򿪵Ĵ�����й���
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);

	// ������ݿ�֮��
	initGroupTalkStatus();   // �ڳ�ʼ���ؼ���ʱ���Ⱥ��״̬�����������ã���Ϊ��ʼ���ؼ���ʱ��Ҫ�õ���
	initControl();
}

TalkWindow::~TalkWindow()
{
	// �ر����촰�ڵ�ʱ��Ӧ��ɾ��������촰�ڵ���Դ�����ͷ���Դ�������Ӧ�����⣬ռ����Դ�ȣ���Ȼ������ڴ�й¶
	WindowManager::getInstance()->deleteWindowName(m_talkId);  // �����������Ҫ�ͷ��Ǹ����ڵ����֣�������һ��˽�г�Ա��Ŷ�Ӧ�Ĵ��������
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	// ��ӱ���ͼƬ
	ui.textEdit->setFocus();    // �ı��༭����ȡ����
	// ���˽���֮��ֱ����ӱ���,ֱ�Ӵ����������
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);   // �����ֱ�ǩ�����ı�
}

QString TalkWindow::getTalkId() const
{
	return m_talkId;
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// ��˫��Ⱥ���촰�ڵ�Ⱥ��Ա���ʱ����ô�ͽ��뵥������
	// �жϵ����������Ǹ�����������ô����ʾ�������촰��
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {
		// ��ȡ��Ӧ���Ĵ��ڵ�����  --  ����Ⱥ���ӳ��ֵ����ȡ
		QString peopleName = m_groupPeopleMap.value(item);   // ��ȡ��Ӧ�������
		// ����1 :  ����ѡ�е����û��������õ��Ĵ��ڵ�id
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()/*, PTOP, peopleName*/);  // ʹ�����ݿ�����޸�
	}
}

void TalkWindow::initControl()
{
	// �������ұߵĴ������óߴ磬�����ߴ������һ���б�������
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;
	// ��������ˮƽ�ģ���ô�Ͱ���������Ĵ�������ˮƽ�Ŀ��
	// ��ֱ�ģ��Ͱ���������Ĵ������ô�ֱ�ĸ߶�
	ui.bodySplitter->setSizes(rightWidgetSize);   // ���óߴ�Ĳ�����һ���б�

	ui.textEdit->setFontPointSize(10);            // �����ı������������
	ui.textEdit->setFocus();                      // ���ý��㣬�����û��Ϳ���ֱ�Ӵ򿪾Ϳ��������ˣ����������֮��������

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));
	connect(ui.sendBtn, &QPushButton::clicked, this, &TalkWindow::onSendBtnClicked);

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	// �ļ��򿪰�ť�������Ȼ�󴥷���Ӧ�Ĵ���ۺ��� --��Ϊ�ļ����Ͱ�ť�������ǵ�talkwindow����֮�µģ����Է����ź�ʵ��talkwindow�з���
	// Ȼ��talkwindow�еĲۺ������ļ����ڹ��������֮��Ĳ��������ļ������еĴ�����
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	/*�������ݿ�֮���ڴ˴��ж�һ�µ�ǰ��������Ⱥ�Ļ��ǵ�������*/
	if (m_isGroupTalk) {  // ��Ⱥ�ĳ�ʼ��Ⱥ�ģ������ʼ������
		initTalkWindow();
	}
	else {
		initPtoPTalk();
	}

	/* ʹ�����ݿ�֮��������޸�
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
	default:   // ��������
	{
		initPtoPTalk();        // ��ʼ������ʹ��
		break;
	}
	}
	*/
}

// �������ݿ�֮��
void TalkWindow::initGroupTalkStatus()
{
	// �������ǵ������Ĵ��ڵĲ���id�������ݿ���ȥ���ң������ҵ�����Ⱥ�Ļ��ǵ���
	// �����Ⱥ�ĵĻ������ǲ���id������ǵ�������Ļ������û�id
	
	QSqlQueryModel sqlDepartmentModel;
	/* ʹ������ѡ��Ĵ��ڵ�idȥ���ű����ң������������˵����Ⱥ�ģ����û���Ǿ��ǵ���*/
	QString strSql = QString("SELECT * FROM table_department WHERE departmentID = %1").arg(m_talkId.toInt());

	sqlDepartmentModel.setQuery(strSql);
	
	// �ж��к�
	int rows = sqlDepartmentModel.rowCount();
	if (rows == 0) {  // ��������
		m_isGroupTalk = false;
	}
	else { // Ⱥ��
		m_isGroupTalk = true;
	}
}

/*ʹ�����ݿ�֮����ӵĺ���*/
int TalkWindow::getCompDepId()
{
	QSqlQuery queryDepId(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryDepId.exec();    // ִ��
	queryDepId.next();    // ��ǰ����һ�����ǵ�һ��

	return queryDepId.value(0).toInt();  // ���ز鵽�Ĺ�˾id
}

/* �������ݿ�֮�����ǾͲ���Ҫ����Ӧ��Ⱥ���г�ʼ���ˡ�ֻ��Ҫ��һ����ʼ��Ⱥ�ĵĺ���
   �Ϳ����ˡ�
void TalkWindow::initCompanyTalk()
{
	// �����������ĸ���
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // ���ú����ǵ�ָʾ������
	// ����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell�߶� - shell��ͷ��(talkwindow titlewindow�ĸ߶�)
	ui.treeWidget->setFixedHeight(646);   // ���ù̶��߶�ֵ talkwindowshell�Ĵ����С730 - talkwindow��ͷ84�ĸ߶� = ��ǰָ�������� 

	int nEmployeeNum = 50;   // ��ǰָ��һ����̬�����ݣ�Ա������Ϊ50��
	QString qsGroupName = QString::fromLocal8Bit("��˾Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);      // ��Ӹ���
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // ������Ŀ�Ĳ���

	// չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // ��װһ������ʵ��
	}
}

void TalkWindow::initPersonelTalk()
{
	// �����������ĸ���
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // ���ú����ǵ�ָʾ������
	// ����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell�߶� - shell��ͷ��(talkwindow titlewindow�ĸ߶�)
	ui.treeWidget->setFixedHeight(646);   // ���ù̶��߶�ֵ talkwindowshell�Ĵ����С730 - talkwindow��ͷ84�ĸ߶� = ��ǰָ�������� 

	int nEmployeeNum = 5;   // ��ǰָ��һ����̬�����ݣ�Ա������Ϊ50��
	QString qsGroupName = QString::fromLocal8Bit("����Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);      // ��Ӹ���
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // ������Ŀ�Ĳ���

	// չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // ��װһ������ʵ��
	}
}

void TalkWindow::initMarketTalk()
{
	// �����������ĸ���
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // ���ú����ǵ�ָʾ������
	// ����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell�߶� - shell��ͷ��(talkwindow titlewindow�ĸ߶�)
	ui.treeWidget->setFixedHeight(646);   // ���ù̶��߶�ֵ talkwindowshell�Ĵ����С730 - talkwindow��ͷ84�ĸ߶� = ��ǰָ�������� 

	int nEmployeeNum = 8;   // ��ǰָ��һ����̬�����ݣ�Ա������Ϊ50��
	QString qsGroupName = QString::fromLocal8Bit("�г�Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);      // ��Ӹ���
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // ������Ŀ�Ĳ���

	// չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // ��װһ������ʵ��
	}
}

void TalkWindow::initDevelopTalk()
{
	// �����������ĸ���
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // ���ú����ǵ�ָʾ������
	// ����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell�߶� - shell��ͷ��(talkwindow titlewindow�ĸ߶�)
	ui.treeWidget->setFixedHeight(646);   // ���ù̶��߶�ֵ talkwindowshell�Ĵ����С730 - talkwindow��ͷ84�ĸ߶� = ��ǰָ�������� 

	int nEmployeeNum = 30;   // ��ǰָ��һ����̬�����ݣ�Ա������Ϊ50��
	QString qsGroupName = QString::fromLocal8Bit("�з�Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);      // ��Ӹ���
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // ������Ŀ�Ĳ���

	// չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++) {
		addPeopleInfo(pRootItem);        // ��װһ������ʵ��
	}
}
*/

/*ʹ�����ݿ�֮����� -- ��ʼ��Ⱥ��*/
void TalkWindow::initTalkWindow()
{
	// �����������ĸ���
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);     // ���ú����ǵ�ָʾ������
	// ����data���������ָ�������
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	// shell�߶� - shell��ͷ��(talkwindow titlewindow�ĸ߶�)
	ui.treeWidget->setFixedHeight(646);   // ���ù̶��߶�ֵ talkwindowshell�Ĵ����С730 - talkwindow��ͷ84�ĸ߶� = ��ǰָ�������� 

	// ��ǰ�����Ⱥ�������������� -- �����ݿ��в�ѯ
	QString strGroupName;
	QSqlQuery queryGroupName(QString("SELECT department_name FROM table_department WHERE departmentID = %1").arg(m_talkId));
	queryGroupName.exec();
	if (queryGroupName.next())  // ָ�����ݿ��һ���ɹ���Ҳ���ǲ�ѯ������д�������
	{
		strGroupName = queryGroupName.value(0).toString();
	}

	// ��ȡ����    --  ��ʵ���Ǵ����ݿ���ȥ��ѯ(ֱ����Ա�����в�ѯ�����ݶ�Ӧ�Ĳ��źţ����������¼��ô���м�����)
	// ��������ǹ�˾Ⱥ�����ǵĲ��ű��У���û�в��źŶ�Ӧ�����������Ƕ����������Ĵ���

	
	
	// �����ݿ���ȡ���ݣ�������д��
	// int nEmployeeNum = 30;   // ��ǰָ��һ����̬�����ݣ�Ա������Ϊ50��

	QSqlQueryModel queryEmployeeModel;
	if (getCompDepId() == m_talkId.toInt())   // Ⱥ���ǹ�˾Ⱥ
	{
		/*��ѯԱ����id���жϵ�ǰ�ж�������Ⱥ�У���Ȼ����Ѿ�ע�����˻���û�б�Ҫ��ʾ��
		  �������ǲ�ѯ��¼����������status = 1 ���У���Ϊ=0�ľͱ�ע���� ����ʾ*/
		queryEmployeeModel.setQuery("SELECT employeeID FROM table_employees WHERE status = 1");
	}
	else {  // ��ͨ��Ⱥ
		// ������ͨ��Ⱥ������ֻ��Ҫ���Ҷ�Ӧ��Ⱥ�����м�����¼�����ˣ�����Ҫ���ݲ��źŲ�ѯ��ͬ��ҲҪ��û��ע�����û�
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM table_employees WHERE status = 1 AND departmentID = %1").arg(m_talkId));
	}

	// �жϽ�������ж����У��жϳ�Ա��������
	int nEmployeeNum = queryEmployeeModel.rowCount();

	QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3")
		.arg(strGroupName)
		.arg(0)
		.arg(nEmployeeNum);


	/* ʹ�����ݿ�������޸�
	QString qsGroupName = QString::fromLocal8Bit("�з�Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
	*/
	pItemName->setText(qsGroupName);

	//�������ڵ�
	ui.treeWidget->addTopLevelItem(pRootItem);      // ��Ӹ���
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);    // ������Ŀ�Ĳ���

	// չ��
	pRootItem->setExpanded(true);

	// �������
	for (int i = 0; i < nEmployeeNum; i++) {
		// ��ӳ�Ա������Ϣ -- ���ܺ�֮ǰд����һ���ˣ���Ϊ������ӵ�ÿһ���Ա������Ϣ
		// addPeopleInfo(pRootItem);        // ��װһ������ʵ��
		// ��Ϊ����ǰ���Ѿ�����Ӧ�����ݶ���ѯ����ģ�͵��У���������ʹ��ģ��������ģ���ж�ȡ����
		QModelIndex modelIndex = queryEmployeeModel.index(i,0);  // ��ȡ��ǰ�ж�Ӧ��ģ��������ͨ��ģ��������ȡ����
		int employeeID = queryEmployeeModel.data(modelIndex).toInt();

		// ����Ա��id�����
		addPeopleInfo(pRootItem,employeeID); // �ഫ��һ��Ա��id
	}
}

// ���������죬���ǲ���Ҫ��ʾ������ֱ����ʾһ��ͼƬ����
void TalkWindow::initPtoPTalk()
{
	QImage pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png"); // ����ͼ��

	ui.widget->setFixedSize(pixSkin.size());    // �ұߴ���ΪͼƬ��С
	// pixSkin.scaled(ui.widget->size());
	 
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(QPixmap::fromImage(pixSkin));
	skinLabel->setFixedSize(ui.widget->size());
}

void TalkWindow::addPeopleInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	// ���Ա����Ϣ
	QTreeWidgetItem* pChild = new QTreeWidgetItem;   // ����һ������

	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");  // ����ͼƬ

	// ����ӽڵ�
	pChild->setData(0, Qt::UserRole, 1);
	// pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild)); ʹ�����ݿ�֮������޸ģ�֮ǰ����ʹ�õ��Ǵ��ڵ�id��ʹ�õ��ǵ�ַ����ʾid��
	pChild->setData(0, Qt::UserRole + 1,employeeID); // ����ӽڵ�ı�ʾ��Ա����id(�������Ⱥ�ڲ���Ա��id������ʹ��Ա����)

	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	// �����ݿ��л�ȡ����ǩ����ͷ��
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;  // ��ѯԱ����Ϣ
	// ���ݴ����Ա��id����ѯ��ӦԱ�����������
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM table_employees WHERE employeeID = %1").arg(employeeID));
	
	QModelIndex nameIndex, signIndex, pictureIndex;  // ʹ��ģ��������ȡ����
	nameIndex = queryInfoModel.index(0, 0);   // 0��0�е�������������ȡ��Ӧλ�õ�����
	signIndex = queryInfoModel.index(0, 1);
	pictureIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();  // ����
	strSign = queryInfoModel.data(signIndex).toString();  // ǩ��
	strPicturePath = queryInfoModel.data(pictureIndex).toString(); // ͼƬ·��

	// ʹ�����ݿ�֮��������޸�
	// const QPixmap image(":/Resources/MainWindow/girl.png"); //��constΪ�˺��洫����
	QImage imageHead;   // ����ͼƬ
	imageHead.load(strPicturePath);
	
	// ���þ�̬���ݽ�����ʾ
	// static int i = 0; ʹ�����ݿ���иı�
	// pContactItem->setPixmap(CommonUtils::getRoundImage(image, pix1, pContactItem->getHeadLabelSize()));
	pContactItem->setPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));
	
	// vs�´������ĵ�ʱ������ʹ��fromLocal8Bit����һ�±����ת������������
	// pContactItem->setUserName(QString::fromLocal8Bit("����%1").arg(i++));  ʹ�������޸�
	pContactItem->setUserName(strName);

	// ʹ�����ݿ�����޸�
	// pContactItem->setSignName(QString::fromLocal8Bit(""));  // ����ǩ��Ϊ�գ�ֻ��Ч���Ͳ�������
	pContactItem->setSignName(strSign);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	// Ϊ���ܹ�ʶ���Ӧ�Ĵ��壬���Ƕ���һ�������id��һ��ӳ��
	QString str = pContactItem->getUserName();  // ��ȡ���û�����
	m_groupPeopleMap.insert(pChild, str);       // ���û�������ӵ�ӳ�䵱��
}


void TalkWindow::onSendBtnClicked(bool clicked) {
	// ���Ͱ�ť����� 
	// ���ı��༭��ת��Ϊ���ı����жϴ��ı��Ƿ�Ϊ�գ�Ҳ���Ƿ��͵�ʱ���ı�������û�����ݣ������ݲŷ���
	if (ui.textEdit->toPlainText().isEmpty()) {
		// Ϊ�ս���һЩ��ʾ,��ʾ�ı�
		// ���ȴ���������ı���λ����Ϣ������,����QPoint��ȡ������Ե�λ�ã�Ҫ����Ϊȫ�ֵ�����
		// ȫ�������������������ϽǾͲ������촰������Ͻ���
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)), QString::fromLocal8Bit("���͵���Ϣ����Ϊ��"),
			this, QRect(0, 0, 120, 100), 2000);  // ������Ϊ��ǰ�Ĵ��壬������ʾ����εĴ�С,����ֻ��ʾ2000ms
		return;  // ֱ�ӷ��ؿվ��У���Ϊ��ǰ�����û���ı������
	}

	// �������ı������
	// ��ȡ�ı��༭�����ĵ����ݣ���ȡ�䵱ǰ�ĵ�ת��Ϊhtml����ΪҪ������������ҳ������ʾ
	QString html = ui.textEdit->document()->toHtml();   // �����ַ������������ݱ���

	// �ı�htmlû��������������
	// ����ͼƬ��ͬʱ�������ı�
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
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�ļ�������"));
			return;
		}

		if (!html.contains(fontHtml)) {
			html.replace(text, fontHtml);
		}
	}

	// ��ʽת����֮�󣬾��ܹ�׷����Ϣ��
	// ����Ϣ����֮����ʾ����ô���Ƿ�����Ϣ���ı��༭���Ͳ���Ҫ������ʾ��Ϣ��
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();  // ��Ϊ������ı���һ�������鴴���Ƕ�̬���ٿռ�ģ�����
										   // �ı�����ɾ��֮�󣬻���Ҫ������ռ�õ��ڴ棬���ǽ����˷�װ
	ui.msgWidget->appendMsg(html);		   // msgWidget�ǽ�������֮���

}

void TalkWindow::onFileOpenBtnClicked(bool flag) {
	// ���췢���ļ��ĶԻ���Ȼ���������ʾ
	SendFile * sendFile = new SendFile(this);  // ���ǵ�����ļ�֮�󣬶�Ӧ��Ҫ�����ļ��Ŀ��ӻ��ؼ�����Ϊ��Ӧ��ťʵ��talkwindow�����µģ����Բۺ���ҲӦ���ڴδ�����
	sendFile->show();    // ��ʾ����
}
