#ifndef ALTERTABLESYNTACTIC_H
#define ALTERTABLESYNTACTIC_H

#include <SQLSyntactic.h>

class AlterTableSyntactic : public SQLSyntactic{

    private:
        std::string tableName;

    private:
        void destroyAuxiliaryStructures();

        void table_name();
        void column_name();
        void value_name(int op = 0);
        void command_name();
        void alter_table_specification();

        std::string getTableName();

    public:
        AlterTableSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }

        AlterTableSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            alter_table_specification();
        }

        virtual ~AlterTableSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // ALTERTABLESYNTACTIC_H
