#ifndef SQLExecutor_HPP
#define SQLExecutor_HPP

#include <DataDictionary.h>
#include <Parser.h>

class SQLExecutor {

    private:
        DatabaseManager *dbmg;
        DataDictionary *dic;
        Parser *p;

    private:
        void destroyAuxiliaryStructures();

    public:
        SQLExecutor(DatabaseManager *dbmg);
        SQLExecutor(std::string sirenSqlStatement, DatabaseManager *dbmg);
        virtual ~SQLExecutor();

        void setDataBaseManager(DatabaseManager *dbmg);

        Parser *getParser();
        DataDictionary *dictionary();
        DatabaseManager *dbManager();
        Token currentToken();

        std::string sirenSqlStatement();
        bool endOfStatement();
};

#endif // SQLExecutor_HPP
