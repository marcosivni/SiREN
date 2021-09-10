#ifndef InsertIntoSyntactic_HPP
#define InsertIntoSyntactic_HPP

#include <SQLSyntactic.h>
#include <QString>
#include <QStringList>

class InsertIntoSyntactic : public SQLSyntactic{

    private:
        std::string tableName;
        std::vector<std::string> columns;
        std::vector<std::string> values;

    private:
        void destroyAuxiliaryStructures();

        //Syntactic Methods
        void table_name();
        void insert_column();
        void insert_column_list();
        std::string build_particulate();
        std::string build_particulate_list(size_t *count);
        void insert_value(size_t *position);
        void insert_value_list(size_t *position);
        void load_all_columns();
        void insert_specification();

        //Semantic Methods
        std::string getTableName();
        void checkColumnsAndValues();

    public:
        InsertIntoSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }


        InsertIntoSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            insert_specification();
        }

        virtual ~InsertIntoSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // InsertIntoSyntactic_HPP
