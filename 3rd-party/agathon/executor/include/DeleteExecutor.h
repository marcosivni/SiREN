#ifndef DELETEEXECUTOR_HPP
#define DELETEEXECUTOR_HPP

#include <SQLExecutor.h>
#include <IndexManager.h>

class DeleteExecutor : public SQLExecutor{

    public:
        DeleteExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        DeleteExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }
        ~DeleteExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // DELETEEXECUTOR_HPP
