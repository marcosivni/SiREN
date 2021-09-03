#include "DropTableExecutor.h"

DropTableExecutor::~DropTableExecutor(){
}


std::vector<std::string> DropTableExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    getParser()->parse(sqlStatement);

    try{
        QStringList complexColumns = dictionary()->getComplexAttributes(getParser()->getToken(2)->getLexem().c_str());
        answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE TableName = '" + getParser()->getToken(2)->getLexem() + "'");
        for (int x = 0; x < complexColumns.size(); x++){
            QStringList metrics = dictionary()->getMetrics(getParser()->getToken(2)->getLexem().c_str(), complexColumns.at(x));

            //Removing all indexes entrances
            for (int k = 0; k < metrics.size(); k++){
                std::string indexFile = dictionary()->getIndexFile(getParser()->getToken(2)->getLexem().c_str(),
                                                                   complexColumns.at(x),
                                                                   metrics.at(k)).toStdString();
                if (!indexFile.empty()){
                    remove(("_slim_" + indexFile).c_str());
                    remove(("_dummy_" + indexFile).c_str());
                }
            }
            answer.push_back("DROP TABLE " + dictionary()->tablePrefix(dictionary()->columnDataType(getParser()->getToken(2)->getLexem().c_str(), complexColumns.at(x))).toStdString() + "$" + getParser()->getToken(2)->getLexem() + "_" + complexColumns.at(x).toStdString());
            complexColumns[x].clear();
        }
        complexColumns.clear();
        answer.push_back("DROP TABLE " + getParser()->getToken(2)->getLexem());
    } catch (...) {
    }

    getParser()->resetParser();

    return answer;
}

