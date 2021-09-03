#include "DeleteSyntactic.h"

DeleteSyntactic::~DeleteSyntactic(){

    destroyAuxiliaryStructures();
}

void DeleteSyntactic::destroyAuxiliaryStructures(){

    tableName.clear();
}

void DeleteSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem().c_str())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DeleteSyntactic::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
        column_reference_list();
    } else {
        addError("WHERE clause expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DeleteSyntactic::column_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidColumn(getTableName().c_str(), currentToken().getLexem().c_str())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Column '" + currentToken().getLexem() + "' does not exists in the table '" + getTableName() + "'.");
        }
    } else {
        addError("Identifier expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DeleteSyntactic::value_name(){

    if ((currentToken().getTokenType() == Token::TK_STRING) ||
            (currentToken().getTokenType() == Token::TK_INTEGER) ||
            (currentToken().getTokenType() == Token::TK_FLOATING_POINT)){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("STRING | INTEGER | FLOAT expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void DeleteSyntactic::column_reference(){

    column_name();

    if ((currentToken().getTokenType() == Token::TK_EQUAL) ||
            (currentToken().getTokenType() == Token::TK_NE) ||
            (currentToken().toUpperLexem() == "LIKE") ||
            (currentToken().getTokenType() == Token::TK_N2) ||
            (currentToken().getTokenType() == Token::TK_LQ) ||
            (currentToken().getTokenType() == Token::TK_LE) ||
            (currentToken().getTokenType() == Token::TK_GQ) ||
            (currentToken().getTokenType() == Token::TK_GE)){
        getParser()->match(getParser()->getCurrentToken());
        value_name();
    } else {
        if (currentToken().toUpperLexem() == "NOT"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "IN"){
                getParser()->match(getParser()->getCurrentToken());
                getParser()->match("(", Token::TK_OPEN_BRACE);
                value_name();
                while (currentToken().getTokenType() == Token::TK_COMMA){
                    getParser()->match(getParser()->getCurrentToken());
                    value_name();
                }
                getParser()->match(")", Token::TK_CLOSE_BRACE);
            } else {
                addError("IN expected. '" + currentToken().getLexem() + "' found instead.");
            }
        }
        if (currentToken().toUpperLexem() == "IN"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match("(", Token::TK_OPEN_BRACE);
            value_name();
            while (currentToken().getTokenType() == Token::TK_COMMA){
                getParser()->match(getParser()->getCurrentToken());
                value_name();
            }
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {
            if (currentToken().toUpperLexem() == "BETWEEN"){
                value_name();
                if (currentToken().toUpperLexem() == "AND"){
                    getParser()->match(getParser()->getCurrentToken());
                    value_name();
                } else {
                    addError("AND expected. '" + currentToken().getLexem() + "' found instead.");
                }
            } else {
                addError("Predicate '" + currentToken().getLexem() + "' is not valid for DELETE statement.");
            }
        }
    }
}

void DeleteSyntactic::column_reference_list(){

    column_reference();

    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        column_reference_list();
    }
}

void DeleteSyntactic::delete_specification(){

    try{
        if (currentToken().toUpperLexem() == "DELETE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "FROM"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if (!endOfStatement()){
                    where_clause();
                }
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("FROM expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DELETE expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...){
        addError("Syntax error when processing DELETE statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}


std::string DeleteSyntactic::getTableName(){

    return tableName;
}

bool DeleteSyntactic::isValid(std::string sqlStatement){

    destroyAuxiliaryStructures();
    getParser()->parse(sqlStatement);
    delete_specification();
    return (!hasErrors());
}


