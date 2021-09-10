#ifndef INSERTINTOEXECUTOR_HPP
#define INSERTINTOEXECUTOR_HPP

#include <SQLExecutor.h>
#include <BasicArrayObject.h>
#include <IndexManager.h>


class InsertIntoExecutor : public SQLExecutor{

    private:
        std::vector<std::string> columns;
        std::vector<std::string> values;
        std::string tableName;

    private:
        std::string additionalInsertIntoStatement(std::string table, std::string column, std::string dataType, std::string key, std::string complexDataPath);
        std::string originalInsertIntoStatement();

        FeatureVector build_particulate_list();

        void clear();
        void load_all_columns();
        void insert_column_list();
        void insert_value_list();

    public:
        InsertIntoExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
        }

        InsertIntoExecutor(std::string sirenSqlStatement, DatabaseManager *dbmg) : SQLExecutor(sirenSqlStatement, dbmg){
        }

        virtual ~InsertIntoExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // INSERTINTOEXECUTOR_HPP
