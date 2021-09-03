#ifndef CREATETABLESYNTACTIC_H
#define CREATETABLESYNTACTIC_H

#include <SQLSyntactic.h>
#include <CreateTableSemanticStructs.h>

class CreateTableSyntactic : public SQLSyntactic{

    private:
        Constraint currentConstraint;
        CheckValidColumnsAndTypes *checkColumns;
        CheckConstraints *checkConstraints;

    private:
        void destroyAuxiliaryStructures();

        //Methods for syntatic analyzer
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
        void create_table_specification();

    public:

        CreateTableSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic(dbmg){
            checkColumns = new CheckValidColumnsAndTypes(dictionary());
            checkConstraints = new CheckConstraints(dictionary());
        }

        CreateTableSyntactic(std::string expression, DatabaseManager *dbmg) : SQLSyntactic(expression, dbmg){
            checkColumns = new CheckValidColumnsAndTypes(dictionary());
            checkConstraints = new CheckConstraints(dictionary());
            create_table_specification();
        }

        virtual ~CreateTableSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // CREATETABLESYNTACTIC_H
