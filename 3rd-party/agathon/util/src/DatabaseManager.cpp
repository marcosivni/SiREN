#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(QString hostname, QString username, QString password, int port, DBMSType dbmsType){

    //#to-do Support for other DBMSs
    if (dbmsType == DBMSType::MYSQL){
        db = QSqlDatabase::addDatabase("QMYSQL");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    this->dbmsType = dbmsType;
    //#End to-do

    db.setHostName(hostname);
    if (dbmsType == DBMSType::MYSQL){
        db.setDatabaseName("SIREN");
        db.setConnectOptions("MYSQL_OPT_RECONNECT=TRUE");
    } else {
        db.setDatabaseName("database/SIREN.db");
    }

    db.setUserName(username);
    db.setPassword(password);
    db.setPort(port);

    if (!db.open()) {
        throw new std::invalid_argument("Driver/Database is not available!");
        return;
    }

    runnableQuery = nullptr;

}

DatabaseManager::~DatabaseManager(){

    if (runnableQuery != nullptr)
        delete (runnableQuery);

    db.commit();
    db.close();
}

bool DatabaseManager::isValid(){

    return runnableQuery->isValid();
}


bool DatabaseManager::openConnection(){

    bool answer = false;
    answer = db.open();

    if (answer){
       runnableQuery = new QSqlQuery(db);
    }

    return answer;
}

bool DatabaseManager::runDDL(QString statement){

    return runnableQuery->exec(statement);
}


bool DatabaseManager::runInsert(QString statement){

    return runnableQuery->exec(statement);
}

bool DatabaseManager::isValidSelect(QString query){

    return runnableQuery->exec(query);
}

QMap<QString, QStringList>* DatabaseManager::runSelect(QString query){

    QSqlRecord sqlRecord;
    QMap<QString, QStringList> *resultSet;
    QString attribute;
    QStringList tuple;

    runnableQuery->exec(query);
    sqlRecord = runnableQuery->record();

    resultSet = new QMap<QString, QStringList>();
    while (runnableQuery->next()){
        sqlRecord = runnableQuery->record();

        if (resultSet->empty()){
            for(int i = 0; i < sqlRecord.count(); ++i) {
                attribute = sqlRecord.fieldName(i);
                resultSet->insert(attribute, tuple);
            }
        }

        for(int i = 0; i < sqlRecord.count(); ++i) {
            attribute = runnableQuery->value(i).toString();
            resultSet->find(sqlRecord.fieldName(i)).value().append(attribute);
        }
    }

    attributesName.clear();
    for (auto iter = resultSet->constBegin(); iter != resultSet->constEnd(); ++iter) {
        attributesName.append(iter.key());
    }

    return resultSet;
}

QStringList DatabaseManager::fetchAttributesNames(){

    return attributesName;
}

DatabaseManager::DBMSType DatabaseManager::dbms(){

    return dbmsType;
}

QString DatabaseManager::error(){

    QString answer;
    answer = runnableQuery->lastError().text();
    return answer;
}

