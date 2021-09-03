#include "ConsoleManager.h"

ConsoleManager::ConsoleManager(){

    last.clear();
    currentStatement().clear();
    name = "Siren::SQL> ";
 }

ConsoleManager::~ConsoleManager(){

    statement.clear();
    last.clear();
    name.clear();
}

void ConsoleManager::printLogoLinux(){

    std::cout << std::endl;
    std::cout << " SIREN (R) Command Line Shell - January 2014 - Databases and Image Group" << std::endl;
    std::cout << "                     University of Sao Paulo - USP " << std::endl;
    std::cout << "                 See manual.txt for more information." << std::endl << std::endl << std::endl;
}

void ConsoleManager::printLogoWindows(){

    std::cout << std::endl;
    std::cout << " SIREN (R) Command Line Shell - January 2014 - Databases and Image Group" << std::endl;
    std::cout << "                     University of Sao Paulo - USP " << std::endl;
    std::cout << "                 See manual.txt for more information." << std::endl << std::endl << std::endl;
}

std::string ConsoleManager::backspace(std::string query){

    if (query.size() > 0){
        std::string aux;
        for (size_t x = 0; x < query.size()-1; x++){
            aux += query[x];
        }

        query.clear();
        query = aux;
        std::cout << '\b';
        std::cout << " ";
        std::cout << '\b';
        std::cout << '\r';
        std::cout << name << query;
        aux.clear();
    }

    return query;
}

int ConsoleManager::consoleManagerChar(){

    int ch;
    #ifdef __unix__
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #else
        ch = getch();
    #endif

        return ch;
}

int ConsoleManager::upDownArrowOrRegularChar(){

    int answer;

    int k = 0;
    int ch = consoleManagerChar();

    if (ch == 27){
        k = 1;
        ch = consoleManagerChar();
        if ((ch == 91) && (k == 1)){
            k = 2;
            ch = consoleManagerChar();
            if((ch == 65) && (k == 2)){
                answer = -500;
            } else {
                if((ch == 66) && (k == 2)){
                    answer = -501;
                } else {
                    if((ch == 68) && (k == 2)){
                        answer = -502;//left
                    } else {
                        if((ch == 67) && (k == 2)){
                            answer = -503;//right
                        } else {
                            answer = ch;
                        }
                    }
                }
            }
        } else {
            answer = ch;
        }
    } else {
        answer = ch;
    }

    return answer;
}

void ConsoleManager::printLogo(){

    #ifdef __unix__
        printLogoLinux();
    #else
        printLogoWindows();
    #endif
}

void ConsoleManager::clear(){

     #ifdef __unix__
        system("clear");
     #else
        system("cls");
     #endif
}

void ConsoleManager::close(){

    std::cout << "Bye!" << std::endl;
    exit(0);
}

void ConsoleManager::commit(DatabaseManager *db){

    db->runDDL("commit");
}

std::string ConsoleManager::write(std::string in){

    std::cout << in << std::endl;

    return in;
}

std::string ConsoleManager::readStatement(){

    std::string query;

    statement.clear();
    query.clear();

    std::cout << name;

    int ch = upDownArrowOrRegularChar();

    while (!((((char) ch) == '\n') && (query.size() > 0) && (query[query.size()-1] == ';'))){

        if (ch == -500){
            if (query.size() > 0){
                for (size_t x = 0; x < query.size(); x++){
                    for (size_t x = 0; x < query.size(); x++){
                        std::cout << '\b';
                        std::cout << " ";
                        std::cout << '\b';
                    }
                 }
            }
            query.clear();
            std::cout << '\r';
            query = last;
            std::cout << name << query;
        } else {
            if (ch == -501){
                if (query.size() > 0){
                    for (size_t x = 0; x < query.size(); x++){
                        std::cout << '\b';
                        std::cout << " ";
                        std::cout << '\b';
                    }
                }
                std::cout << '\r';
                query.clear();
                std::cout << name << query;
            } else {
                if ((ch == 8) || (ch == 127)){
                    query = backspace(query);
                } else {
                    if (ch == 10){
                        std::cout << (char) ch;
                        std::cout << std::setw(name.size()) << "-> ";
                    } else {
                        query += ch;
                        std::cout << (char) ch;
                    }
                }
            }
        }
        ch = upDownArrowOrRegularChar();
    }

    for (size_t x = 0; x < query.size()-1; x++){
        if (query[x] == '\n'){
            statement += ' ';
        } else {
            statement += query[x];
        }
    }

    if (statement.size() > 0){
        last.clear();
        last = statement;
    }

    query.clear();

    std::cout << std::endl;

    return statement;
}

std::string ConsoleManager::currentStatement(){

    return statement;
}

std::string ConsoleManager::lastInputStatement(){

    return last;
}

void ConsoleManager::printResultQuery(DatabaseManager *db, std::vector<std::string> query, bool see){


    bool all = true;

    if (see){
        std::cout << "\033[1;36m" << "\nRegular SQL equivalent expressions:\033[0m \n" << std::endl;
    }
    for (size_t x = 0; x < query.size(); x++){

        if (!db->runInsert(query[x].c_str())){
            all = false;
            std::cout << "\033[1;31m" << "Operation cannot be performed: \033[0m" << std::endl;
            std::cout << "\033[0;31m" << query[x] << "\033[0m \n" << std::endl;
        }
        else if (see) {
            std::cout << query[x] << "\n" << std::endl;
        }
    }
}

void ConsoleManager::printResultQuery(DatabaseManager *db, std::string query, bool see){


    QMap<QString, QStringList>* answer = db->runSelect(query.c_str());

    if (see){
        std::cout << "\033[1;36m" << "\nRegular SQL equivalent expressions:\033[0m \n" << std::endl;
        std::cout << query << "\n" << std::endl;
    }

    if (answer->constBegin()->size() > 0){
        QStringList columns = db->fetchAttributesNames();
        std::vector<int> maxSizes;
        std::string bar;

        for (int x = 0; x < columns.size(); x++){
            maxSizes.push_back(columns.at(x).size());
        }

        size_t x = 0;
        for (auto iter = answer->constBegin(); iter != answer->constEnd() && x < answer->constBegin()->size(); ++iter) {
            for (int y = 0; y < iter.value().size(); y++){
                if (iter->at(y).size() > maxSizes[x]){
                    maxSizes[x] = iter->at(y).size();
                }
            }
            x++;
        }

        for (x = 0; x < maxSizes.size(); x++){
            bar += "+-";
            for (int y = 0; y < maxSizes[x]; y++){
                bar += "-";
            }
            bar += "-";
        }
        bar += "+";

        std::cout << bar << std::endl;
        std::cout << "| ";
        for (x = 0; x < maxSizes.size(); x++){
            std::cout << std::setw(maxSizes[x]) << columns.at(x).toStdString() << " | ";
        }
        std::cout << std::endl;
        std::cout << bar << std::endl;


        for (int y = 0; y < answer->constBegin().value().size(); y++){
            std::cout << "| ";
            x = 0;
            for (auto iter = answer->constBegin(); iter != answer->constEnd(); ++iter) {
                std::cout << std::setw(maxSizes[x]) << iter.value().at(y).toStdString() << " | ";
                x++;
            }
            std::cout << std::endl;
        }
        std::cout << bar << std::endl;
    } else {
        std::cout << "Empty result set." << std::endl;
    }

    if (answer != nullptr){
        delete (answer);
    }
}

void ConsoleManager::printError(std::vector<std::string> errors){

    std::cout << errors.size() << " error(s) found: " << std::endl;
    for (size_t x = 0; x < errors.size(); x++){
        std::cout << errors[x] << std::endl;
    }
}

