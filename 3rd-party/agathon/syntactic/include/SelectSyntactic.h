#ifndef SelectSyntactic_HPP
#define SelectSyntactic_HPP

#include <SQLSyntactic.h>
#include <SelectExecutor.h>


class SelectSyntactic : public SQLSyntactic{

    private:
        SelectExecutor *translator = nullptr;

    private:
        void destroyAuxiliaryStructures();

    public:
        SelectSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
            translator = new SelectExecutor(dbmg);
        }

        SelectSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            translator = new SelectExecutor(sql, dbmg);
            isValid(sql);
        }

        virtual ~SelectSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // SelectSyntactic_HPP
