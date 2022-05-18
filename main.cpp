#include <QCoreApplication>

#include <SirenShell.h>
#include <ConsoleManager.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString hostname, username, password, dbms = "SIREN";
    int port = -1;
    std::string pass;
    DatabaseManager *db;

    for (int x = 1; x < argc; x++){
        QString aux = argv[x];
        if ((aux.toUpper() == "-H") && (x+1 < argc)){
            hostname = argv[x+1];
            x++;
        } else {
            if ((aux.toUpper() == "-U") && (x+1 < argc)){
                username = argv[x+1];
                x++;
            } else {
                if ((aux.toUpper() == "-P") && (x+1 < argc)){
                    port = QString(argv[x+1]).toInt();
                    x++;
                } else {
                    if ((aux.toUpper() == "-D") && (x+1 < argc)){
                        dbms = QString(argv[x+1]);
                        x++;
                    } else{
                        std::cout << "Invalid SIREN server setup. Exiting... \n" << std::endl;
                        return 0;
                    }
                }
            }
        }
    }


    if (hostname.isEmpty()){
        std::string aux;
        std::cout << "Hostname: ";
        std::cin >> aux;
        hostname.fromStdString(aux);
    }

    if (username.isEmpty()){
        std::string aux;
        std::cout << "User: ";
        std::cin >> aux;
        username.fromStdString(aux);
    }

    if (port == -1){
        std::string aux;
        std::cout << "Port: ";
        std::cin >> aux;
        port = QString::fromStdString(aux).toInt();
    }

    std::cout << "Password: ";
    char ch = (char) ConsoleManager::upDownArrowOrRegularChar();
    while (ch != '\n'){
        if (!((ch == 8) || (ch == 127))){
            pass += ch;
            std::cout << "*";
        } else {
            if (pass.size() > 0){
                std::cout << '\b';
                std::cout << " ";
                std::cout << '\b';
                std::string aux;
                for (size_t x = 0; x < pass.size()-1; x++){
                    aux += pass[x];
                }
                pass.clear();
                pass = aux;
            }
        }
        ch = (char) ConsoleManager::upDownArrowOrRegularChar();
    }
    std::cout << std::endl;
    password.fromStdString(pass);

    try {
        if (dbms.toUpper() == "MYSQL"){
            db = new DatabaseManager(hostname, username, password, 3306, DatabaseManager::MYSQL);
        } else {
            db = new DatabaseManager(hostname, username, password);
        }
    } catch (std::invalid_argument *e){
        std::cout << "Fatal error: " << e->what();
        delete (e);
        return 0;
    }

    if (db->openConnection()){
        std::cout << "DB connection opened." << std::endl;
    } else {
        std::cout << "Can't establish the DB connection. " << std::endl;
        return 0;
    }

    QHostAddress address;
    address.setAddress(hostname);

    SirenShell shell(db, address, port);
    shell.startTcpServer();

    return a.exec();
}
