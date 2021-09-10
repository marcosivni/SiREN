#ifndef SirenSQLSyntactic_HPP
#define SirenSQLSyntactic_HPP

#include <SelectSyntactic.h>
#include <UpdateSyntactic.h>
#include <DeleteSyntactic.h>
#include <InsertIntoSyntactic.h>
#include <CreateTableSyntactic.h>
#include <CreateIndexSyntactic.h>
#include <CreateMetricSyntactic.h>
#include <AlterTableSyntactic.h>
#include <DropIndexSyntactic.h>
#include <DropMetricSyntactic.h>
#include <DropTableSyntactic.h>

class SirenSQLSyntactic : public SQLSyntactic{

    private:
        SelectSyntactic *select;
        UpdateSyntactic *update;
        DeleteSyntactic *deleteFrom;
        InsertIntoSyntactic *insertInto;
        CreateTableSyntactic *createTable;
        CreateIndexSyntactic *createIndex;
        CreateMetricSyntactic *createMetric;
        AlterTableSyntactic *alter;
        DropIndexSyntactic *dropIndex;
        DropMetricSyntactic *dropMetric;
        DropTableSyntactic *dropTable;

    public:
        SirenSQLSyntactic(DatabaseManager *dbmg) : SQLSyntactic (dbmg){

            select = new SelectSyntactic(dbmg);
            update = new UpdateSyntactic(dbmg);
            deleteFrom = new  DeleteSyntactic(dbmg);
            insertInto = new InsertIntoSyntactic(dbmg);
            createTable = new CreateTableSyntactic(dbmg);
            createIndex = new CreateIndexSyntactic(dbmg);
            createMetric = new CreateMetricSyntactic(dbmg);
            alter = new AlterTableSyntactic(dbmg);
            dropIndex = new DropIndexSyntactic(dbmg);
            dropMetric = new DropMetricSyntactic(dbmg);
            dropTable = new DropTableSyntactic(dbmg);
        }

        virtual ~SirenSQLSyntactic();

        bool isValid(std::string sqlStatement);

        bool isSelect();
        bool isDML();
        bool isDDL();
};

#endif // SirenSQLSyntactic_HPP
