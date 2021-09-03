#ifndef PARSER_HPP
#define PARSER_HPP

#include <Lexical.h>

class Parser{

    private:
        std::vector<Token*> list;
        std::string original;
        int position;
        int activeScope;

    private:
        void setPosition(int position);
        int getPosition();
        void destroyTokenList();

    public:
        Parser();
        Parser(std::string input);
        ~Parser();

        void parse(std::string input);
        void resetParser();
        bool match(std::string identifier, int tokenType) throw (std::exception *);
        bool match(Token *token) throw (std::exception *);
        void insertToken(Token *token, int position) throw (std::exception *);
        void removeToken(int position) throw (std::exception *);
        bool updateToken(Token *token, int position);
        void insertTokenList(std::vector<Token *> newList, int position) throw (std::exception *);

        void defineRightScopes();

        size_t countTokens();
        void setActiveScope(int activeScope);

        int getActiveScope();
        std::string getOriginal();
        std::string getCurrent();
        Token* getCurrentToken() throw (std::exception *);
        Token* getToken() throw (std::exception *);
        Token* getToken(int position) throw (std::exception *);
        std::vector<Token*> getTokenList();
        std::vector<Token> getTokenList(int scope);
        int getCurrentPosition();
        int getNumberOfScopes();

        void print();

};

#endif // PARSER_HPP
