#ifndef DESCEXECUTOR_HPP
#define DESCEXECUTOR_HPP

#include <SQLExecutor.h>

class DescExecutor : public SQLExecutor{

    public:
        DescExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        DescExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }
        ~DescExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // DESCEXECUTOR_HPP
