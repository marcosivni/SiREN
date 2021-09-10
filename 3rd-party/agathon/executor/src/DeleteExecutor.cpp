#include "DeleteExecutor.h"

DeleteExecutor::~DeleteExecutor(){
}

//@todo Should remove from index also!!
std::vector<std::string> DeleteExecutor::translate(std::string sqlStatement){

    std::vector<std::string> answer;
    std::string aux;
    getParser()->parse(sqlStatement);

    try{

        getParser()->match(getParser()->getCurrentToken());//DELETE
        getParser()->match(getParser()->getCurrentToken());//FROM
        std::string tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());//tb name

        QStringList cps = dictionary()->getComplexAttributes(tableName.c_str());

        std::string whereClause;

        if (!endOfStatement()){
           getParser()->match(getParser()->getCurrentToken());
           while (!endOfStatement()){
               whereClause += " " + currentToken().getLexem();
               getParser()->match(getParser()->getCurrentToken());
           }
        }

        for (int x = 0; x < cps.size(); x++){

            QStringList ids = dictionary()->getComplexAttributeReferenceId(tableName.c_str(), cps.at(x), whereClause.c_str());
            QStringList metricCodes = dictionary()->getMetrics(tableName.c_str(), cps.at(x));

            if (ids.size() > 0){
                aux = "DELETE FROM ";
                aux += dictionary()->tablePrefix(dictionary()->getComplexAttributeReference(tableName.c_str(), cps.at(x))).toStdString();
                aux += "$" + tableName + "_" + cps.at(x).toStdString();

                aux += " WHERE " + cps.at(x).toStdString() + "_id IN ( ";
                for (int y = 0; y < ids.size(); y++){
                    if (y > 0)
                        aux += ", ";
                    aux += ids.at(y).toStdString() + " ";
                }

                aux += ")";
                answer.push_back(aux);

                //Removing from indexes
                for (int k = 0; k < metricCodes.size(); k++){
                    WrapperDF eval(dictionary()->getDistanceFunction(metricCodes.at(x)).toInt());
                    IndexManager *idx = new IndexManager(
                                dictionary()->getIndexFile(tableName.c_str(), cps.at(x), metricCodes.at(k)).toStdString(),
                                eval,
                                false,
                                IndexManager::PAGE_SIZE,
                                true);

                    for (int y = 0; y < ids.size(); y++){
                        //@to-do
                        //idx->removeItem(atoi(ids.at(y).toStdString().c_str()));
                    }

                    delete (idx);
                }
                //
            }
        }

        aux = "DELETE FROM " + tableName;
        if (!whereClause.empty()){
            aux += " WHERE " + whereClause;
        }
        answer.push_back(aux);
        aux.clear();
    } catch (...){
    }

    getParser()->resetParser();

    return answer;
}

