#ifndef DescSyntactic_HPP
#define DescSyntactic_HPP

#include <SQLSyntactic.h>

class DescSyntactic: public SQLSyntactic{

    private:
        void table_name();
        void metric_name();
        void desc_specification();


    public:
        DescSyntactic(std::string sql, DatabaseManager *dbmg): SQLSyntactic (sql, dbmg){
        }

        DescSyntactic(DatabaseManager *dbmg = nullptr): SQLSyntactic (dbmg){
        }

        bool isValid(std::string sqlStatement);

        ~DescSyntactic();
};

#endif // DescSyntactic_HPP
