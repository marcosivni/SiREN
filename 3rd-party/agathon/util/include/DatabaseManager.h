#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

//Qt Database includes
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMap>
#include <QVariant>

#include <iostream>


class DatabaseManager{

    public:
        enum DBMSType {SQLITE, MYSQL};

    private:
        QSqlDatabase db;
        QStringList attributesName;
        DBMSType dbmsType;
        QSqlQuery *runnableQuery;
        bool valid;

    private:
        void addAttributeName(QString attributeName);
        QMap<QString, QStringList>* resultSetTransformation();

    public:
        DatabaseManager(QString hostname, QString username, QString password, int port = 3306, DBMSType dbmsType = DatabaseManager::SQLITE);
        ~DatabaseManager();

        bool openConnection();

        bool runDDL(QString query);
        bool runInsert(QString query);
        bool isValidSelect(QString query);
        bool isValid();

        QMap<QString, QStringList>* runSelect(QString query);
        QStringList fetchAttributesNames();
        DBMSType dbms();
        QString error();
};

#endif // DATABASEMANAGER_H
