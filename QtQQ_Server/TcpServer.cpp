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
		qDebug() << QString::fromLocal8Bit("服务端监听端口: %1 成功").arg(m_port);
		
	}
	return false;
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << QString::fromLocal8Bit("新的连接：") << socketDescriptor << Qt::endl;

	TcpSocket* tcpSocket = new TcpSocket();
	tcpSocket->setSocketDescriptor(socketDescriptor);
	tcpSocket->run();

	//收到客户端数据后，Server进行处理
	connect(tcpSocket, &TcpSocket::signalGetDataFromClient, this, &TcpServer::socketDataProcessing);

	//收到客户端断开连接后，Server进行处理
	connect(tcpSocket, &TcpSocket::signalClientDisconnect, this, &TcpServer::socketDisConeected);

	//将socket添加到列表中
	m_tcpSocketConnectList.append(tcpSocket);



}

void TcpServer::socketDataProcessing(QByteArray& sendData, int descriptor)
{
	for (int i = 0; i < m_tcpSocketConnectList.count(); ++i)
	{
		QTcpSocket* item = m_tcpSocketConnectList.at(i);
		if (item->socketDescriptor() == descriptor)
		{
			qDebug() << u8"来自IP：" << item->peerAddress().toString()
				<< u8"发来的数据：" << QString(sendData);
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

		//查找断开连接的socket
		
		if (itemDescriptor == descriptor || itemDescriptor == -1)
		{
			m_tcpSocketConnectList.removeAt(i);
			item->deleteLater();
			qDebug() << u8"TcpSocket断开连接：" << descriptor << Qt::endl;
			return;
		}
	}
}


