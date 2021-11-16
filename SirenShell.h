#ifndef SIRENSHELL_H
#define SIRENSHELL_H

#include <ConsoleManager.h>
#include <SirenSQLQuery.h>
#include <QString>
#include <QDateTime>

//Qt includes for network - OS binding
#include <QTcpSocket>
#include <QTcpServer>
#include <QAbstractSocket>

#include <QThread>
#include <QThreadPool>


class SirenShell: public QThread{

    Q_OBJECT

    private:
        ConsoleManager console;
        QTcpServer *tcpServer;
        QList<QTcpSocket*> socketList;
        QString dataFormat;
        DatabaseManager *db;
        QHostAddress ip;
        int port;

    private:
        void destroyTcpServer();
        void destroyDBConnection();
        QStringList fromStringVector(std::vector<std::string> input);
        QString logResultErrors(QStringList errors);
        QString logResultSet(QMap<QString, QStringList> resultSet);

    public:
        SirenShell(DatabaseManager *db, QHostAddress ip = QHostAddress::LocalHost, int port = 3333);
        ~SirenShell();

        void run();
        void startTcpServer();

    private slots:
        void onNewConnection();
        void onSocketStateChanged(QAbstractSocket::SocketState socketState);
        void onReadyRead();

};

#endif // SIRENSHELL_H
