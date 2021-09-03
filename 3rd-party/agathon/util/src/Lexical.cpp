#include "Lexical.h"

Lexical::Lexical(){

    //Just for sanity
    setInput("");
    setPosition(0);
    setLength(0);
    state = new StateMachine();
}


Lexical::Lexical(std::string input){

    this->input = input;
    length = this->input.length();
    position = 0;
    state = new StateMachine();
}

Lexical::~Lexical(){

    if (state != nullptr){
        delete (state);
    }
    input.clear();
}


std::string Lexical::getChar(){

    std::string ch = "";

    if (position < length){
        ch = input[position];
    } else {
        state->setCurrentState(StateMachine::ST_ERROR); // Reached the end of input std::string
    }
    position++;

    return ch;
}


Token* Lexical::getToken(){

    Token *token = new Token();
    state->setCurrentState(StateMachine::ST_BEGIN); // First, set state as BEGIN
    std::string ch = this->getChar(); // getChar() can change the state to ERROR!

    while ((!state->end()) && (!state->error())) {

        // Spaces: advance to the next character
        if(ch == " "){
            ch = this->getChar();
        }
        // Strings 'xyz' or "xyz": quotation marks
        else if(ch =="\'" || ch == "\""){
            this->resolveQuotes(ch, token);
        }
        // Identifier: [a-zA-Z][a-zA-z0-9$]*
        else if( (ch >= "A" && ch <= "Z") || (ch >= "a" && ch <= "z") || ch == "$"){
            this->resolveIdentifier(ch, token);
        }
        // Number: [-+]?[0-9]+("."[0-9]*)?([eE]"-"?[0-9]*)?
        else if( (ch >= "0" && ch <= "9") || ch == "+" || ch == "-"){
            this->resolveNumber(ch, token);
        }
        // Symbols ">" and ">="
        else if(ch == ">"){
            this->resolveGreaterSymbol(ch, token);
        }
        // Symbols "<", "<=" and "<>"
        else if(ch == "<"){
            this->resolveLessSymbol(ch, token);
        }
        // Symbol "!="
        else if(ch == "!"){
            this->resolveExclamationMark(ch, token);
        }
        // Symbols ( ) { } , ; . = * / %
        else if(ch == "(" || ch == ")" || ch == "{" || ch == "}" || ch == "," || ch == ";" || ch == "." || ch == "=" || ch == "*" || ch == "/" || ch == "%"){
            this->resolveSymbols(ch, token);
        } else {
            //Awkward symbol found
            state->setCurrentState(StateMachine::ST_ERROR);
            break;
        }
    }// End while

    if (state->error()){
        if (token != nullptr)
            delete (token);

        token = nullptr;
    }
    return token;
}

void Lexical::resolveExclamationMark(std::string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol !=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_NE);
        }
        else
        {
                this->ungetChar(); // Just the symbol !. Put back the character read.
                token->setTokenType(Token::TK_LEX_ERROR);
        }

        state->setCurrentState(StateMachine::ST_END);
}

void Lexical::resolveGreaterSymbol(std::string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol >=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_GE);
        }
        else
        {
                this->ungetChar(); // Just the symbol >. Put back the character read.
                token->setTokenType(Token::TK_GQ);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input std::string finished.
        // In both cases, this method recognizes the symbol and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

void Lexical::resolveIdentifier(std::string ch, Token *token){

        // Add letters to the lexeme
        while( (ch >= "A" && ch <= "Z") || (ch >= "a" && ch <= "z") || (ch >= "0" && ch <= "9") || ch == "_" || ch == "$")
        {
                token->appendToLexem(ch);
                ch = this->getChar();
        }

        this->ungetChar(); // Put back the character that broke the loop

        token->setTokenType(Token::TK_IDENTIFIER);

        //IN and BETWEEN are actually operators in
        //std::string format
        if (token->toUpperLexem() == "IN"){
            token->setTokenType(Token::TK_IN);
        }
        if (token->toUpperLexem() == "BETWEEN"){
            token->setTokenType(Token::TK_BETWEEN);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input std::string finished.
        // In both cases, this method recognizes the identifier and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

void Lexical::resolveLessSymbol(std::string ch, Token *token){

        token->setLexem(ch);
        ch = this->getChar();

        if(ch == "=") // Symbol <=
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_LE);
        }
        else if(ch == ">") // Symbol <>
        {
                token->appendToLexem(ch);
                token->setTokenType(Token::TK_N2);
        }
        else
        {
                this->ungetChar(); // Just the symbol <. Put back the character read.
                token->setTokenType(Token::TK_LQ);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input std::string finished.
        // In both cases, this method recognizes the symbol and finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);
}

void Lexical::resolveNumber(std::string ch, Token *token){

        // Recognize the signal: + or - or nothing
        this->resolveNumberSignal(ch, token);

        // Just continue if there are digits to process
        while(!state->end())
        {
                ch = this->getChar(); // Here, must be at least one digit

                // Get digit(s)
                this->resolveNumberIntegerPart(ch, token);

                // The next character can be "." (floating point), an "eE" (exponential) or anything (integer)
                ch = this->getChar();
                if(ch == ".") // Floating point number (for example, 12.345)
                {
                        // Add the "." to the lexeme
                        token->appendToLexem(ch);

                        // After the "." must be at least one digit
                        ch = this->getChar();
                        if(ch >= "0" && ch <= "9")
                        {
                                this->resolveNumberIntegerPart(ch, token);

                                // Checking exponential (Number in format 12.34e56)
                                ch = this->getChar();
                                if(ch == "e" || ch == "E")
                                {
                                        this->resolveNumberExponential(ch, token);
                                        state->setCurrentState(StateMachine::ST_END);
                                }
                                else
                                {
                                        this->ungetChar(); // Put back the character read.
                                        token->setTokenType(Token::TK_FLOATING_POINT);
                                        state->setCurrentState(StateMachine::ST_END);
                                }
                        }
                        else
                        {
                                token->setTokenType(Token::TK_FLOATINGPOINT_ERROR);
                                state->setCurrentState(StateMachine::ST_END);
                        }
                }
                else if(ch == "e" || ch == "E")
                {
                        this->resolveNumberExponential(ch, token);
                        state->setCurrentState(StateMachine::ST_END);
                }
                else // Integer
                {
                        this->ungetChar(); // Put back the character read.
                        token->setTokenType(Token::TK_INTEGER);
                        state->setCurrentState(StateMachine::ST_END); // Stop
                }
        }
}

void Lexical::resolveNumberExponential(std::string ch, Token *token){

        // Add the "eE" to the lexeme
        token->appendToLexem(ch);

        // Check the signal
        ch = this->getChar();
        this->resolveNumberSignal(ch, token);

        // Check if there are digits to process
        if(!state->end())
        {
                ch = this->getChar(); // Here, must be at least one digit

                // Get digit(s)
                this->resolveNumberIntegerPart(ch, token);
                token->setTokenType(Token::TK_FLOATING_POINT);
        }
        else // Error: just a signal after "eE".
        {
                token->setTokenType(Token::TK_FLOATINGPOINT_ERROR);
        }
}

void Lexical::resolveNumberIntegerPart(std::string ch, Token *token){
        // Add digits to the lexeme

        while(ch >= "0" && ch <= "9"){
                token->appendToLexem(ch);
                ch = this->getChar();
        }

        this->ungetChar(); // Put back the character that broke the loop
}

void Lexical::resolveNumberSignal(std::string ch, Token *token){

        if(ch == "+" || ch == "-"){

                if(ch == "+") token->setTokenType(Token::TK_SUM);
                else token->setTokenType(Token::TK_SUB);
                // Here, just append.
                // Do not set because exponential can be signalized too.
                token->appendToLexem(ch);


                // Checks if the signal is followed by a number.
                // If it is not, we have a + or - operator; otherwise, is a signed number.
                ch = this->getChar();

                // Read a character more to perform this checking.
                // Outside this IF, it will be put back.
                if(ch < "0" || ch > "9"){
                        // Finish. It is just a signal.
                        state->setCurrentState(StateMachine::ST_END);
                }
        }

        // If ch is a signal, inside the IF an extra character was read.
        // So, unget that character.
        // If ch is not a signal, put it back.
        this->ungetChar();
}

void Lexical::resolveQuotes(std::string quote, Token *token){

        std::string ch;
        token->setLexem(quote); // Initialize the token lexeme

        do{
                ch = this->getChar();
                token->appendToLexem(ch);
        } while(ch != quote && !state->error());

        // Set the token type
        if(!state->error()){
                token->setTokenType(Token::TK_STRING);
        } else {
                token->setTokenType(Token::TK_UNTERMINATED_STRING);
        }

        // If the previous state == BEGIN, there are more characters to process.
        // If the previous state == ERROR, the input std::string finished and std::string is unterminated.
        // In both cases, this method recognizes the std::string (or unterninated std::string) and
        // finishes in a success state.
        state->setCurrentState(StateMachine::ST_END);;
}

void Lexical::resolveSymbols(std::string ch, Token *token){

    token->setLexem(ch);

    char c = ch.at(0);

    switch(c){
        case '(': token->setTokenType(Token::TK_OPEN_BRACE); break;
        case ')': token->setTokenType(Token::TK_CLOSE_BRACE); break;
        case '{': token->setTokenType(Token::TK_OPEN_BLOCK); break;
        case '}': token->setTokenType(Token::TK_CLOSE_BLOCK); break;
        case ',': token->setTokenType(Token::TK_COMMA); break;
        case ';': token->setTokenType(Token::TK_SEMICOLON); break;
        case '.': token->setTokenType(Token::TK_PERIOD); break;
        case '=': token->setTokenType(Token::TK_EQUAL); break;
        case '*': token->setTokenType(Token::TK_MUL); break;
        case '/': token->setTokenType(Token::TK_DIV); break;
        case '%': token->setTokenType(Token::TK_PER); break;
    }

    state->setCurrentState(StateMachine::ST_END);
}

void Lexical::ungetChar(){

    // If state == ERROR, the end of the input std::string was reached.
    // So, it is not possible to unget.
    if(!state->error()){
        position--;
    }
}

std::string Lexical::toUpper(std::string input){

    std::string aux = input;
    char empty = ' ';
    std::string aux2;

    std::transform(aux.begin(), aux.end(), aux.begin(), ::toupper);

    if ((aux.size() != input.size()) || (aux[aux.size()-1] == empty)){
        aux2 = aux;
        aux = "";
        for (unsigned long x = 0; x < aux2.size()-1; x++)
            aux += aux2[x];
    }

    return aux;
}

void Lexical::setLength(int length) {

    this->length = length;
}

void Lexical::setPosition(int position) {

    this->position = position;
}

void Lexical::setInput(std::string input) {

    this->input = input;
}

void Lexical::setState(int state) {

    this->state->setCurrentState(state);
}

int Lexical::getLength() {

    return length;
}


int Lexical::getPosition() {

    return position;
}

std::string Lexical::getInput() {

    return input;
}

int Lexical::getState() {

    return state->getCurrentState();
}
