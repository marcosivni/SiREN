#include "UpdateSyntactic.h"

void UpdateSyntactic::column_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidColumn(getTableName().c_str(), currentToken().getLexem().c_str())){
            if (dictionary()->isComplexAttributeReference(getTableName().c_str(), currentToken().getLexem().c_str())){
                addError("Updating complex data-type is not allowed in the current implementation.");
            } else {
                getParser()->match(getParser()->getCurrentToken());
            }
        } else {
            addError("Column '" + currentToken().getLexem() + "' does not exists in table '" + getTableName() + "'.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void UpdateSyntactic::value_name(){

    if ((currentToken().getTokenType() == Token::TK_STRING) ||
            (currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
            (currentToken().getTokenType() == Token::TK_INTEGER)){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Invalid value for column '" + currentToken().getLexem() + "' found instead.");
    }
}

void UpdateSyntactic::update_reference(){

    column_name();
    getParser()->match("=", Token::TK_EQUAL);
    value_name();
}

void UpdateSyntactic::update_list(){

    update_reference();
    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(getParser()->getCurrentToken());
        update_list();
    }
}

void UpdateSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem().c_str())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void UpdateSyntactic::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
        column_where_list();
    } else {
        addError("WHERE clause expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void UpdateSyntactic::column_where_list(){

    column_where();
    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        column_where_list();
    }
}

void UpdateSyntactic::column_where(){

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
                addError("Predicate '" + currentToken().getLexem() + "' is not valid for UPDATE statement.");
            }
        }
    }
}

void UpdateSyntactic::update_specification(){

    try{
        if (currentToken().toUpperLexem() == "UPDATE"){
            getParser()->match(getParser()->getCurrentToken());
            table_name();
            if (currentToken().toUpperLexem() == "SET"){
                getParser()->match(getParser()->getCurrentToken());
                update_list();
                if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WHERE")){
                    where_clause();
                } else {
                    addError("WHERE expected. '" + currentToken().getLexem() + "' found instead.");
                }
            } else {
                addError("SET expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("UPDATE expected. '" + currentToken().getLexem() + "' found instead.");
        }

        if (!endOfStatement()){
            addError("Extra tokens at the end of statement: '" + currentToken().getLexem() + "'.");
        }
    } catch (...){
        addError("Syntax error in UPDATE statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

UpdateSyntactic::~UpdateSyntactic(){

    destroyAuxiliaryStructures();
}

void UpdateSyntactic::destroyAuxiliaryStructures(){

    tableName.clear();
}

std::string UpdateSyntactic::getTableName(){

    return tableName;
}

bool UpdateSyntactic::isValid(std::string sqlStatement){

    destroyAuxiliaryStructures();
    getParser()->parse(sqlStatement);
    update_specification();
    return (!hasErrors());
}





