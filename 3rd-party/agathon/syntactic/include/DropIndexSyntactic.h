#ifndef DROPINDEXSYNTACTIC_H
#define DROPINDEXSYNTACTIC_H

#include <SQLSyntactic.h>

class DropIndexSyntactic : public SQLSyntactic{

    private:
        std::string tableName;

    private:
        void destroyAuxiliaryStructures();

        void index_name();
        void table_name();
        void drop_index_specification();

        std::string getTableName();

    public:
        DropIndexSyntactic(DatabaseManager *dbmg = nullptr)  : SQLSyntactic (dbmg){
        }

        DropIndexSyntactic(std::string sql, DatabaseManager *dbmg)  : SQLSyntactic (sql, dbmg){
            drop_index_specification();
        }

        virtual ~DropIndexSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // DROPINDEXSYNTACTIC_H
