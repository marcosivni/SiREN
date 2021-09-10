#ifndef ALTERTABLEEXECUTOR_HPP
#define ALTERTABLEEXECUTOR_HPP

#include <SQLExecutor.h>

class AlterTableExecutor : public SQLExecutor{

    private:
        std::string tableName;

    public:
        AlterTableExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        AlterTableExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }
        ~AlterTableExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // ALTERTABLEEXECUTOR_HPP
