#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>


class Token{

    public:
        static const int TK_LEX_ERROR = -10;
        static const int TK_UNTERMINATED_STRING = -11;
        static const int TK_FLOATINGPOINT_ERROR = -12;

        static const int TK_UNDEFINED = -1;
        static const int TK_IDENTIFIER = 100;
        static const int TK_INTEGER = 101;
        static const int TK_FLOATING_POINT = 102;
        static const int TK_STRING = 103;
        static const int TK_GQ = 110;             // >
        static const int TK_GE = 111;             // >=
        static const int TK_LQ = 112;             // <
        static const int TK_NE = 113;             // !=
        static const int TK_N2 = 114;             // <>
        static const int TK_LE = 115;             // <=
        static const int TK_OPEN_BRACE = 120;     // (
        static const int TK_CLOSE_BRACE = 121;    // )
        static const int TK_OPEN_BLOCK = 122;     // {
        static const int TK_CLOSE_BLOCK = 123;    // }
        static const int TK_COMMA = 124;          // ,
        static const int TK_SEMICOLON = 125;      // ;
        static const int TK_PERIOD = 126;         // .
        static const int TK_EQUAL = 127;          // =
        static const int TK_SUM = 128;            // +
        static const int TK_SUB = 129;            // -
        static const int TK_MUL = 130;            // *
        static const int TK_DIV = 131;            // /
        static const int TK_PER = 132;            // %
        static const int TK_IN = 133;             //IN (...)
        static const int TK_BETWEEN = 134;        //BETWEEN ... AND ...

    private:
        std::string lexem;
        int tokenType;
        int lexemType;
        int scopePosition;

    public:
        Token();
        Token(std::string lexem, int tokenType, int lexemType);
        Token(std::string lexem, int tokenType, int lexemType, int scopePosition);
        ~Token();
        std::string getLexem();

        void setLexem(std::string lexem);
        void setLexemType(int lexemType);
        void setScopePosition(int scopePosition);
        void setTokenType(int tokenType);

        int getLexemType();
        int getScopePosition();
        int getTokenType();

        std::string toUpperLexem();
        void appendToLexem(std::string str);
};

#endif // TOKEN_HPP
