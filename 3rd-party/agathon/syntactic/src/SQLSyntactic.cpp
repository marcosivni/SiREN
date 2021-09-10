#include "SQLSyntactic.h"

SQLSyntactic::SQLSyntactic(DatabaseManager *dbmg){

    this->dbmg = dbmg;
    p = new Parser();
    dic = new DataDictionary(dbmg);
}

SQLSyntactic::SQLSyntactic(std::string expression, DatabaseManager *dbmg){

    this->dbmg = dbmg;
    p = new Parser(expression);
    dic = new DataDictionary(dbmg);
}

SQLSyntactic::~SQLSyntactic(){

    if (p != nullptr){
        delete (p);
    }

    if (dic != nullptr){
        delete (dic);
    }
}

void SQLSyntactic::setDataBaseManager(DatabaseManager *dbmg){

    this->dbmg = dbmg;
    if (dic != nullptr){
        delete (dic);
    }
    dic = new DataDictionary(dbmg);
}

void SQLSyntactic::clearErrors(){

    for (unsigned long x = 0; x < errors.size(); x++){
        errors[x].clear();
    }
    errors.clear();
}

void SQLSyntactic::setSQLStatement(std::string sqlExpression){

    if (p != nullptr){
        delete (p);
    }
    p = new Parser(sqlExpression);
}

void SQLSyntactic::addError(std::string error){

    char buf[8];
    sprintf(buf, "%d", getParser()->getCurrentPosition()+1);
    std::string tmpBuf = buf;
    errors.push_back("[" + tmpBuf + "] " + error);
}

std::string SQLSyntactic::getSQLStatement(){

    std::string answer;

    if (p != nullptr)
        answer = p->getOriginal();

    return answer;
}

Parser * SQLSyntactic::getParser(){

    return p;
}

Token SQLSyntactic::currentToken(){

    return *(p->getCurrentToken());
}

bool SQLSyntactic::endOfStatement(){

    return (getParser()->getCurrentPosition() == -1);
}

bool SQLSyntactic::hasErrors(){

    return (errors.size() > 0);
}

bool SQLSyntactic::isValid(){

    return (!hasErrors());
}

std::vector<std::string> SQLSyntactic::getErrors(){

    return errors;
}

DatabaseManager* SQLSyntactic::databaseManager(){

    return dbmg;
}

std::string SQLSyntactic::getError(int pos){

    if ((pos < countErrors()) && (pos >=0 ))
        return errors[pos];
    else
        throw new std::exception();
}

size_t SQLSyntactic::countErrors(){

    return errors.size();
}

QStringList SQLSyntactic::fetchErrors(){

    QStringList answer;

    if (errors.size() > 0){
        for (size_t x = 0; x < errors.size(); x++){
            answer.append(errors[x].c_str());
        }
    }

    return answer;
}

DataDictionary* SQLSyntactic::dictionary(){

    return dic;
}

DatabaseManager* SQLSyntactic::dbManager(){

    return dbmg;
}
