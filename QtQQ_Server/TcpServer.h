#pragma once

#include <QTcpServer>

class TcpServer  : public QTcpServer
{
	Q_OBJECT

public:
	TcpServer(int port);
	~TcpServer();

public:
	bool run();	//����

protected:
	
	//�ͻ������µ�����ʱ �ϰ汾����������int
	void incomingConnection(qintptr socketDescriptor) override;

signals:
	void signalTcpMsgComes(QByteArray&);

private slots:
	//��������
	void socketDataProcessing(QByteArray& sendData, int descriptor);

	//�Ͽ�����
	void socketDisConeected(int descriptor);
 
private:
	int m_port;	//�˿ں�
	QList<QTcpSocket*> m_tcpSocketConnectList;
};
