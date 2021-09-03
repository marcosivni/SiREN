#ifndef CREATEMETRICEXECUTOR_HPP
#define CREATEMETRICEXECUTOR_HPP

#include <SQLExecutor.h>
//#include <iostream>

class CreateMetricExecutor : public SQLExecutor{

    public:
        CreateMetricExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        CreateMetricExecutor(std::string sirenSqlStatement, DatabaseManager *dbmg) : SQLExecutor(sirenSqlStatement, dbmg){
        }

        virtual ~CreateMetricExecutor();

        std::vector<std::string> translate(std::string sqlStatement = "");
};

#endif // CREATEMETRICEXECUTOR_HPP
