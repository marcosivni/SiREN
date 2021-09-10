#ifndef UpdateSyntactic_HPP
#define UpdateSyntactic_HPP

#include<SQLSyntactic.h>

class UpdateSyntactic : public SQLSyntactic{

    private:
        std::string tableName;

    private:
        void destroyAuxiliaryStructures();

        void table_name();
        void column_name();
        void value_name();
        void update_reference();
        void update_list();
        void where_clause();
        void column_where_list();
        void column_where();
        void update_specification();

        std::string getTableName();

    public:
        UpdateSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }

        UpdateSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            update_specification();
        }

        virtual ~UpdateSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // UpdateSyntactic_HPP
