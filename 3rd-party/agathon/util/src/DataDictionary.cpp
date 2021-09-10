#include "DataDictionary.h"

DataDictionary::DataDictionary(DatabaseManager *db){

    this->db = db;
}

DataDictionary::~DataDictionary(){
}

bool DataDictionary::isMetric(unsigned int distanceCode){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT UPPER(DistanceType) FROM EPD$DistanceFunctions WHERE DistanceCode ='" + QString::number(distanceCode) + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT UPPER(DistanceType) FROM EPD$DistanceFunctions WHERE DistanceCode ='" + QString::number(distanceCode) + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = ( (!resultSet->empty()) && (resultSet->value(db->fetchAttributesNames().at(0)).at(0).toUpper().compare("METRICAL")) );

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isMetric(QString metricName){

    bool answer = false;
    QString query;

    DatabaseManager::DBMSType aux = db->dbms();

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (!resultSet->empty());

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isValidMetric(QString metricName, QString tableName, QString attributeName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + attributeName + "') AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "'))";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + attributeName + "') AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "'))";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = ( (!resultSet->empty()) && (!resultSet->value(db->fetchAttributesNames().at(0)).at(0).toUpper().compare("0")) );

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isPrimaryKey(QString tableName, QString columnName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT k.column_name FROM information_schema.table_constraints t JOIN information_schema.key_column_usage k USING(constraint_name,table_schema,table_name) WHERE t.constraint_type='PRIMARY KEY' AND t.table_schema='SIREN' AND t.table_name='"+ tableName +"' AND k.column_name IN ('" + columnName + "')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT name FROM pragma_table_info ('" + tableName + "') WHERE pk = 1 AND name ='" + columnName +"'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}


bool DataDictionary::isValidPrimaryKey(QString value, QString tableName, QString columnName){

    bool answer = false;
    QString query;

    if (isPrimaryKey(tableName, columnName)) {
        if(db->dbms() == DatabaseManager::MYSQL) {
            query = "SELECT " + columnName + " FROM " + tableName + " WHERE " + columnName + " = '" + value + "'";
        } else {
            if(db->dbms() == DatabaseManager::SQLITE){
                query = "SELECT " + columnName + " FROM " + tableName + " WHERE " + columnName + " = '" + value + "'";
            } else {
                throw std::out_of_range("DBMS not supported yet!");
            }
        }
        QMap<QString, QStringList> *resultSet = db->runSelect(query);
        answer = (resultSet->size() > 0);

        if (resultSet != nullptr)
            delete (resultSet);
    }

    return answer;
}

bool DataDictionary::isValidDatabase(QString databaseName){

    return databaseName.compare("SIREN");

}

bool DataDictionary::isValidDataType(QString dataType){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT DISTINCT UPPER(DATA_TYPE) FROM information_schema.COLUMNS WHERE UPPER(DATA_TYPE)='" + dataType + "'";

        QMap<QString, QStringList> *resultSet = db->runSelect(query);
        answer = (resultSet->size() > 0) || (dataType.toUpper() == "INTEGER");

        if (resultSet != nullptr)
            delete (resultSet);
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            QStringList dataTypes({"INTEGER", "REAL", "TEXT", "BLOB", "VARCHAR"});
            answer = dataTypes.filter(dataType).size() > 0;
            dataTypes.clear();
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }
    return answer;
}

bool DataDictionary::isValidComplexDataType(QString dataType){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT id FROM EPD$DataTypes WHERE UPPER(name)='"+ dataType +"'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT id FROM EPD$DataTypes WHERE UPPER(name)='"+ dataType +"'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isValidIndex(QString tableName, QString indexName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT INDEX_TYPE FROM information_schema.statistics WHERE TABLE_SCHEMA='SIREN' AND TABLE_NAME='"+ tableName +"' AND INDEX_NAME='" + indexName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT * FROM sqlite_master WHERE type = 'index' AND name = '" + indexName + "' and tbl_name = '"+ tableName +"'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isValidTable(QString tableName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT ENGINE FROM information_schema.TABLES WHERE TABLE_SCHEMA='SIREN' AND TABLE_NAME='"+ tableName +"'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT * FROM pragma_table_info ('"+ tableName +"')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isValidColumn(QString tableName, QString columnName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='SIREN' AND TABLE_NAME='" + tableName + "' AND COLUMN_NAME='" + columnName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT name FROM pragma_table_info ('" + tableName + "') WHERE name = '" + columnName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::areValidColumns(QStringList tableName, QStringList columnName){

    bool answer = false;

    if (tableName.size() == columnName.size()){
        answer = true;
        for (int i = 0; (answer && i < tableName.size()); i++){
            answer = answer && isValidColumn(tableName.at(i), columnName.at(i));
        }
    }

    return answer;
}

bool DataDictionary::isDistanceFunctionName(QString distanceName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName + "')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName + "')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}


bool DataDictionary::isComplexAttributeReference(QString tableName, QString attributeName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT ComplexAttribId AS complexid FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT ComplexAttribId AS complexid FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isComplexAttributeReference(QString attributeName){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT count(*) FROM CDD$ComplexAttribute WHERE ComplexAttribName = '" + attributeName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT count(*) FROM CDD$ComplexAttribute WHERE ComplexAttribName = '" + attributeName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isComplexAttributeIndex(QString caIndex){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

bool DataDictionary::isComplexAttributeIndex(QString tableName, QString caIndex){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "' AND TableName='" + tableName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "' AND TableName='" + tableName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}


bool DataDictionary::isComplexAttribMetricTuple(QString tableName, QString caAttribName, QString metricCode){

    bool answer = false;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL) {
        query = "SELECT * FROM CDD$ComplexAttribMetricTuple WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + caAttribName + "') AND MetricCode='" + metricCode + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE){
            query = "SELECT * FROM CDD$ComplexAttribMetricTuple WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + caAttribName + "') AND MetricCode='" + metricCode + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(query);
    answer = (resultSet->size() > 0);

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

int DataDictionary::runSequence(QString sequenceName){

    QString strSql1 = "";
    QString strSql2 = "";
    QString strSql3 = "";
    int result = -1;

    if(db->dbms() == DatabaseManager::MYSQL){
        strSql1 = "SELECT lastValue FROM EPD$Sequences WHERE name = '" + sequenceName + "'";
        strSql2 = "INSERT INTO EPD$Sequences VALUES ( '" + sequenceName + "', '0' )";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            strSql1 = "SELECT lastValue FROM EPD$Sequences WHERE name = '" + sequenceName + "'";
            strSql2 = "INSERT INTO EPD$Sequences VALUES ( '" + sequenceName + "', '0' )";
        }else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    QMap<QString, QStringList> *resultSet = db->runSelect(strSql1);

    if(resultSet->size() > 0){
        result = resultSet->cbegin().value().at(0).toUInt();
        result++;

        if(db->dbms() == DatabaseManager::MYSQL){
            strSql3 = "UPDATE EPD$Sequences SET lastValue=" + QString::number(result) + " WHERE name = '" + sequenceName + "'";
        } else {
            if(db->dbms() == DatabaseManager::SQLITE) {
                strSql3 = "UPDATE EPD$Sequences SET lastValue=" + QString::number(result) + " WHERE name = '" + sequenceName + "'";
            } else {
                throw std::out_of_range("DBMS not supported yet!");
            }
        }

        db->runInsert(strSql3);
    } else {
        db->runInsert(strSql2);
        result = 0;
    }

    return result;
}

QString DataDictionary::newIndexId(){

    unsigned int cid = runSequence("IndexIdSequence");
    return QString::number(cid);
}

QString DataDictionary::newMetricId(){

    unsigned int cid = runSequence("MetricIdSequence");
    return QString::number(cid);
}

QStringList DataDictionary::getComplexAttributeReferenceId(QString table, QString column, QString whereClause){

    QMap<QString, QStringList> *resultSet;
    QStringList answer;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT " + column + " FROM " + table;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT " + column + " FROM " + table;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    if (!whereClause.compare("")){
        query += " WHERE " + whereClause;
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::tablePrefix(QString complexData){

    QString query;
    QString answer;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT UPPER(acronym) FROM CDD$Acronym WHERE UPPER(caType) = UPPER('" + complexData + "')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT UPPER(acronym) FROM CDD$Acronym WHERE UPPER(caType) = UPPER('" + complexData + "')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::complexColumnDataType(QString tableName, QString columnName){

    QString answer;
    QString strSql;
    QMap<QString, QStringList> *resultSet;


    if(db->dbms() == DatabaseManager::MYSQL){
        strSql = "SELECT UPPER(e.name) FROM CDD$ComplexAttribute c JOIN EPD$DataTypes e ON c.ComplexAttribId = e.id WHERE TableName='" + tableName + "' AND ComplexAttribName = '" + columnName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            strSql = "SELECT UPPER(e.name) FROM CDD$ComplexAttribute c JOIN EPD$DataTypes e ON c.ComplexAttribId = e.id WHERE TableName='" + tableName + "' AND ComplexAttribName = '" + columnName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(strSql);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::columnDataType(QString tableName, QString columnName){

    QString answer;
    QString strSql1;
    QString strSql2;
    QMap<QString, QStringList> *resultSet;


    if(db->dbms() == DatabaseManager::MYSQL){
        strSql1 ="SELECT UPPER(e.name) FROM CDD$ComplexAttribute c JOIN EPD$DataTypes e ON c.ComplexAttribId = e.id WHERE TableName='" + tableName + "' AND ComplexAttribName = '" + columnName + "'";
        strSql2 = "SELECT UPPER(DATA_TYPE) FROM information_schema.COLUMNS WHERE TABLE_NAME='" + tableName +"' AND COLUMN_NAME='" + columnName + "' AND TABLE_SCHEMA='SIREN'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            strSql1 = "SELECT UPPER(e.name) FROM CDD$ComplexAttribute c JOIN EPD$DataTypes e ON c.ComplexAttribId = e.id WHERE TableName='" + tableName + "' AND ComplexAttribName = '" + columnName + "'";
            strSql2 = "SELECT type FROM pragma_table_info ('" + tableName + "') WHERE name='" + columnName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(strSql1);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    } else {
        if (resultSet != nullptr)
            delete (resultSet);

        resultSet = db->runSelect(strSql2);
        if (resultSet->size() > 0){
            answer = resultSet->constBegin().value().at(0);
        }
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::generateGetIndicesCommand(){

    QString answer;

    if(db->dbms() == DatabaseManager::MYSQL){
        answer = "SELECT INDEX_NAME, TABLE_NAME, INDEX_TYPE FROM information_schema.statistics WHERE TABLE_SCHEMA='SIREN' UNION SELECT IndexName, TableName, 'SLIMTREE' FROM CDD$ComplexAttribMetric";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            answer = "SELECT name, tbl_name, 'BTREE' FROM sqlite_master WHERE type='index' UNION SELECT IndexName, TableName, 'SLIMTREE' FROM CDD$ComplexAttribMetric";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    return answer;
}

QString DataDictionary::generateDescMetricCommand(QString metricName){

    return "SELECT MetricName AS Metric, name AS DataType DistanceName AS DistanceFunction FROM CDD$MetricStruct m JOIN EPD$DistanceFunctions d ON m.DistanceCode = d.DistanceCode JOIN EPD$DataTypes e ON e.id = m.DataTypeId WHERE MetricName='" + metricName + "'";
}

QString DataDictionary::generateDescTableCommand(QString tableName) {

    QString answer;

    if(db->dbms() == DatabaseManager::MYSQL){
        answer = "DESC " + tableName;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            answer = "SELECT * FROM pragma_table_info ('" + tableName + "');";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    return answer;

}

QString DataDictionary::getDistanceName(QString distanceCode){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT df.DistanceName FROM CDD$ComplexAttribute ca JOIN CDD$ComplexAttribMetric cm ON ca.TableName=cm.TableName AND ca.ComplexAttribName = cm.ComplexAttribName JOIN CDD$MetricStruct ms ON cm.MetricCode = ms.MetricCode JOIN EPD$DistanceFunctions df ON ms.DistanceCode = df.DistanceCode WHERE df.DistanceCode = " + distanceCode;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT df.DistanceName FROM CDD$ComplexAttribute ca JOIN CDD$ComplexAttribMetric cm ON ca.TableName=cm.TableName AND ca.ComplexAttribName = cm.ComplexAttribName JOIN CDD$MetricStruct ms ON cm.MetricCode = ms.MetricCode JOIN EPD$DistanceFunctions df ON ms.DistanceCode = df.DistanceCode WHERE df.DistanceCode = " + distanceCode;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QStringList DataDictionary::allValidComplexDataTypes(){

    QStringList answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT name FROM EPD$DataTypes";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT name FROM EPD$DataTypes";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);


    return answer;
}

QStringList DataDictionary::allColumnNamesFromTable(QString tableName){

    QStringList answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='SIREN' AND TABLE_NAME='" + tableName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT name FROM pragma_table_info ('" + tableName + "');";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QPair<QStringList, QStringList> DataDictionary::allTablesAndComplexColumnNames(QString metricCode){

    QPair<QStringList, QStringList> answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT DISTINCT TableName AS ctn, ComplexAttribName AS cttan FROM CDD$ComplexAttribMetric WHERE MetricCode='" + metricCode + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT DISTINCT TableName AS ctn, ComplexAttribName AS cttan FROM CDD$ComplexAttribMetric WHERE MetricCode='" + metricCode + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer.first = resultSet->constFind("TableName").value();
        answer.second = resultSet->constFind("ComplexAttribName").value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QPair<QStringList, QStringList> DataDictionary::allNullableColumnsInATable(QString tableName){

    QPair<QStringList, QStringList> answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query ="SELECT COLUMN_NAME AS column, IS_NULLABLE AS nullable FROM information_schema.COLUMNS WHERE TABLE_NAME='" + tableName + "' AND TABLE_SCHEMA='SIREN'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT name AS column,\"notnull\" AS nullable FROM pragma_table_info('" + tableName + "')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer.first = resultSet->constFind("TableName").value();
        answer.second = resultSet->constFind("ComplexAttribName").value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QPair<QStringList, QStringList> DataDictionary::getComplexAttributeAndRowId(QString tableName, QString column){

    QPair<QStringList, QStringList> answer;
    QString query;
    QMap<QString, QStringList> *resultSet;
    QString id = column + "_id";


    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT " + column + " AS c1, " + id + " AS c2 FROM " + tablePrefix(complexColumnDataType(tableName, column)) + "$" +tableName + "_" + column;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT " + column + " AS c1, " + id + " AS c2 FROM " + tablePrefix(complexColumnDataType(tableName, column)) + "$" +tableName + "_" + column;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer.first = resultSet->constFind("TableName").value();
        answer.second = resultSet->constFind("ComplexAttribName").value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getDefaultDistanceCode(){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT MIN(DistanceCode) FROM EPD$DistanceFunctions";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT MIN(DistanceCode) FROM EPD$DistanceFunctions";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getDistanceCode(QString distanceName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName +"')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName +"')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getComplexDataTypeCode(QString dataType){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT id FROM EPD$DataTypes WHERE UPPER(name)='"+ dataType +"'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT id FROM EPD$DataTypes WHERE UPPER(name)='"+ dataType +"'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getMetricCode(QString metricName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getComplexAttributeReference(QString tableName, QString attributeName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT UPPER(ComplexAttribType) AS complextype FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT UPPER(ComplexAttribType) AS complextype FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getDefaultMetric(QString tableName, QString attribName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT mi.MetricName FROM CDD$MetricStruct mi, CDD$ComplexAttribMetric ms WHERE TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "' AND UPPER(IsDefault) = 'Y' AND mi.MetricCode=ms.MetricCode";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT mi.MetricName FROM CDD$MetricStruct mi, CDD$ComplexAttribMetric ms WHERE TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "' AND UPPER(IsDefault) = 'Y' AND mi.MetricCode=ms.MetricCode";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getMetricName(QString metricCode){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT UPPER(MetricName) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT UPPER(MetricName) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getMetricType(QString metricCode){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT UPPER(MetricType) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT UPPER(MetricType) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QStringList DataDictionary::getMetrics(QString tableName, QString attribName){

    QStringList answer;
    QString query;
    QMap<QString, QStringList> *resultSet;


    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "select MetricCode from CDD$ComplexAttribMetric where TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "select MetricCode from CDD$ComplexAttribMetric where TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getIndexFile(QString tableName, QString indexName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query =  "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im WHERE im.TableName = '" + tableName + "' AND im.IndexName = '" + indexName + "' AND UPPER(IsDefault)='N'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im WHERE im.TableName = '" + tableName + "' AND im.IndexName = '" + indexName + "' AND UPPER(IsDefault)='N'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getIndexFile(QString tableName, QString attribName, QString metricCode){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im WHERE im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + attribName + "' AND im.MetricCode = " + metricCode;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im WHERE im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + attribName + "' AND im.MetricCode = " + metricCode;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QStringList DataDictionary::getComplexAttributes(QString tableName){

    QMap<QString, QStringList> *resultSet;
    QStringList answer;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QStringList DataDictionary::getParticulateAttribute(QString tableName){

    QMap<QString, QStringList> *resultSet;
    QStringList answer;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND UPPER(ComplexAttribType) = UPPER('particulate')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND UPPER(ComplexAttribType) = UPPER('particulate')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getDistanceFunction(QString metricCode){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT DistanceCode FROM CDD$MetricStruct ms WHERE ms.MetricCode = '" + metricCode + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT DistanceCode FROM CDD$MetricStruct ms WHERE ms.MetricCode = '" + metricCode + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::getDistanceFunction(QString tableName, QString attribName, QString metricName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT DistanceCode FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms WHERE im.MetricCode = ms.MetricCode AND UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + attribName + "') AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query =  "SELECT DistanceCode FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms WHERE im.MetricCode = ms.MetricCode AND UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + attribName + "') AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QStringList DataDictionary::getParametersNameForAlterPVTable(QString metric, QString metricCode){

    QMap<QString, QStringList> *resultSet;
    QStringList answer;
    QString query;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT ParameterName FROM CDD$ParameterStruct WHERE MetricCode = " + metricCode;
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT ParameterName FROM CDD$ParameterStruct WHERE MetricCode = " + metricCode;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value();
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

int DataDictionary::numberOfParticulateDimensions(QString tableName, QString attribName){

    QString answer;
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT ComplexAttribDimensionality FROM CDD$ComplexAttribMetric mi WHERE mi.TableName = '" + tableName + "' AND mi.ComplexAttribName = '" + attribName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT ComplexAttribDimensionality FROM CDD$ComplexAttribMetric mi WHERE mi.TableName = '" + tableName + "' AND mi.ComplexAttribName = '" + attribName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer.toInt();
}


QString DataDictionary::generateTableNameCompAttNameMetricCodeIdxFile(QString indexName){

    QString answer;

    if(db->dbms() == DatabaseManager::MYSQL){
        answer = "select TableName, ComplexAttribName, MetricCode, IndexFile from CDD$ComplexAttribMetric where IndexName = '" + indexName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            answer  = "select TableName, ComplexAttribName, MetricCode, IndexFile from CDD$ComplexAttribMetric where IndexName = '" + indexName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    return answer;
}

QString DataDictionary::getValidMetric(QString metricName, QString tableName, QString attributeName){

    QString answer = "NULL";
    QString query;
    QMap<QString, QStringList> *resultSet;

    if(db->dbms() == DatabaseManager::MYSQL){
        query = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "' AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
    } else {
        if(db->dbms() == DatabaseManager::SQLITE) {
            query = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "' AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    resultSet = db->runSelect(query);
    if (resultSet->size() > 0){
        answer = resultSet->constBegin().value().at(0);
    }

    if (resultSet != nullptr)
        delete (resultSet);

    return answer;
}

QString DataDictionary::dropIndexStatement(QString table, QString indexName){

    QString answer;

    if(db->dbms() ==  DatabaseManager::MYSQL){
        answer = "ALTER TABLE " + table + " DROP INDEX " + indexName;
    } else {
        if(db->dbms()  == DatabaseManager::SQLITE){
            answer =  "DROP INDEX " + indexName;
        } else {
            throw std::out_of_range("DBMS not supported yet!");
        }
    }

    return answer;
}




























