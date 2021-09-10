#include "SQLExecutor.h"

SQLExecutor::SQLExecutor(DatabaseManager *dbmg){

    p = new Parser();
    dic = new DataDictionary(dbmg);
    this->dbmg = dbmg;
}

SQLExecutor::SQLExecutor(std::string sirenSqlStatement, DatabaseManager *dbmg){

    p = new Parser(sirenSqlStatement);
    dic = new DataDictionary(dbmg);
    this->dbmg = dbmg;
}

void SQLExecutor::destroyAuxiliaryStructures(){

    if (p != nullptr){
        delete (p);
    }
    if (dic != nullptr){
        delete (dic);
    }
}

SQLExecutor::~SQLExecutor(){

    destroyAuxiliaryStructures();
}



std::string SQLExecutor::sirenSqlStatement(){

    return p->getOriginal();
}


void SQLExecutor::setDataBaseManager(DatabaseManager *dbmg){

    this->dbmg = dbmg;
    if (dic != nullptr){
        delete (dic);
    }
    dic = new DataDictionary(dbmg);
}

Parser* SQLExecutor::getParser(){

    return p;
}

DataDictionary* SQLExecutor::dictionary(){

    return dic;
}

DatabaseManager* SQLExecutor::dbManager(){

    return dbmg;
}

Token SQLExecutor::currentToken(){

    return *(p->getCurrentToken());
}

bool SQLExecutor::endOfStatement(){

    return (getParser()->getCurrentPosition() == -1);
}
