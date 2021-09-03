#ifndef SIRENSQLQUERY_H
#define SIRENSQLQUERY_H

// Create metric
#include <CreateMetricSyntactic.h>
#include <CreateMetricExecutor.h>
// Create metric
#include <CreateIndexSyntactic.h>
#include <CreateIndexExecutor.h>
// Create table
#include <CreateTableSyntactic.h>
#include <CreateTableExecutor.h>
//Drop metric
#include <DropMetricSyntactic.h>
#include <DropMetricExecutor.h>
//Drop metric
#include <DropIndexSyntactic.h>
#include <DropIndexExecutor.h>
//DropTable
#include <DropTableSyntactic.h>
#include <DropTableExecutor.h>
//Alter table
#include <AlterTableSyntactic.h>
#include <AlterTableExecutor.h>
//Insert Into
#include <InsertIntoSyntactic.h>
#include <InsertIntoExecutor.h>
//Delete
#include <DeleteSyntactic.h>
#include <DeleteExecutor.h>
//Update
#include <UpdateSyntactic.h>
#include <UpdateExecutor.h>
//Select
#include <SelectSyntactic.h>
#include <SelectExecutor.h>
//Desc
#include <DescSyntactic.h>
#include <DescExecutor.h>


class SirenSQLQuery{

    private:
        DatabaseManager *dbmg;
        std::vector<std::string> qErrors;
        std::vector<std::string> qAnswer;
        std::string sqlStatement;
        bool prepared;

        CreateMetricSyntactic createMetricS;
        CreateMetricExecutor createMetricE;
        CreateIndexSyntactic createIndexS;
        CreateIndexExecutor createIndexE;
        CreateTableSyntactic createTableS;
        CreateTableExecutor createTableE;
        DropMetricSyntactic dropMetricS;
        DropMetricExecutor dropMetricE;
        DropIndexSyntactic dropIndexS;
        DropIndexExecutor dropIndexE;
        DropTableSyntactic dropTableS;
        DropTableExecutor dropTableE;
        AlterTableSyntactic alterTableS;
        AlterTableExecutor alterTableE;
        InsertIntoSyntactic insertS;
        InsertIntoExecutor insertE;
        DeleteSyntactic deleteS;
        DeleteExecutor deleteE;
        UpdateSyntactic updateS;
        UpdateExecutor updateE;
        SelectSyntactic selectS;
        SelectExecutor selectE;
        DescSyntactic descS;
        DescExecutor descE;

    public:
        enum SIRENQueryType {CREATE_METRIC, CREATE_INDEX, CREATE_TABLE, DROP_METRIC, DROP_INDEX, DROP_TABLE, ALTER_TABLE, INSERT, DELETE, UPDATE, SELECT, DESC};

    private:
        void clear();
        void init(DatabaseManager *dbmg);

    public:
        SirenSQLQuery(){
        }

        SirenSQLQuery(DatabaseManager *dbmg){
        }

        SirenSQLQuery(std::string sqlStatement, DatabaseManager *dbmg){
            this->sqlStatement = sqlStatement;
        }

        virtual ~SirenSQLQuery(){
            clear();
        }

        bool isValid(std::string sqlStatement);
        bool prepare(std::string sqlStatement, bool syntacticOnly = false);
        bool runQuery();

        bool hasErrors();
        bool isPrepared();

        std::vector<std::string> errors();
        std::vector<std::string> translation();

};

#endif // SIRENSQLQUERY_H
