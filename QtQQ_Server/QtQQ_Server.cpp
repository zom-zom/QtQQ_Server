#include "QtQQ_Server.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QFileDialog>
#include <QHostAddress>


const int gTcpPort = 8888;
const int gUdpPort = 6666;

#define TCP_PORT 8888
#define UDP_PORT 6666



QtQQ_Server::QtQQ_Server(QWidget *parent)
    : QDialog(parent)
	, m_pixPath("")
{
    ui.setupUi(this);
	if (!connectMySql())
	{
		QMessageBox::warning(NULL, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�������ݿ�ʧ��!"));
	}

	//��ͷ����ʾ����ǩ
	QPixmap pixmap;
	pixmap.load(":/QtQQ_Server/boyImage.png");
	qreal widthRatio = (qreal)ui.label->width() / (qreal)pixmap.width();
	qreal heightRatio = (qreal)ui.label->height() / (qreal)pixmap.height();

	QSize size(pixmap.width() * widthRatio, pixmap.height() * heightRatio);
	ui.label->setPixmap(pixmap.scaled(size));

	setDepNameMap();
	setStatusMap();
	setOnlineMap();
	initComboBoxData();

	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);	//���༭�����д����ﵽֻ��Ч��
	//m_queryInfoModel.setQuery("SELECT * FROM tab_employees");
	
	//��ʼ����ѯ��˾Ⱥ����Ա����Ϣ
	m_depID = getCompDepID();
	m_compDepID = m_depID;
	m_employeeID = 0;

	
	updateTableData();

	//��ʱˢ������
	m_timer = new QTimer(this);
	m_timer->setInterval(200);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &QtQQ_Server::onRefresh);



    initTcpSocket();
	initUdpSocket();

}

QtQQ_Server::~QtQQ_Server()
{}

void QtQQ_Server::initComboBoxData()
{
	QString itemText;		//��Ͽ�����ı�

	//��ȡ��˾�ܵĲ�����
	QSqlQueryModel queryDepModel;
	queryDepModel.setQuery("SELECT * FROM tab_department");
	int depCounts = queryDepModel.rowCount() - 1;//"��˾Ⱥ"��������˾�ķ�֧����
	
	for (int i = 0; i < depCounts; i++)
	{
		itemText = ui.employeeDepBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//����Ա������������Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.employeeDepBox->setItemData(i, queryDepID.value(0).toInt());
	}
	
	//��һ����˾Ⱥ
	for (int i = 0; i < depCounts + 1; i++)
	{
		itemText = ui.departmentBox->itemText(i);
		QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(itemText));
		queryDepID.exec();
		queryDepID.first();

		//���ò�����Ͽ������Ϊ��Ӧ�Ĳ���QQ��
		ui.departmentBox->setItemData(i, queryDepID.value(0).toInt());
	}

}

void QtQQ_Server::initTcpSocket()
{
    m_tcpServer = new TcpServer(gTcpPort);
    m_tcpServer->run();
    
    //�յ�tcp�ͻ��˷���������Ϣ�����udp�㲥
    connect(m_tcpServer, &TcpServer::signalTcpMsgComes, this, &QtQQ_Server::onUDPbroadMsg);
}

void QtQQ_Server::initUdpSocket()
{
	m_udpSender = new QUdpSocket(this);
}

void QtQQ_Server::onUDPbroadMsg(QByteArray& btData)
{
	for (quint16 port = gUdpPort; port < gUdpPort + 200; port++)
	{
		m_udpSender->writeDatagram(btData, btData.size(), QHostAddress::Broadcast, port);
	}

}


void QtQQ_Server::onRefresh()
{
	updateTableData(m_depID,m_employeeID);
}

void QtQQ_Server::on_queryDepartmentBtn_clicked()
{
	ui.queryIDLineEdit->clear();
	m_employeeID = 0;
	m_depID = ui.departmentBox->currentData().toInt();
	updateTableData(m_depID);
}

void QtQQ_Server::on_queryIDBtn_clicked()
{
	ui.departmentBox->setCurrentIndex(0);
	m_depID = m_compDepID;
	
	//���Ա��qq���Ƿ�����
	if (!ui.queryIDLineEdit->text().length())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("������Ա��QQ�ţ�"));
		ui.queryIDLineEdit->setFocus();
		return;
	}

	//��ȡ�û������Ա��qq��
	int employeeID = ui.queryIDLineEdit->text().toInt();

	//��������Ա��qq�źϷ���
	QSqlQuery queryInfo(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	queryInfo.exec();
	if (!queryInfo.next())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("��������ȷ��Ա��QQ�ţ�"));
		ui.queryIDLineEdit->setFocus();
		return;
	}
	else
	{
		m_employeeID = employeeID;
		updateTableData(m_employeeID);
	}



}

void QtQQ_Server::on_logoutBtn_clicked()
{
	ui.queryIDLineEdit->clear();
	ui.departmentBox->setCurrentIndex(0);

	//���Ա��qq���Ƿ�����
	if (!ui.logOutIDLineEdit->text().length())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("������Ա��QQ�ţ�"));
		ui.logOutIDLineEdit->setFocus();
		return;
	}

	//��ȡ�û������Ա��qq��
	int employeeID = ui.logOutIDLineEdit->text().toInt();
	//��������Ա��qq�źϷ���			����ͬʱ˳���������
	QSqlQuery queryInfo(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	queryInfo.exec();
	if (!queryInfo.next())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("��������ȷ��Ա��QQ�ţ�"));
		ui.logOutIDLineEdit->setFocus();
		return;
	}
	else
	{
		//ע������ �������ݿ����ݣ���Ա��״̬(status)����Ϊ0
		QSqlQuery sqlUpdate(QString("UPDATE tab_employees SET status = 0 WHERE employeeID = %1").arg(employeeID));
		sqlUpdate.exec();

		//��ȡע����Ա������
		QString strName = queryInfo.value(0).toString();
		QMessageBox::information(this, QString(u8"��ʾ"), QString(u8"Ա�� %1 ��qq:%2 �ѱ�ע����").arg(strName).arg(employeeID));

		ui.logOutIDLineEdit->clear();
	}

}



void QtQQ_Server::on_selectPictureBtn_clicked()
{
	m_pixPath = QFileDialog::getOpenFileName(this, QString(u8"ѡ��ͷ��"), ".", "*.png;*.jpg;*.jpeg");
	if (!m_pixPath.size())
	{
		return;
	}

	//��ͷ����ʾ����ǩ
	QPixmap pixmap;
	pixmap.load(m_pixPath);
	qreal widthRatio = (qreal)ui.headLabel->width() / (qreal)pixmap.width();
	qreal heightRatio = (qreal)ui.headLabel->height() / (qreal)pixmap.height();

	QSize size(pixmap.width() * widthRatio, pixmap.height() * heightRatio);
	ui.headLabel->setPixmap(pixmap.scaled(size));

}

void QtQQ_Server::on_addBtn_clicked()
{
	//���Ա������������
	QString strName = ui.nameLineEdit->text();
	if (!strName.size())
	{
		QMessageBox::information(this, u8"��ʾ", u8"������Ա��������");
		ui.nameLineEdit->setFocus();
		return;
	}
	if (!m_pixPath.size())
	{
		QMessageBox::information(this, u8"��ʾ", u8"��ѡ��Ա��ͼƬ·����");
		return;
	}
	
	

	//���ݿ�����µ�Ա������
	//��ȡԱ��qq�� ���ֵ�Ա��һ
	QSqlQuery maxEmployeeID("SELECT MAX(employeeID) FROM tab_employees");
	maxEmployeeID.exec();
	maxEmployeeID.next();

	int employeeID = maxEmployeeID.value(0).toInt() + 1;

	//Ա������qq��
	int depID = ui.employeeDepBox->currentData().toInt();

	//ͼƬ·����ʽ����Ϊ xxx\xxx\xxx.png
	m_pixPath.replace("/", "\\\\");


	QSqlQuery insertSql(QString("INSERT INTO tab_employees(departmentID,employeeID,employee_name,picture) \
		VALUES(%1,%2,'%3','%4')").arg(depID).arg(employeeID).arg(strName).arg(m_pixPath));

	insertSql.exec();
	QSqlQuery queryIsAdd(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	if (!queryIsAdd.next())
	{
		QMessageBox::information(this, u8"��ʾ", u8"����Ա��ʧ�ܣ�");
		return;
	}
	QMessageBox::information(this, u8"��ʾ", u8"����Ա���ɹ���");
	
	//��һ�����Ա��ʱ    ���������ͼƬ·�� �Լ�����
	m_pixPath.clear();
	ui.nameLineEdit->clear();

}

bool QtQQ_Server::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");		//���ݿ�����
	db.setHostName("localhost");	//������
	db.setUserName("root");			//�û���
	db.setPassword("hdw123");		//����
	db.setPort(3306);				//�˿�

	if (db.open())
		return true;

	return false;
}

void QtQQ_Server::updateTableData(int depID /*= 0*/, int employeeID /*= 0*/)
{
	ui.tableWidget->clear();
	
	if (depID && depID != m_compDepID)	//��ѯ����
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE departmentID = %1").arg(depID));

	}
	else if (employeeID)	//��ȷ����
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	}
	else	//��ѯ����
	{
		m_queryInfoModel.setQuery(QString("SELECT * FROM tab_employees"));
	}

	int rows = m_queryInfoModel.rowCount();	//������
	int columns = m_queryInfoModel.columnCount();	//������
	QModelIndex index;	//ģ����������ȡ����

	//���ñ�������������
	ui.tableWidget->setRowCount(rows);
	ui.tableWidget->setColumnCount(columns);

	//���ñ�ͷ
	QStringList headers;
	headers << QStringLiteral("����")
			<< QStringLiteral("����")
			<< QStringLiteral("Ա������")
			<< QStringLiteral("Ա��ǩ��")
			<< QStringLiteral("Ա��״̬")
			<< QStringLiteral("Ա��ͷ��")
			<< QStringLiteral("����״̬");
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	//�����еȿ�
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			index = m_queryInfoModel.index(i, j);
			QString strData = m_queryInfoModel.data(index).toString();	//��ȡi��j�е�����

			//��ȡ�ֶ�����
			QSqlRecord record = m_queryInfoModel.record(i);		//��ǰ�еļ�¼
			QString strRecordName = record.fieldName(j);		//��ȡ��ǰ��

			if (strRecordName == QLatin1String("departmentID"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_depNameMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("status"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_statusMap.value(strData)));
				continue;
			}
			else if (strRecordName == QLatin1String("online"))
			{
				ui.tableWidget->setItem(i, j, new QTableWidgetItem(m_onlineMap.value(strData)));
				continue;
			}

			ui.tableWidget->setItem(i, j, new QTableWidgetItem(strData));
		}
	}

}

int QtQQ_Server::getCompDepID()
{
	QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'").arg(u8"��˾Ⱥ"));
	queryCompDepID.exec();
	queryCompDepID.first();
	return queryCompDepID.value(0).toInt();	
}

void QtQQ_Server::setDepNameMap()
{
	m_depNameMap.insert(QStringLiteral("2001"), QStringLiteral("���²�"));
	m_depNameMap.insert(QStringLiteral("2002"), QStringLiteral("�з���"));
	m_depNameMap.insert(QStringLiteral("2003"), QStringLiteral("�г���"));
}

void QtQQ_Server::setStatusMap()
{
	m_statusMap.insert(QStringLiteral("1"), QStringLiteral("��Ч"));
	m_statusMap.insert(QStringLiteral("0"), QStringLiteral("��ע��"));
}

void QtQQ_Server::setOnlineMap()
{
	m_onlineMap.insert(QStringLiteral("1"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("2"), QStringLiteral("����"));
	m_onlineMap.insert(QStringLiteral("3"), QStringLiteral("����"));
}
