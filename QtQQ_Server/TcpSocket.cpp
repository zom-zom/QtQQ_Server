#include "TcpSocket.h"

#include <QAbstractSocket>

TcpSocket::TcpSocket()
{
	m_descriptor = 0;
}

TcpSocket::~TcpSocket()
{}

void TcpSocket::run()
{
	//设置描述符
	m_descriptor = this->socketDescriptor();

	//connect(this, &QIODevice::readyRead, this, &TcpSocket::onReceiveData);
	connect(this, SIGNAL(readyRead()), this, SLOT(onReceiveData()));
	connect(this, &QAbstractSocket::disconnected, this, &TcpSocket::onClientDisconnect);

}

void TcpSocket::onReceiveData()
{
	QByteArray buffer = this->readAll();
	if(!buffer.isEmpty())
	{
		QString strData = QString::fromLocal8Bit(buffer);
		
		//发射接收到了客户端数据的信号
		emit signalGetDataFromClient(buffer, m_descriptor);
	}
}

void TcpSocket::onClientDisconnect()
{
	emit signalClientDisconnect(m_descriptor);
}

