#include "DropIndexExecutor.h"

DropIndexExecutor::~DropIndexExecutor(){
}

std::vector<std::string> DropIndexExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    getParser()->parse(sqlStatement);

    try{
        std::string tableName = getParser()->getToken(2)->getLexem();
        std::string idxName = getParser()->getToken(4)->getLexem();

        if (!dictionary()->getIndexFile(tableName.c_str(), idxName.c_str()).size()){
            answer.push_back(dictionary()->dropIndexStatement(tableName.c_str(), idxName.c_str()).toStdString());
        } else {
            answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE IndexName='" + idxName + "'");

            std::string indexFile = dictionary()->getIndexFile(tableName.c_str(), idxName.c_str()).toStdString();
            if (indexFile.size()){
                remove(("_slim_" + indexFile).c_str());
                remove(("_dummy_" + indexFile).c_str());
            }
        }
    } catch (...){
    }

    getParser()->resetParser();

    return answer;
}

