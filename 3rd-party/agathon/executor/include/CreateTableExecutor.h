#ifndef CREATETABLEEXECUTOR_HPP
#define CREATETABLEEXECUTOR_HPP

#include <SQLExecutor.h>
#include <CreateTableSemanticStructs.h>

class CreateTableExecutor : public SQLExecutor{

    private:
        Constraint currentConstraint;
        CheckValidColumnsAndTypes *checkColumns;
        CheckConstraints *checkConstraints;

    private:
        void destroyAuxiliaryStructures();

        void table_name();
        void column_definition();
        void column_constraint();
        void constraint_name();
        void column_name_list(int op = 0);
        void column_name(int op = 0);
        void metric_name_list();
        void metric_name();
        void column_constraint_list();
        void data_type();
        void table_element_list();
        void table_element();
        void table_constraint();

        std::string originalCreateTable();
        std::string additionalCreateTable(std::string table, std::string column, std::string dataType);
        std::string additionalInsertIntoComplexAttribute(std::string column, std::string table, std::string dataType);
        std::vector<std::string> additionalInsertIntoComplexAttibMetric(Constraint constraint, std::string column, std::string table, std::string dimensionality);

    public:
        CreateTableExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
            checkColumns = nullptr;
            checkConstraints = nullptr;
        }

        CreateTableExecutor(std::string sql, DatabaseManager *dbmg) : SQLExecutor(sql, dbmg){
            checkColumns = new CheckValidColumnsAndTypes(dictionary());
            checkConstraints = new CheckConstraints(dictionary());
        }
        virtual ~CreateTableExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
};

#endif // CREATETABLEEXECUTOR_HPP
