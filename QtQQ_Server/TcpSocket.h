#pragma once

#include <QTcpSocket>

class TcpSocket  : public QTcpSocket
{
	Q_OBJECT

public:
	TcpSocket();
	~TcpSocket();

	void run();
private slots:
	void onReceiveData();	//����readyRead�źŶ�ȡ������
	void onClientDisconnect();	//�ͻ��˶Ͽ�����

signals:
	void signalGetDataFromClient(QByteArray&, int);		//�ӿͻ����յ����ݺ����ź�������Ҫ����
	void signalClientDisconnect(int);	//����Server�пͻ��˶Ͽ�����

private:
	int m_descriptor;	//����������Ψһ��ʶ

};
