#pragma once

#include <QTcpServer>

class TcpServer  : public QTcpServer
{
	Q_OBJECT

public:
	TcpServer(int port);
	~TcpServer();

public:
	bool run();	//监听

protected:
	
	//客户端有新的连接时 老版本参数类型用int
	void incomingConnection(qintptr socketDescriptor) override;

signals:
	void signalTcpMsgComes(QByteArray&);

private slots:
	//处理数据
	void socketDataProcessing(QByteArray& sendData, int descriptor);

	//断开连接
	void socketDisConeected(int descriptor);
 
private:
	int m_port;	//端口号
	QList<QTcpSocket*> m_tcpSocketConnectList;
};
