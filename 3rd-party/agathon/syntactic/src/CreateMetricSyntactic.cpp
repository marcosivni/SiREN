#include "CreateMetricSyntactic.h"

CreateMetricSyntactic::~CreateMetricSyntactic(){
}

void CreateMetricSyntactic::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isMetric(currentToken().getLexem().c_str())){
            addError("Metric '" + currentToken().getLexem() + "' already exists!");
        } else {
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Metric name expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateMetricSyntactic::distance_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isDistanceFunctionName(currentToken().getLexem().c_str())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError(currentToken().getLexem() + " is not a valid identifier for a distance funtion.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateMetricSyntactic::datatype_name(){

    std::string types;

    QStringList dataTypes = dictionary()->allValidComplexDataTypes();
    for (int x = 0; x < dataTypes.size(); x++){
        if (x > 0){
            types += " | ";
        }
        types += dataTypes.at(x).toStdString();
    }

    if (dataTypes.contains(currentToken().toUpperLexem().c_str())) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'" + currentToken().getLexem() + "' is not a valid data type.");
        addError("Valid types are ( " + types + " )");
        types.clear();
    }
}

void CreateMetricSyntactic::association(){

    if (currentToken().getTokenType() == Token::TK_INTEGER){
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Dimensionality expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateMetricSyntactic::create_metric_specification(){

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "METRIC"){
                getParser()->match(getParser()->getCurrentToken());
                metric_name();
                if (currentToken().toUpperLexem() == "USING"){
                    getParser()->match(getParser()->getCurrentToken());
                    distance_name();
                }
                if (currentToken().toUpperLexem() == "FOR"){
                    getParser()->match(getParser()->getCurrentToken());
                    datatype_name();
                } else {
                    addError("Missing FOR clause for CREATE METRIC statement.");
                }
                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("Missing METRIC after CREATE clause.");
            }
        } else {
            addError("Missing CREATE clause.");
        }
    } catch (...) {
        addError("Syntax Error on CREATE METRIC statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool CreateMetricSyntactic::isValid(std::string sqlStatement){

    getParser()->parse(sqlStatement);
    create_metric_specification();
    return (!hasErrors());
}
