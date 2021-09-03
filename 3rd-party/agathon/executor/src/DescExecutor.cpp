#include "DescExecutor.h"

DescExecutor::~DescExecutor(){
}

std::vector<std::string> DescExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    getParser()->parse(sqlStatement);

    try{
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "METRIC"){
            getParser()->match(getParser()->getCurrentToken());
            answer.push_back(dictionary()->generateDescMetricCommand(currentToken().getLexem().c_str()).toStdString());
        } else {
            answer.push_back(dictionary()->generateDescTableCommand(currentToken().getLexem().c_str()).toStdString());
        }
    } catch (...) {
    }

    getParser()->resetParser();

    return answer;
}


