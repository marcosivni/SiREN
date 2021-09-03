#ifndef UPDATEEXECUTOR_HPP
#define UPDATEEXECUTOR_HPP

#include <SQLExecutor.h>

class UpdateExecutor : public SQLExecutor{

    public:
        UpdateExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        UpdateExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        std::vector<std::string> translate(std::string sqlStatement);
        bool isExecutable();
};

#endif // UPDATEEXECUTOR_HPP
