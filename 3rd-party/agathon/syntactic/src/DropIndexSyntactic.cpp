#include "DropIndexSyntactic.h"

DropIndexSyntactic::~DropIndexSyntactic(){

    destroyAuxiliaryStructures();
}

void DropIndexSyntactic::destroyAuxiliaryStructures(){

    tableName.clear();
}

std::string DropIndexSyntactic::getTableName(){

    return tableName;
}

void DropIndexSyntactic::index_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if ((!(dictionary()->isComplexAttributeReference(getTableName().c_str(), currentToken().getLexem().c_str()))) &&
            ((dictionary()->isComplexAttributeIndex(getTableName().c_str(), currentToken().getLexem().c_str()) ||
            (dictionary()->isValidIndex(getTableName().c_str(), currentToken().getLexem().c_str()))))){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Index '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DropIndexSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem().c_str())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists!");
        }
    } else {
        addError("Table identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DropIndexSyntactic::drop_index_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INDEX"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if ((!endOfStatement()) && (currentToken().getLexem() == ".")){
                    getParser()->match(getParser()->getCurrentToken());
                    index_name();
                } else {
                    addError("Syntax error. You should provide DROP INDEX 'table_name'.'index_name'.");
                }

                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: " + currentToken().getLexem());
                }

            } else {
                addError("INDEX clause missing for DROP INDEX statement.");
            }
        } else {
            addError("DROP clause missing for DROP INDEX statement.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE INDEX statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool DropIndexSyntactic::isValid(std::string sqlStatement){

    destroyAuxiliaryStructures();
    getParser()->parse(sqlStatement);
    drop_index_specification();
    return (!hasErrors());
}



