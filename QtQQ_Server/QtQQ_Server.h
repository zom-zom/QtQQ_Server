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
    void initComboBoxData();    //初始化组合框的数据
    void initTcpSocket();       //初始化TCP监听端口
    void initUdpSocket();       //初始化UDP广播
	bool connectMySql();
    void updateTableData(int depID = 0, int employeeID = 0);
    int getCompDepID();

    void setDepNameMap();
    void setStatusMap();
    void setOnlineMap();

private slots:
    void onUDPbroadMsg(QByteArray& btData);
    void onRefresh();
    //根据群QQ号查找员工
    void on_queryDepartmentBtn_clicked();   //点击信号与槽函数自动连接
    //根据员工QQ号筛选
    void on_queryIDBtn_clicked();    
    //根据员工QQ号进行注销   设置状态为0
    void on_logoutBtn_clicked();
    //选择图片（员工的寸照）
    void on_selectPictureBtn_clicked();
    //添加员工
    void on_addBtn_clicked();
 
private:
    Ui::QtQQ_ServerClass ui;

    TcpServer* m_tcpServer;     //tcp服务端
    QUdpSocket* m_udpSender;    //udp广播


    QTimer* m_timer;        //定时刷新数据
    int m_compDepID;        //公司群QQ号
	int m_depID;            //部门QQ号
	int m_employeeID;       //员工QQ号
    QString m_pixPath;      //头像路径
    QMap<QString, QString> m_statusMap;     //状态
    QMap<QString, QString> m_depNameMap;    //部门名称
    QMap<QString, QString> m_onlineMap;     //在线
    QSqlQueryModel m_queryInfoModel;   //查询所有员工模型


};
