#ifndef CREATEINDEXEXECUTOR_HPP
#define CREATEINDEXEXECUTOR_HPP

#include <SQLExecutor.h>
#include <IndexManager.h>

class CreateIndexExecutor : public SQLExecutor{

    private:
        std::vector<std::string> columnName;

    private:
        void column_name();
        std::string getColumnName(size_t pos) throw (std::exception*);

    public:
        CreateIndexExecutor(DatabaseManager *dbmg  = nullptr) : SQLExecutor(dbmg){
        }

        CreateIndexExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
        }
        ~CreateIndexExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};


#endif // CREATEINDEXEXECUTOR_HPP
