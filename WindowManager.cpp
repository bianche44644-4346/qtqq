#include "WindowManager.h"
#include "TalkWindowItem.h"
#include "TalkWindow.h"
#include  <QSqlQueryModel>

// ����ģʽ������ȫ�־�̬����
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
	// ���ݴ���Ĵ������֣������Ǵ�����ӳ���в��ң�����ҵ��ͷ��ض�Ӧ�Ĵ��壬���û����ô�ͷ���nullptr
	if (m_windowMap.contains(qsWindowName)) {
		return m_windowMap.value(qsWindowName);
	}

	return nullptr;
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	// ֱ�Ӵ�ӳ���н���ɾ��
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	// ���ӳ����û�ж�Ӧ�Ĵ���������ô�ͽ������
	if (!m_windowMap.contains(qsWindowName)) {
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{
	return theInstance();    // ���ص�ǰ��ʵ��
}

// ������ݿ�֮��������������˱仯��Ҳ���Ƕ�Ӧ�Ĳ���
void WindowManager::addNewTalkWindow(const QString& uid/*, GroupType groupType, const QString& strPeople*/)
{
	// ����µ����촰��,��������һ�����ʱ��TalkWindowShell�����ȹ������������Ӧ���Ƚ��й���
	if (m_talkwindowshell == nullptr) {
		m_talkwindowshell = new TalkWindowShell;    // û�о͹���
		// ���������ڽ��йرյ�ʱ������Ҳ�����ָ��Ҳ��������
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* object) {
			m_talkwindowshell = nullptr;     // �ر�֮�󣬽�����������Ϊ��
		});
	}

	// �������ݿ�֮������ǵ�������ͱ�ʾ��ӦԱ����id�ţ�Ⱥ�ľ���Ⱥ��
	QWidget* widget = findWindowName(uid);  // ����Ψһ�ļ����в���

	// ���widgetû�У���ô���Ǿͽ��д����������˵�������Ѿ��򿪹��ˣ����Ծ�ֱ�Ӷ���Ӧ�Ĵ��ڽ���ϵ���ǾͿ�����
	if (!widget) {
		m_strCreatingTalkId = uid;

		// �������ݿ�֮��TalkWindow�Ĳ���Ҳ�����˱仯
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid/*, groupType)*/);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkwindow);

		m_strCreatingTalkId = "";  // ����Ļ�����Ϊ��

 		// ���ݿ�֮����޸�
		// �ж���Ⱥ�Ļ��ǵ���
		QSqlQueryModel sqlDepModel;   // ���ݿ��ѯģ�� -- �����жϲ�ѯ����Ƿ��м�¼

		// ����ж���Ⱥ�Ļ��ǵ��ģ���������uidȥ����Ⱥ��ȥ��ѯ����������ҵ���ô
		// ����Ⱥ��
		QString strSql = QString("SELECT department_name,sign FROM table_department WHERE departmentID = %1").arg(uid.toInt());
		sqlDepModel.setQuery(strSql);  // ����ѯģ������sql���
		int rows = sqlDepModel.rowCount();  // �жϲ�ѯ���м��У����==0���Ǿ��ǵ�������

		QString strWindowName, strMsgLabel;

		if (rows == 0) {  // ����
			// ��ȡ�û���ǩ��
			QString sql = QString("SELECT  employee_name,employee_sign FROM table_employees WHERE employeeID = %1").arg(uid);
			sqlDepModel.setQuery(sql);   // ���ò�ѯ���
		}

		// ����ģ����������ΪҪ��ģ����ȡ����
		QModelIndex indexDepIndex, signIndex;  // ����������ǩ������
		indexDepIndex = sqlDepModel.index(0, 0);  // ����������ģ�͵ĵ�0�е�0��
		signIndex = sqlDepModel.index(0, 1);

		// ���ô������֣�ģ�͵���data����������ģ��������������
		strWindowName = sqlDepModel.data(signIndex).toString();
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

        // ���ô���
		talkwindow->setWindowName(strWindowName);  // ��������
		talkWindowItem->setMsgLabelContent(strMsgLabel);  // ��ϵ���ı���ʾ

		// ������촰�� -- ����uid���ж��ǵ��Ļ���Ⱥ������Ӷ�Ӧ�Ĵ���
		m_talkwindowshell->addTalkWindow(talkwindow, talkWindowItem, uid);

		/* �����������ݿ�֮������Ҳ����Ҫ��
		switch (groupType) {
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("�ڹ�˾"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("��˾Ⱥ"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("������"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("����Ⱥ"));
			break;
		}
		case MARKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("���г�"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("�г�Ⱥ"));
			break;
		}
		case DEVELOPMENTGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("�ڿ���"));
			talkWindowItem->setMsgLabelContent(QStringLiteral("����Ⱥ"));
			break;
		}
		case PTOP:
		{
			talkwindow->setWindowName(QStringLiteral(""));    // ���Ĳ����ô�����
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
		// ������൱ǰ�����촰��
		// ��ȡ�������ֵ
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);    // �����б���
		item->setSelected(true);  // ����ǰ������Ϊ��ѡ��

		// �����Ҳ൱ǰ�����촰��
		// ��Ч���ͽ���Ӧ������Ϊ��ǰ�����촰
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();            // �Դ��������ʾ
	m_talkwindowshell->activateWindow();  // ����Ϊ��ǰ�Ļ����
}

TalkWindowShell * WindowManager::getTalkWindowShell() const
{
	return m_talkwindowshell;
}

QString WindowManager::getCreatingTalkId()
{
	return m_strCreatingTalkId;
}
