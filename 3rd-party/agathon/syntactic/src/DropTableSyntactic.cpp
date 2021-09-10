#include "DropTableSyntactic.h"

DropTableSyntactic::~DropTableSyntactic(){
}

void DropTableSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem().c_str())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}


void DropTableSyntactic::drop_table_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "TABLE"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: " + currentToken().getLexem() + "'.");
                }
            } else {
                addError("TABLE is expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DROP TABLE statement is expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing DROP TABLE near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool DropTableSyntactic::isValid(std::string sqlStatement){

    getParser()->parse(sqlStatement);
    drop_table_specification();
    return (!hasErrors());
}


