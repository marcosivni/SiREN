#ifndef DROPINDEXEXECUTOR_HPP
#define DROPINDEXEXECUTOR_HPP

#include <SQLExecutor.h>
#include <fstream>
#include <cstdio>

class DropIndexExecutor : public SQLExecutor{

    public:
        DropIndexExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        DropIndexExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }

        ~DropIndexExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // DROPINDEXEXECUTOR_HPP
