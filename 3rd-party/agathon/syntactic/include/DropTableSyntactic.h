#ifndef DropTableSyntactic_HPP
#define DropTableSyntactic_HPP

#include <SQLSyntactic.h>

class DropTableSyntactic : public SQLSyntactic{

    private:
        void table_name();
        void drop_table_specification();

    public:
        DropTableSyntactic(DatabaseManager *dbmg = nullptr):SQLSyntactic (dbmg){
        }

        DropTableSyntactic(std::string sql, DatabaseManager *dbmg):SQLSyntactic (sql, dbmg){
            drop_table_specification();
        }

        virtual ~DropTableSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // DropTableSyntactic_HPP
