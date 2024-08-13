#include "TcpServer.h"
#include <QTcpSocket>
#include "TcpSocket.h"



TcpServer::TcpServer(int port)
	:m_port(port)
{

}

TcpServer::~TcpServer()
{}

bool TcpServer::run()
{
	if (this->listen(QHostAddress::AnyIPv4, m_port))
	{
		qDebug() << QString::fromLocal8Bit("����˼����˿�: %1 �ɹ�").arg(m_port);
		
	}
	return false;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << QString::fromLocal8Bit("�µ����ӣ�") << socketDescriptor << Qt::endl;

	TcpSocket* tcpSocket = new TcpSocket();
	tcpSocket->setSocketDescriptor(socketDescriptor);
	tcpSocket->run();

	//�յ��ͻ������ݺ�Server���д���
	connect(tcpSocket, &TcpSocket::signalGetDataFromClient, this, &TcpServer::socketDataProcessing);

	//�յ��ͻ��˶Ͽ����Ӻ�Server���д���
	connect(tcpSocket, &TcpSocket::signalClientDisconnect, this, &TcpServer::socketDisConeected);

	//��socket��ӵ��б���
	m_tcpSocketConnectList.append(tcpSocket);



}

void TcpServer::socketDataProcessing(QByteArray& sendData, int descriptor)
{
	for (int i = 0; i < m_tcpSocketConnectList.count(); ++i)
	{
		QTcpSocket* item = m_tcpSocketConnectList.at(i);
		if (item->socketDescriptor() == descriptor)
		{
			qDebug() << u8"����IP��" << item->peerAddress().toString()
				<< u8"���������ݣ�" << QString(sendData);
			emit signalTcpMsgComes(sendData);
		}
	}
}

void TcpServer::socketDisConeected(int descriptor)
{
	for (int i = 0; i < m_tcpSocketConnectList.count(); ++i)
	{
		QTcpSocket* item = m_tcpSocketConnectList.at(i);
		int itemDescriptor = item->socketDescriptor();

		//���ҶϿ����ӵ�socket
		
		if (itemDescriptor == descriptor || itemDescriptor == -1)
		{
			m_tcpSocketConnectList.removeAt(i);
			item->deleteLater();
			qDebug() << u8"TcpSocket�Ͽ����ӣ�" << descriptor << Qt::endl;
			return;
		}
	}
}


