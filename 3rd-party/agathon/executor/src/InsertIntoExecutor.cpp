#include "InsertIntoExecutor.h"

InsertIntoExecutor::~InsertIntoExecutor(){

    clear();
}

std::string InsertIntoExecutor::additionalInsertIntoStatement(std::string table, std::string column, std::string dataType, std::string key, std::string complexData){

    std::string strSql;
    FeatureVector fv;
    QStringList metrics = dictionary()->getMetrics(table.c_str(), column.c_str());

    fv.unserializeFromString(complexData);
    fv.setOID(QString(key.c_str()).toUInt());
    //Underlying DBMS may not offer the expected support to byte arrays... =(
    complexData = FeatureVector::toBase64(fv.serializeToString());

    strSql = "INSERT INTO " + dictionary()->tablePrefix(dataType.c_str()).toStdString();
    strSql += "$" + table + "_" + column + "(" + column + "_id, " + column + ") VALUES (" + key + ", '" + complexData +"')";

    for (int x = 0; x < metrics.size(); x++){
        std::string indexFileName = dictionary()->getIndexFile(table.c_str(), column.c_str(), metrics.at(x)).toStdString();
        MetricDistanceFunction *eval = new MetricDistanceFunction(dictionary()->getDistanceFunction(metrics.at(x)).toInt());

        IndexManager *idx = new IndexManager(indexFileName, *eval, false, IndexManager::PAGE_SIZE, true);
        idx->addItem(&fv);

        delete (eval);
        delete (idx);
    }

    return strSql;
}

std::string InsertIntoExecutor::originalInsertIntoStatement(){

    std::string aux = " (";
    for (size_t x = 0; x < columns.size(); x++){
        if (x > 0)
            aux += ",";
        aux += " " + columns[x] + " ";
    }
    aux += ") VALUES (";

    for (size_t x = 0; x < values.size(); x++){
        if (x > 0)
            aux += ",";
        aux += " " + values[x] + " ";
    }
    aux += ")";

    std::string strSql =  "INSERT INTO " + tableName + aux;

    return strSql;
}

void InsertIntoExecutor::clear(){

    for (size_t x = 0; x < columns.size(); x++){
        columns[x].clear();
    }
    columns.clear();
    for (size_t x = 0; x < values.size(); x++){
        values[x].clear();
    }
    values.clear();
    tableName.clear();

}

void InsertIntoExecutor::load_all_columns(){

    QStringList cols =  dictionary()->allColumnNamesFromTable(tableName.c_str());

    for (int x = 0; x < cols.size(); x++){
        columns.push_back(cols.at(x).toStdString());
    }
    cols.clear();
}


void InsertIntoExecutor::insert_column_list(){

    while (currentToken().getLexem() != ")"){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            columns.push_back(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }
}


FeatureVector InsertIntoExecutor::build_particulate_list(){

    FeatureVector answer;

    answer.add(QString(currentToken().getLexem().c_str()).toDouble());
    getParser()->match(getParser()->getCurrentToken());
    while (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        answer.add(QString(currentToken().getLexem().c_str()).toDouble());
        getParser()->match(getParser()->getCurrentToken());
    }

    return answer;
}

void InsertIntoExecutor::insert_value_list(){

    while (currentToken().getLexem() != ")"){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            if (currentToken().getTokenType() == Token::TK_OPEN_BLOCK){
                getParser()->match(getParser()->getCurrentToken());
                values.push_back(build_particulate_list().serializeToString());
                getParser()->match(getParser()->getCurrentToken());//Skip TK_CLOSE_BLOCK
            } else {
                values.push_back(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            }
        }
    }
}

std::vector<std::string> InsertIntoExecutor::translate(std::string sqlStatement){

    std::vector<std::string> commands;

    if (sqlStatement.size() > 0){
        getParser()->parse(sqlStatement);
    } else {
        if (getParser()->countTokens() == 0)
            return commands;
    }

    try{
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        tableName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getLexem() == "("){
            getParser()->match(getParser()->getCurrentToken());
            insert_column_list();
            getParser()->match(")", Token::TK_CLOSE_BRACE);
        } else {
            load_all_columns();
        }
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        insert_value_list();
        getParser()->match(")", Token::TK_CLOSE_BRACE);

        for (size_t x = 0; x < columns.size(); x++){
            if (dictionary()->isComplexAttributeReference(tableName.c_str(), columns[x].c_str())){
                int cid = dictionary()->runSequence(dictionary()->tablePrefix(dictionary()->complexColumnDataType(tableName.c_str(), columns[x].c_str())));
                commands.push_back(additionalInsertIntoStatement(tableName, columns[x], dictionary()->complexColumnDataType(tableName.c_str(), columns[x].c_str()).toStdString(), QString::number(cid).toStdString(), values[x]));
                values[x] = QString::number(cid).toStdString();
            }
        }
        commands.insert(commands.begin(), originalInsertIntoStatement());
    } catch (...){
    }

    return commands;
}


