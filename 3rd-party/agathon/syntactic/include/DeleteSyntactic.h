#ifndef DELETESYNTACTIC_H
#define DELETESYNTACTIC_H

#include <SQLSyntactic.h>

class DeleteSyntactic : public SQLSyntactic{

    private:
        std::string tableName;

    private:
        void destroyAuxiliaryStructures();

        void table_name();
        void where_clause();
        void column_name();
        void value_name();
        void column_reference();
        void column_reference_list();
        void delete_specification();

        std::string getTableName();

    public:
        DeleteSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }

        DeleteSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            delete_specification();
        }

        virtual ~DeleteSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // DELETESYNTACTIC_H
