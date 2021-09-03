#ifndef LEXICAL_HPP
#define LEXICAL_HPP

#include <Token.h>
#include <StateMachine.h>

class Lexical{

    private:
        std::string input;
        int length;
        int position;
        StateMachine *state;

    public:
        // Defines lexem types
        const static int LK_UNDEFINED = -2;
        const static int LK_COLUMN_NAME = 200;
        const static int LK_TABLE_NAME = 201;
        const static int LK_TABLE_NAME_REF = 202;
        const static int LK_STILLIMAGE_DATATYPE = 203;
        const static int LK_PARTICULATE_DATATYPE = 204;
        const static int LK_AUDIO_DATATYPE = 205;
        const static int LK_DELETE_CLAUSE = 206;
        const static int LK_SUM_SG = 207;
        const static int LK_AVG_SG = 208;
        const static int LK_ALL_SG = 209;
        const static int LK_EVERY_SG = 210;
        const static int LK_ANY_SG = 211;
        const static int LK_MAX_SG = 212;
        const static int LK_TIMESERIES_DATATYPE = 213;

    private:
        std::string getChar();

        void resolveExclamationMark(std::string ch, Token *token);
        void resolveGreaterSymbol(std::string ch, Token *token);
        void resolveIdentifier(std::string ch, Token *token);
        void resolveLessSymbol(std::string ch, Token *token);
        void resolveNumber(std::string ch, Token *token);
        void resolveNumberExponential(std::string ch, Token *token);
        void resolveNumberIntegerPart(std::string ch, Token *token);
        void resolveNumberSignal(std::string ch, Token *token);
        void resolveQuotes(std::string quote, Token *token);
        void resolveSymbols(std::string ch, Token *token);
        void ungetChar();

    public:
        Lexical();
        Lexical(std::string input);
        ~Lexical();

        std::string toUpper(std::string input);
        void setLength(int length);
        void setPosition(int position);
        void setInput(std::string input);
        void setState(int state);

        int getLength();
        int getPosition();
        std::string getInput();
        int getState();

        Token* getToken();
};
#endif // LEXICAL_HPP
