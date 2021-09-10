#ifndef CONSOLEMANAGER_HPP
#define CONSOLEMANAGER_HPP

#ifdef __unix__
#include <termios.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <conio.h>
#endif

#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <DatabaseManager.h>

class ConsoleManager{

    private:
        std::string statement;
        std::string last;
        std::string name;

    private:
        void printLogoLinux();
        void printLogoWindows();

        std::string backspace(std::string query);

    public:
        ConsoleManager();
        ~ConsoleManager();

        static int upDownArrowOrRegularChar();
        static int consoleManagerChar();

        void printLogo();
        void printResultQuery(DatabaseManager *db, std::string query, bool see = false);
        void printResultQuery(DatabaseManager *db, std::vector<std::string> query, bool see = false);
        void printError(std::vector<std::string> errors);

        void clear();
        void close();
        void commit(DatabaseManager *db);
        std::string write(std::string in);

        std::string readStatement();

        std::string currentStatement();
        std::string lastInputStatement();
};

#endif // CONSOLEMANAGER_HPP
