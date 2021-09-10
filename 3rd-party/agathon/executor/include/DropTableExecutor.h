#ifndef DROPTABLEEXECUTOR_HPP
#define DROPTABLEEXECUTOR_HPP

#include <SQLExecutor.h>

class DropTableExecutor : public SQLExecutor{

    public:
        DropTableExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        DropTableExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        ~DropTableExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};


#endif // DROPTABLEEXECUTOR_HPP
