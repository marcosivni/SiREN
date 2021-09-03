#include "UpdateExecutor.h"

std::vector<std::string> UpdateExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    std::string aux;

    getParser()->parse(sqlStatement);
    while (!endOfStatement()){
        if (aux.size() > 0)
            aux += " ";
        aux += currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
    }

    answer.push_back(aux);
    aux.clear();

    getParser()->resetParser();

    return answer;
}
