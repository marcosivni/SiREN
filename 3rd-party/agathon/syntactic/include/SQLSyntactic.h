#ifndef SQLSyntactic_HPP
#define SQLSyntactic_HPP


#include <Parser.h>
#include <DataDictionary.h>
#include <QStringList>

class SQLSyntactic{

    private:
        DatabaseManager *dbmg;
        DataDictionary *dic;
        std::vector<std::string> errors;
        Parser *p;

    protected:
        DatabaseManager *databaseManager();

    public:
        SQLSyntactic(DatabaseManager *dbmg);
        SQLSyntactic(std::string expression, DatabaseManager *dbmg);
        virtual ~SQLSyntactic();

        void clearErrors();
        void setDataBaseManager(DatabaseManager *dbmg);
        void setSQLStatement(std::string sqlExpression);
        void addError(std::string error);
        std::string getSQLStatement();
        Parser *getParser();
        Token currentToken();
        bool endOfStatement();
        bool hasErrors();
        bool isValid();

        std::string getError(int pos);
        std::vector<std::string> getErrors();
        size_t countErrors();

        //Qt wrappers
        QStringList fetchErrors();
        DataDictionary *dictionary();
        DatabaseManager *dbManager();
};

#endif // SQLSyntactic_HPP
