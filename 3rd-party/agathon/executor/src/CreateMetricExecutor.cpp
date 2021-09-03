#include "CreateMetricExecutor.h"

CreateMetricExecutor::~CreateMetricExecutor(){
}

std::vector<std::string> CreateMetricExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;

    if (sqlStatement.size() > 0){
        getParser()->parse(sqlStatement);
    } else {
        if (getParser()->countTokens() == 0)
            return answer;
    }

    std::string metricName, distanceCode, dataTypeCode, insertStatement;
    std::string metricCode = dictionary()->newMetricId().toStdString();

    try{
        getParser()->match(getParser()->getCurrentToken()); //create
        getParser()->match(getParser()->getCurrentToken()); //metric
        metricName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken()); //metric_name
        if (currentToken().toUpperLexem() == "USING"){
            getParser()->match(getParser()->getCurrentToken()); //using
            distanceCode = dictionary()->getDistanceCode(currentToken().getLexem().c_str()).toStdString();
            getParser()->match(getParser()->getCurrentToken()); //distance function
        } else {
            distanceCode = dictionary()->getDefaultDistanceCode().toStdString();
        }
        getParser()->match(getParser()->getCurrentToken()); //for
        dataTypeCode = dictionary()->getComplexDataTypeCode(currentToken().getLexem().c_str()).toStdString();
        getParser()->match(getParser()->getCurrentToken()); //complex data type


        insertStatement = "INSERT INTO CDD$MetricStruct (MetricName, DistanceCode, MetricCode, DataTypeId) VALUES ('" + metricName + "', " + distanceCode + ", "  + metricCode + ", "  + dataTypeCode + ")";
        answer.push_back(insertStatement);
    } catch (...) {
    }


    return answer;
}

