#include "Parser.h"

Parser::Parser(){

    list.clear();
    setPosition(0);
    setActiveScope(0);
}

Parser::Parser(std::string input){

    list.clear();
    parse(input);
    setPosition(0);
    setActiveScope(0);
}

Parser::~Parser(){

    destroyTokenList();
}

void Parser::destroyTokenList(){

    for (size_t i = 0; i < list.size(); i++){
        if (list[i] != nullptr)
            delete(list[i]);
    }
    list.clear();
    original.clear();
}

void Parser::setPosition(int position){

    this->position = position;
}

int Parser::getPosition(){

    if ((position < list.size()) && (position >= 0))
        return position;
    else
        return -1;
}

void Parser::setActiveScope(int activeScope){

    this->activeScope =  activeScope;
}

int Parser::getActiveScope(){

    return activeScope;
}

std::string Parser::getOriginal(){

    return original;
}

std::string Parser::getCurrent(){

    std::string answer;
    for (size_t x = 0; x < countTokens(); x++){
        if (x > 0){
            answer += " ";
        }
        answer += getToken(x)->getLexem();
    }
    return answer;
}

void Parser::parse(std::string input){

    destroyTokenList();

    Lexical *l = new Lexical(input);
    Token *tok = l->getToken();

    while (tok != nullptr){
        list.push_back(tok);
        tok = l->getToken();
    }
    for (size_t x = 0; x < list.size(); x++){
        list[x]->setScopePosition(0);
    }
    original.clear();
    original = input;

    if (l != nullptr)
        delete (l);
}

void Parser::resetParser(){

    for (size_t i = 0; i < list.size(); i++){
        delete(list[i]);
    }
    list.clear();

    parse(getOriginal());
    setPosition(0);
}

bool Parser::match(std::string identifier, int tokenType) throw (std::exception *){

    if (getPosition() == -1)
        throw new std::runtime_error("End of token list.");

    if ((getToken(getPosition())->getLexem() == identifier) && (getToken(getPosition())->getTokenType() == tokenType)) {
        setPosition(getPosition()+1);
        return true;
    } else {
        throw new std::runtime_error("Token " + getToken(getPosition())->getLexem() + " unmatched.");
        return false;
    }
}

bool Parser::match(Token* token) throw (std::exception *){

    if (getPosition() == -1)
        throw new std::runtime_error("End of token list.");

    if ((getToken(getPosition())->getLexem() == token->getLexem()) && (getToken(getPosition())->getTokenType() == token->getTokenType())) {
        setPosition(getPosition()+1);
        return true;
    } else {
        throw new std::runtime_error("Token " + getToken(getPosition())->getLexem() + " unmatched.");
        return false;
    }
}

void Parser::insertToken(Token *token, int position) throw (std::exception *){

    if (list.size() > position){

        std::vector<Token*> result;

        for (int x = 0; x < position; x++){
            result.push_back(list[x]);
        }

        result.push_back(token);

        for (int x = position; x < list.size(); x++){
            result.push_back(list[x]);
        }

        list = result;
    } else {
        if (list.size() == position){
            list.push_back(token);
        } else {
            throw new std::exception();
        }
    }
}

void Parser::removeToken(int position) throw (std::exception *){

    if (list.size() > position){

        std::vector<Token*> result;

        for (int x = 0; x < position; x++){
            result.push_back(list[x]);
        }

        delete (list[position]);

        for (int x = position+1; x < list.size(); x++){
            result.push_back(list[x]);
        }

        list = result;

        if (position > list.size()){
            position = -1;
        }

    } else {
        throw new std::exception();
    }
}

bool Parser::updateToken(Token *token, int position){

    if (position < countTokens()){
        *getTokenList()[position] = *token;
        return true;
    }

    return false;
}


void Parser::insertTokenList(std::vector<Token *> newList, int position) throw (std::exception *){

    if (position <= list.size()){
        for (int x = newList.size()-1; x >= 0; x--){
            insertToken(newList[x], position);
        }
    } else {
        throw new std::exception();
    }
}


void Parser::defineRightScopes(){

    int qtdBraces = 0;
    for (int x = 0; x < list.size(); x++){
        if (list[x]->getLexem() == ")")
            qtdBraces--;
        list[x]->setScopePosition(qtdBraces);
        if (list[x]->getLexem() == "(")
            qtdBraces++;
    }
}

size_t Parser::countTokens(){

    return list.size();
}

Token* Parser::getCurrentToken() throw (std::exception *){

    if (getPosition() == -1)
        throw new std::exception();

    return getToken(getPosition());
}

Token* Parser::getToken() throw (std::exception *){

    if (position < list.size()-1){
        setPosition(getPosition()+1);
        return list[position];
    } else {
        throw new std::exception();
    }
}

Token* Parser::getToken(int position) throw (std::exception *){

    if (position < list.size()){
        return list[position];
    } else {
        throw new std::exception();
    }
}

std::vector<Token*> Parser::getTokenList(){

    return list;
}

std::vector<Token> Parser::getTokenList(int scope){

    std::vector<Token> result;

    for (int x = 0; x < countTokens(); x++){
        if (getToken(x)->getScopePosition() >= scope){
            result.push_back(*getToken(x));
        }
    }

    return result;
}

int Parser::getCurrentPosition(){

    return getPosition();
}

int Parser::getNumberOfScopes(){

    int answer = 0;

    for (int x = 0; x < countTokens(); x++){
        if (getToken(x)->getScopePosition() > answer){
            answer = getToken(x)->getScopePosition();
        }
    }

    return answer;
}

void Parser::print(){

    for (size_t x = 0; x < countTokens(); x++){
        if (x > 0){
            std::cout << " ";
        }
        std::cout << getToken(x)->getLexem();
    }
    std::cout << std::endl;
}
