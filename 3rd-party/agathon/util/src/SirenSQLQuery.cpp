#include "SirenSQLQuery.h"

void SirenSQLQuery::clear(){

    qErrors.clear();
    qAnswer.clear();
    prepared = false;
}


void SirenSQLQuery::init(DatabaseManager *dbmg){

    this->dbmg = dbmg;
    createMetricS.setDataBaseManager(dbmg);
    createMetricE.setDataBaseManager(dbmg);
    createIndexS.setDataBaseManager(dbmg);
    createIndexE.setDataBaseManager(dbmg);
    createTableS.setDataBaseManager(dbmg);
    createTableE.setDataBaseManager(dbmg);
    dropMetricS.setDataBaseManager(dbmg);
    dropMetricE.setDataBaseManager(dbmg);
    dropIndexS.setDataBaseManager(dbmg);
    dropIndexE.setDataBaseManager(dbmg);
    dropTableS.setDataBaseManager(dbmg);
    dropTableE.setDataBaseManager(dbmg);
    alterTableS.setDataBaseManager(dbmg);
    alterTableE.setDataBaseManager(dbmg);
    insertS.setDataBaseManager(dbmg);
    insertE.setDataBaseManager(dbmg);
    deleteS.setDataBaseManager(dbmg);
    deleteE.setDataBaseManager(dbmg);
    updateS.setDataBaseManager(dbmg);
    updateE.setDataBaseManager(dbmg);
    selectS.setDataBaseManager(dbmg);
    selectE.setDataBaseManager(dbmg);
    descS.setDataBaseManager(dbmg);
    descE.setDataBaseManager(dbmg);

    prepared = false;
}


bool SirenSQLQuery::isValid(std::string sqlStatement){

    return prepare(sqlStatement, true);
}

bool SirenSQLQuery::prepare(std::string sqlStatement, bool syntacticOnly){

    bool answer = false;
    Parser *p = new Parser(sqlStatement);
    SIRENQueryType qType;

    clear();

    if (p->getCurrentToken()->toUpperLexem() == "CREATE"){
        p->match(p->getCurrentToken());
        if (p->getCurrentToken()->toUpperLexem() == "METRIC"){
            qType = SIRENQueryType::CREATE_METRIC;
        } else {
            if (p->getCurrentToken()->toUpperLexem() == "INDEX"){
                qType = SIRENQueryType::CREATE_INDEX;
            } else {
                qType = SIRENQueryType::CREATE_TABLE;
            }
        }
    } else {
        if (p->getCurrentToken()->toUpperLexem() == "DROP"){
            p->match(p->getCurrentToken());
            if (p->getCurrentToken()->toUpperLexem() == "METRIC"){
                qType = SIRENQueryType::DROP_METRIC;
            } else {
                if (p->getCurrentToken()->toUpperLexem() == "INDEX"){
                    qType = SIRENQueryType::DROP_INDEX;
                } else {
                    qType = SIRENQueryType::DROP_TABLE;
                }
            }
        } else {
            if (p->getCurrentToken()->toUpperLexem() == "ALTER"){
                qType = SIRENQueryType::ALTER_TABLE;
            } else {
                if (p->getCurrentToken()->toUpperLexem() == "INSERT"){
                    qType = SIRENQueryType::INSERT;
                } else {
                    if (p->getCurrentToken()->toUpperLexem() == "DELETE"){
                        qType = SIRENQueryType::DELETE;
                    } else {
                        if (p->getCurrentToken()->toUpperLexem() == "UPDATE"){
                            qType = SIRENQueryType::UPDATE;
                        } else {
                            if (p->getCurrentToken()->toUpperLexem() == "SELECT"){
                                qType = SIRENQueryType::SELECT;
                            } else {
                                qType = SIRENQueryType::DESC;
                            }
                        }
                    }
                }
            }
        }
    }

    switch (qType){
    case CREATE_METRIC:
        answer = createMetricS.isValid(sqlStatement);
        qErrors = createMetricS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = createMetricE.translate(sqlStatement);
        }
        break;

    case CREATE_INDEX:
        answer = createIndexS.isValid(sqlStatement);
        qErrors = createIndexS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = createIndexE.translate(sqlStatement);
        }
        break;

    case CREATE_TABLE:
        answer = createTableS.isValid(sqlStatement);
        qErrors = createTableS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = createTableE.translate(sqlStatement);
        }
        break;

    case DROP_METRIC:
        answer = dropMetricS.isValid(sqlStatement);
        qErrors = dropMetricS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = dropMetricE.translate(sqlStatement);
        }
        break;

    case DROP_INDEX:
        answer = dropIndexS.isValid(sqlStatement);
        qErrors = dropIndexS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = dropIndexE.translate(sqlStatement);
        }
        break;

    case DROP_TABLE:
        answer = dropTableS.isValid(sqlStatement);
        qErrors = dropTableS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = dropTableE.translate(sqlStatement);
        }
        break;

    case ALTER_TABLE:
        answer = alterTableS.isValid(sqlStatement);
        qErrors = alterTableS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = alterTableE.translate(sqlStatement);
        }
        break;

    case INSERT:
        answer = insertS.isValid(sqlStatement);
        qErrors = insertS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = insertE.translate(sqlStatement);
        }
        break;

    case DELETE:
        answer = deleteS.isValid(sqlStatement);
        qErrors = deleteS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = deleteE.translate(sqlStatement);
        }
        break;

    case UPDATE:
        answer = updateS.isValid(sqlStatement);
        qErrors = updateS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = updateE.translate(sqlStatement);
        }
        break;

    case SELECT:
        if (QString(sqlStatement.c_str()).contains(" NEAR ", Qt::CaseInsensitive)
                || QString(sqlStatement.c_str()).contains(" FAR ", Qt::CaseInsensitive)){
            answer = selectS.isValid(sqlStatement);
            qErrors = selectS.getErrors();
            if (answer && !syntacticOnly){
                qAnswer = selectE.translate(sqlStatement);
            }
        } else {
            answer = true;
            qAnswer.clear();
            qAnswer.push_back(sqlStatement);
        }
        break;

    default :
        answer = descS.isValid(sqlStatement);
        qErrors = descS.getErrors();
        if (answer && !syntacticOnly){
            qAnswer = descE.translate(sqlStatement);
        }
    }

    if (p != nullptr){
        delete (p);
    }
    prepared = answer;

    return answer;
}

bool SirenSQLQuery::runQuery(){

    bool answer = true;

    for (size_t x = 0; x < qAnswer.size() && prepared; x++){
        answer += dbmg->runInsert(qAnswer[x].c_str());
    }
    if (!answer){
        qErrors.push_back(dbmg->error().toStdString());
    }

    return answer && prepared;
}

bool SirenSQLQuery::isPrepared(){

    return prepared;
}

bool SirenSQLQuery::hasErrors(){

    return qErrors.size() > 0;
}

std::vector<std::string> SirenSQLQuery::errors(){

    return qErrors;
}

std::vector<std::string> SirenSQLQuery::translation(){

    return qAnswer;
}
