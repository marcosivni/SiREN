#ifndef DROPMETRICEXECUTOR_HPP
#define DROPMETRICEXECUTOR_HPP

#include <SQLExecutor.h>
#include <cstdio>

class DropMetricExecutor : public SQLExecutor{


    public:
        DropMetricExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        DropMetricExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        ~DropMetricExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};



#endif // DROPMETRICEXECUTOR_HPP
