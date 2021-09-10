#include "Token.h"

Token::Token() {

    lexem = "";
    tokenType = TK_UNDEFINED;
    lexemType = -2;
    scopePosition = -1;
}

Token::Token(std::string lexem, int tokenType, int lexemType) {

    this->lexem = lexem;
    this->tokenType = tokenType;
    this->lexemType = lexemType;
    this->scopePosition = -1;
}

Token::Token(std::string lexem, int tokenType, int lexemType, int scopePosition) {

    this->lexem = lexem;
    this->tokenType = tokenType;
    this->lexemType = lexemType;
    this->scopePosition = scopePosition;
}

Token::~Token(){
}

std::string Token::getLexem() {

    return lexem;
}

void Token::setLexem(std::string lexem) {

    this->lexem = lexem;
}

int Token::getLexemType() {

    return lexemType;
}

void Token::setLexemType(int lexemType) {

    this->lexemType = lexemType;
}

int Token::getScopePosition() {

    return scopePosition;
}

void Token::setScopePosition(int scopePosition) {

    this->scopePosition = scopePosition;
}

int Token::getTokenType() {

    return tokenType;
}

void Token::setTokenType(int tokenType) {

    this->tokenType = tokenType;
}

std::string Token::toUpperLexem(){

    std::string aux = getLexem();
    char empty = ' ';
    std::string aux2;

    std::transform(aux.begin(), aux.end(), aux.begin(), ::toupper);

    if ((aux.size() != getLexem().size()) || (aux[aux.size()-1] == empty)){
        aux2 = aux;
        aux = "";
        for (size_t x = 0; x < aux2.size()-1; x++)
            aux += aux2[x];
    }

    return aux;
}

void Token::appendToLexem(std::string str){

	lexem = lexem.append(str);
}
