#ifndef CREATEINDEXSYNTACTIC_H
#define CREATEINDEXSYNTACTIC_H

#include <SQLSyntactic.h>

class CreateIndexSyntactic : public SQLSyntactic{

    private:
        std::string tableName;
        std::vector<std::string> columnName;
        std::string indexName;

    private:
        void destroyAuxiliaryStructures();

        void index_name();
        void table_name();
        void column_name();
        void metric_name();
        void create_index_specification();

        std::string getTableName();
        std::string getColumnName(size_t pos) throw (std::exception*);
        std::string getIndexName();

    public:
        CreateIndexSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }


        CreateIndexSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            create_index_specification();
        }

        virtual ~CreateIndexSyntactic();

        bool isValid(std::string sqlStatement);

};

#endif // CREATEINDEXSYNTACTIC_H
