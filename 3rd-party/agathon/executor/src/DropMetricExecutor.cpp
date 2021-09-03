#include "DropMetricExecutor.h"

DropMetricExecutor::~DropMetricExecutor(){
}

std::vector<std::string> DropMetricExecutor::translate(std::string sqlStatement){

    getParser()->parse(sqlStatement);

    std::vector<std::string> answer;
    std::string metricName = getParser()->getToken(2)->getLexem();

    answer.push_back("DELETE FROM CDD$MetricStruct WHERE MetricCode = " + dictionary()->getMetricCode(metricName.c_str()).toStdString());

    QPair<QStringList, QStringList> tablesAndColumns = dictionary()->allTablesAndComplexColumnNames(dictionary()->getMetricCode(metricName.c_str()));

    for (int x = 0; x < tablesAndColumns.first.size(); x++){

        //Remove complex index file for the metric
        std::string indexFile = dictionary()->getIndexFile(tablesAndColumns.first.at(x), tablesAndColumns.second.at(x), dictionary()->getMetricCode(metricName.c_str())).toStdString();
        if (!(indexFile.empty())){
            remove(("_slim_" + indexFile).c_str());
            remove(("_dummy_" + indexFile).c_str());
        }

        if (dictionary()->getMetrics(tablesAndColumns.first.at(x), tablesAndColumns.second.at(x)).size()){
            answer.push_back("DELETE FROM CDD$ComplexAttribute WHERE TableName = '" + tablesAndColumns.first.at(x).toStdString() + "' AND ComplexAttribName='" + tablesAndColumns.second.at(x).toStdString() + "'");
            answer.push_back("DROP TABLE " + dictionary()->tablePrefix(dictionary()->getComplexAttributeReference(tablesAndColumns.first.at(x), tablesAndColumns.first.at(x))).toStdString() + "$" + tablesAndColumns.first.at(x).toStdString() + "_" + tablesAndColumns.second.at(x).toStdString());
            answer.push_back("ALTER TABLE " + tablesAndColumns.first.at(x).toStdString() + " DROP COLUMN " + tablesAndColumns.second.at(x).toStdString());
        }
    }
    metricName.clear();

    return answer;
}


