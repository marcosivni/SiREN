#ifndef DATADICTIONARY_H
#define DATADICTIONARY_H

#include <DatabaseManager.h>

class DataDictionary{

    private:
        DatabaseManager *db;

    public:
        DataDictionary(DatabaseManager *db = nullptr);
        ~DataDictionary();

        bool isMetric(unsigned int distanceCode);
        bool isMetric(QString metricName);
        bool isValidMetric(QString metricName, QString tableName, QString attributeName);
        bool isPrimaryKey(QString tableName, QString columnName);
        bool isValidPrimaryKey(QString value, QString tableName, QString columnName);
        bool isValidDatabase(QString databaseName);
        bool isValidDataType(QString dataType);
        bool isValidComplexDataType(QString dataType);
        bool isValidIndex(QString tableName, QString indexName);
        bool isValidTable(QString tableName);
        bool isValidColumn(QString tableName, QString columnName);
        bool areValidColumns(QStringList tableName, QStringList columnName);
        bool isDistanceFunctionName(QString distanceName);
        bool isComplexAttributeReference(QString tableName, QString attributeName);
        bool isComplexAttributeReference(QString attributeName);
        bool isComplexAttributeIndex(QString caIndex);
        bool isComplexAttributeIndex(QString tableName, QString caIndex);
        bool isComplexAttribMetricTuple(QString tableName, QString caAttribName, QString metricCode);

        int runSequence(QString sequenceName);
        QString newIndexId();
        QString newMetricId();

        QString complexColumnDataType(QString tableName, QString columnName);
        QString columnDataType(QString tableName, QString columnName);
        QString tablePrefix(QString complexData);

        QString generateGetIndicesCommand();
        QString generateDescMetricCommand(QString metricName);
        QString generateDescTableCommand(QString tableName);
        QString generateTableNameCompAttNameMetricCodeIdxFile(QString indexName);


        QStringList allColumnNamesFromTable(QString tableName);
        QPair<QStringList, QStringList> allNullableColumnsInATable(QString tableName);
        QPair<QStringList, QStringList> allTablesAndComplexColumnNames(QString metricCode);
        QStringList allValidComplexDataTypes();

        QPair<QStringList, QStringList> getComplexAttributeAndRowId(QString tableName, QString column);
        QString getComplexAttributeReference(QString tableName, QString attributeName);
        QStringList getComplexAttributeReferenceId(QString table, QString column, QString whereClause = "");
        QStringList getComplexAttributes(QString tableName);
        QString getComplexDataTypeCode(QString dataType);

        QString getDefaultDistanceCode();
        QString getDefaultMetric(QString tableName, QString attribName);

        QString getDistanceCode(QString distanceName);
        QString getDistanceFunction(QString metricCode);
        QString getDistanceFunction(QString tableName, QString attribName, QString metricName);
        QString getDistanceName(QString distanceCode);

        QString getIndexFile(QString tableName, QString indexName);
        QString getIndexFile(QString tableName, QString attribName, QString metricName);

        QString getMetricCode(QString metricName);
        QString getMetricName(QString metricCode);
        QString getMetricType(QString metricCode);
        QStringList getMetrics(QString tableName, QString attribName);

        int numberOfParticulateDimensions(QString tableName, QString attribName);

        QStringList getParticulateAttribute(QString tableName);
        QStringList getParametersNameForAlterPVTable(QString metric, QString metricCode);

        QString getValidMetric(QString metricName, QString tableName, QString attributeName);

        QString dropIndexStatement(QString table, QString indexName);
};

#endif // DATADICTIONARY_H
