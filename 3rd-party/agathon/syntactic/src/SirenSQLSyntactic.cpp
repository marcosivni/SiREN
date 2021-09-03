#include "SirenSQLSyntactic.h"

SirenSQLSyntactic::~SirenSQLSyntactic(){

    delete (select);
    delete (update);
    delete (deleteFrom);
    delete (insertInto);
    delete (createTable);
    delete (createIndex);
    delete (createMetric);
    delete (alter);
    delete (dropIndex);
    delete (dropMetric);
    delete (dropTable);
}


bool SirenSQLSyntactic::isValid(std::string sqlStatement){

    clearErrors();
    std::vector<std::string> ws;

    getParser()->parse(sqlStatement);

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INDEX"){
                if (createIndex->isValid(sqlStatement)){
                    return true;
                } else {
                    ws = createIndex->getErrors();
                    for (size_t x = 0; x < ws.size(); x++){
                        addError(ws[x]);
                    }
                }
            } else {
                if (currentToken().toUpperLexem() == "TABLE"){
                    if (createTable->isValid(sqlStatement)){
                        return true;
                    } else {
                        ws = createTable->getErrors();
                        for (size_t x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "METRIC"){
                        if (createMetric->isValid(sqlStatement)){
                            return true;
                        } else {
                            ws = createMetric->getErrors();
                            for (size_t x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        addError("Invalid CREATE command: " + sqlStatement);
                    }
                }
            }
        } else{
            if (currentToken().toUpperLexem() == "DROP") {
                getParser()->match(getParser()->getCurrentToken());
                if (currentToken().toUpperLexem() == "INDEX"){
                    if (dropIndex->isValid(sqlStatement)){
                        return true;
                    } else {
                        ws = dropIndex->getErrors();
                        for (size_t x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "METRIC"){
                        if (dropMetric->isValid(sqlStatement)){
                            return true;
                        } else {
                            ws = dropMetric->getErrors();
                            for (size_t x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        if (currentToken().toUpperLexem() == "TABLE"){
                            if (dropTable->isValid(sqlStatement)){
                                return true;
                            } else {
                                ws = dropTable->getErrors();
                                for (size_t x = 0; x < ws.size(); x++){
                                    addError(ws[x]);
                                }
                            }
                        } else {
                            addError("Invalid DROP command: " + sqlStatement);
                        }
                    }
                }
            } else {
                if (currentToken().toUpperLexem() == "ALTER"){
                    if (alter->isValid(sqlStatement)){
                        return true;
                    } else {
                        ws = alter->getErrors();
                        for (size_t x = 0; x < ws.size(); x++){
                            addError(ws[x]);
                        }
                    }
                } else {
                    if (currentToken().toUpperLexem() == "INSERT"){
                        if (insertInto->isValid(sqlStatement)){
                            return true;
                        } else {
                            ws = insertInto->getErrors();
                            for (size_t x = 0; x < ws.size(); x++){
                                addError(ws[x]);
                            }
                        }
                    } else {
                        if (currentToken().toUpperLexem() == "DELETE"){
                            if (deleteFrom->isValid(sqlStatement)){
                                return true;
                            } else {
                                ws = deleteFrom->getErrors();
                                for (size_t x = 0; x < ws.size(); x++){
                                    addError(ws[x]);
                                }
                            }
                        } else {
                            if (currentToken().toUpperLexem() == "UPDATE"){
                                if (update->isValid(sqlStatement)){
                                    return true;
                                } else {
                                    ws = update->getErrors();
                                    for (size_t x = 0; x < ws.size(); x++){
                                        addError(update->getError(x));
                                    }
                                }
                            } else {
                                if (currentToken().toUpperLexem() == "SELECT"){
                                    if (select->isValid(sqlStatement)){
                                        return true;
                                    } else {
                                        ws = select->getErrors();
                                        for (size_t x = 0; x < ws.size(); x++){
                                            addError(ws[x]);
                                        }
                                    }
                                } else {
                                    addError("Invalid command: " + sqlStatement);
                                }
                            }
                        }
                    }
                }
            }
        }

    } catch (...) {
        addError("'" + getParser()->getOriginal() + "' is an invalid SQL expression.");
    }

    for (size_t x = 0; x < ws.size(); x++){
        ws[x].clear();
    }
    ws.clear();
    return (!hasErrors());
}

bool SirenSQLSyntactic::isSelect(){

    return (currentToken().toUpperLexem() == "SELECT");
}

bool SirenSQLSyntactic::isDML(){

    return ( (currentToken().toUpperLexem() == "INSERT") ||
             (currentToken().toUpperLexem() == "DELETE") ||
             (currentToken().toUpperLexem() == "UPDATE") ||
             (currentToken().toUpperLexem() == "SELECT") );

}

bool SirenSQLSyntactic::isDDL(){


    return ((currentToken().toUpperLexem() == "CREATE") ||
            (currentToken().toUpperLexem() == "DROP") ||
            (currentToken().toUpperLexem() == "ALTER") );
}
