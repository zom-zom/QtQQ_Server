#pragma once

#include <QtWidgets/QDialog>
#include <QSqlQueryModel>
#include <QTimer>
#include <QUdpSocket>

#include "ui_QtQQ_Server.h"
#include "TcpServer.h"

class QtQQ_Server : public QDialog
{
    Q_OBJECT

public:
    QtQQ_Server(QWidget *parent = nullptr);
    ~QtQQ_Server();

private:
    void initComboBoxData();    //��ʼ����Ͽ������
    void initTcpSocket();       //��ʼ��TCP�����˿�
    void initUdpSocket();       //��ʼ��UDP�㲥
	bool connectMySql();
    void updateTableData(int depID = 0, int employeeID = 0);
    int getCompDepID();

    void setDepNameMap();
    void setStatusMap();
    void setOnlineMap();

private slots:
    void onUDPbroadMsg(QByteArray& btData);
    void onRefresh();
    //����ȺQQ�Ų���Ա��
    void on_queryDepartmentBtn_clicked();   //����ź���ۺ����Զ�����
    //����Ա��QQ��ɸѡ
    void on_queryIDBtn_clicked();    
    //����Ա��QQ�Ž���ע��   ����״̬Ϊ0
    void on_logoutBtn_clicked();
    //ѡ��ͼƬ��Ա���Ĵ��գ�
    void on_selectPictureBtn_clicked();
    //���Ա��
    void on_addBtn_clicked();
 
private:
    Ui::QtQQ_ServerClass ui;

    TcpServer* m_tcpServer;     //tcp�����
    QUdpSocket* m_udpSender;    //udp�㲥


    QTimer* m_timer;        //��ʱˢ������
    int m_compDepID;        //��˾ȺQQ��
	int m_depID;            //����QQ��
	int m_employeeID;       //Ա��QQ��
    QString m_pixPath;      //ͷ��·��
    QMap<QString, QString> m_statusMap;     //״̬
    QMap<QString, QString> m_depNameMap;    //��������
    QMap<QString, QString> m_onlineMap;     //����
    QSqlQueryModel m_queryInfoModel;   //��ѯ����Ա��ģ��


};
