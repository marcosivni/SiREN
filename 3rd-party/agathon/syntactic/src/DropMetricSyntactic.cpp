#include "DropMetricSyntactic.h"

DropMetricSyntactic::~DropMetricSyntactic(){
}

void DropMetricSyntactic::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isMetric(currentToken().getLexem().c_str())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Metric '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}


void DropMetricSyntactic::drop_metric_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "METRIC"){
                getParser()->match(getParser()->getCurrentToken());
                metric_name();
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: " + currentToken().getLexem() + "'.");
                }
            } else {
                addError("METRIC is expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DROP METRIC statement is expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing DROP METRIC near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool DropMetricSyntactic::isValid(std::string sqlStatement){

    getParser()->parse(sqlStatement);
    drop_metric_specification();
    return (!hasErrors());
}



