#include "CreateIndexExecutor.h"

CreateIndexExecutor::~CreateIndexExecutor(){

    for (size_t x = 0; x < columnName.size(); x++){
        columnName[x].clear();
    }
    columnName.clear();
}

std::string CreateIndexExecutor::getColumnName(size_t pos) throw (std::exception*){

    if (pos < columnName.size())
        return columnName[pos];
    else
        throw (new std::exception());
}

void CreateIndexExecutor::column_name(){

    bool first = true;
    while ((currentToken().getLexem() != ")") || (first)){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        }
        columnName.push_back(currentToken().getLexem());
        first = false;
        getParser()->match(getParser()->getCurrentToken());
    }
}

std::vector<std::string> CreateIndexExecutor::translate(std::string sqlStatement){

    std::string trans;
    std::vector<std::string> answer;

    getParser()->parse(sqlStatement);

    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        std::string indexName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());

        column_name();

        getParser()->match(getParser()->getCurrentToken());

        trans = "CREATE INDEX " + getParser()->getToken(2)->getLexem() + " ON ";
        trans += getParser()->getToken(4)->getLexem() + " (";
        for (size_t x = 0; x < columnName.size(); x++){
            if (x > 0)
                trans += ",";
            trans += " " + getColumnName(x) + " ";
        }
        trans += ")";

        if (!endOfStatement()){
            trans += " USING ";
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "BTREE"){
                trans += "BTREE";
                getParser()->match(getParser()->getCurrentToken());
            } else {
                if (currentToken().toUpperLexem() == "HASH"){
                    trans += "HASH";
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    trans.clear();
                    trans = "INSERT INTO CDD$ComplexAttribMetric VALUES ( '";
                    trans += getParser()->getToken(4)->getLexem() + "', '";
                    trans += getParser()->getToken(6)->getLexem() + "', ";
                    trans += dictionary()->getMetricCode(currentToken().getLexem().c_str()).toStdString() + ", ";
                    trans += "'N', '";
                    trans += indexName + "', '";
                    trans += indexName + ".dat' )";

                    indexName += ".dat";

                    //Adding feature vectors to the new index
                    std::string table = getParser()->getToken(4)->getLexem();
                    std::string column = getParser()->getToken(6)->getLexem();
                    std::string metricName = currentToken().getLexem();

                    Lexical lex;
                    std::vector<FeatureVector *> fVectors;
                    FeatureVector fv;
                    std::string cpTable = dictionary()->tablePrefix(dictionary()->columnDataType(table.c_str(), column.c_str())).toStdString();

                    QPair<QStringList, QStringList> complexDataAndRowId = dictionary()->getComplexAttributeAndRowId(cpTable.c_str(), column.c_str());
                    WrapperDF *eval = new WrapperDF(dictionary()->getDistanceFunction(table.c_str(), column.c_str(), metricName.c_str()).toInt());

                    for (int m = 0; m < complexDataAndRowId.first.size(); m++){
                        fv.unserializeFromString(FeatureVector::fromBase64(complexDataAndRowId.first.at(m).toStdString()));
                        fVectors.push_back(&fv);
                    }

                    IndexManager *idx = new IndexManager(indexName, *eval, false, IndexManager::PAGE_SIZE, true);
                    for (size_t x = 0; x < fVectors.size(); x++){
                        idx->addItem(fVectors[x]);
                        delete (fVectors[x]);
                    }
                    fVectors.clear();

                    delete (eval);
                    delete (idx);
                }
            }
        }

        answer.push_back(trans);
        trans.clear();

        getParser()->resetParser();
    } catch (...){
    }

    return answer;
}

