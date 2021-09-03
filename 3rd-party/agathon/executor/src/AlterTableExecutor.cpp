#include "AlterTableExecutor.h"

AlterTableExecutor::~AlterTableExecutor(){
    tableName.clear();
}


std::vector<std::string> AlterTableExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    getParser()->parse(sqlStatement);

    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().toUpperLexem() == "ADD"){
            getParser()->match(getParser()->getCurrentToken());
            std::string column = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            std::string type = currentToken().getLexem();
            answer.push_back("ALTER TABLE " + tableName + " ADD " + column + " " + type);
        }
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match(getParser()->getCurrentToken());
            std::string column = currentToken().getLexem();
            if (dictionary()->isComplexAttributeReference(tableName.c_str(), column.c_str())){
                answer.push_back("DROP TABLE " + dictionary()->tablePrefix(dictionary()->getComplexAttributeReference(tableName.c_str(), column.c_str())).toStdString() + "$" + tableName + "_" + column);
                answer.push_back("DELETE FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName='" + column + "'");
                answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE TableName = '" + tableName + "' AND ComplexAttribName='" + column + "'");
            }
            answer.push_back("ALTER TABLE " + tableName + " DROP COLUMN " + column);
        }
        if (currentToken().toUpperLexem() == "MODIFY"){
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match(getParser()->getCurrentToken());
            std::string column = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            std::string type = currentToken().getLexem();
            answer.push_back("ALTER TABLE " + tableName + " MODIFY COLUMN " + column);
        }


    } catch (...){
    }

    tableName.clear();
    getParser()->resetParser();

    return answer;
}

