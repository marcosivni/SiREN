#include "SirenShell.h"

SirenShell::SirenShell(DatabaseManager *db, QHostAddress ip, int port){

    console.printLogo();
    dataFormat = "yyyy-MM-dd HH:mm";

    this->db = db;
    tcpServer = nullptr;
    this->ip = ip;
    this->port = port;
}

SirenShell::~SirenShell(){

    destroyTcpServer();
    destroyDBConnection();
}

void SirenShell::destroyTcpServer(){

    if (tcpServer != nullptr)
        delete (tcpServer);
    tcpServer = nullptr;

    for (QTcpSocket* socket : qAsConst(socketList)){
        if (socket != nullptr)
            delete (socket);
    }
    socketList.clear();
}

void SirenShell::destroyDBConnection(){

    if (db != nullptr)
        delete (db);
    db = nullptr;
}


void SirenShell::startTcpServer(){

    bool answer = false;

    destroyTcpServer();
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    answer = tcpServer->listen(ip, port);

    if (answer){
        console.write(QDateTime::currentDateTime().toString(dataFormat).toStdString() + " : TCP server started...");
    } else {
        console.write(QDateTime::currentDateTime().toString(dataFormat).toStdString() + " : TCP server failed to start. ");
    }
}

void SirenShell::run(){

    std::string input;

    while (true){
        input.clear();
        input = console.readStatement();

        Parser *parser = new Parser(input);

        if ((parser->countTokens() == 1) && (parser->getToken(0)->toUpperLexem() == "CLEAR")){
            console.clear();
        } else {
            if ((parser->countTokens() == 1) && (parser->getToken(0)->toUpperLexem() == "QUIT")){
                delete (parser);
                parser = nullptr;
                destroyDBConnection();
                console.write(QDateTime::currentDateTime().toString(dataFormat).toStdString() + " : DB connection closed!");
                destroyTcpServer();
                console.write(QDateTime::currentDateTime().toString(dataFormat).toStdString() + " : TCP server connection closed!");
                break;
            } else {
                console.write("Invalid command (CLEAR | SAVE | QUIT). Only SIREN clients can issue queries.");
            }
        }
        if (parser != nullptr){
            delete (parser);
        }
    }
}


void SirenShell::onNewConnection(){

    //Grab the socket
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    socketList.push_back(clientSocket);
    console.write(QDateTime::currentDateTime().toString(dataFormat).toStdString()
                  + " : IP: " + clientSocket->peerAddress().toString().toStdString()
                  + " PORT: " + QString::number(clientSocket->peerPort()).toStdString() + " connected to server!");
    clientSocket->write("Siren::SQL> ");
}


void SirenShell::onSocketStateChanged(QAbstractSocket::SocketState socketState){

    if (socketState == QAbstractSocket::UnconnectedState){
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        socketList.removeOne(sender);
        sender->close();
    }
}

void SirenShell::onReadyRead(){

    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray datas = sender->readAll();


    console.write( QDateTime::currentDateTime().toString(dataFormat).toStdString()
                   + " : IP: " + sender->peerAddress().toString().toStdString()
                   + " PORT: " + QString::number(sender->peerPort()).toStdString()
                   + " : " + datas.toStdString());

    //Ignore [Carriage ENTER]
    if (datas != "\r\n"){

        //Convert input to QString and remove \n, etc
        QString query = datas;
        query = query.simplified();
        //Remove ';'
        if (query.at(query.size() -1) == ";"){
            query.chop(1);
        }

        //Close connection by user-request
        if (query.toUpper() == "QUIT"){
            socketList.removeOne(sender);
            sender->close();
            return;
        }

	//Ignore \n commands
	if (!query.size()){
	     sender->write("\n");
 	    return;
	}

        //Extended SQL processing
        try{
            SirenSQLQuery sirenQuery(db);
            sirenQuery.prepare(query.toStdString());

            if (sirenQuery.hasErrors()){
                sender->write("Command failed.\n");
                sender->write(logResultErrors(fromStringVector(sirenQuery.errors())).toStdString().c_str());
            } else {
                QStringList sqlStatements = fromStringVector(sirenQuery.translation());
                for (int x = 0; x < sqlStatements.size(); x++){
                    QMap<QString, QStringList> *resultSet = db->runSelect(sqlStatements.at(x));
                    console.write(sqlStatements[x].toStdString());
                    QString pp = logResultSet(*resultSet);
                    sender->write(pp.toLocal8Bit(), pp.size());
                    //Sync sending...
                    sender->waitForBytesWritten(-1);
                    //Memory cleaning
                    resultSet->clear();
                    if (resultSet != nullptr)
                        delete (resultSet);

                }
            }
        } catch (...){
            console.write("Fatal error executing the query.\n");
            sender->write("Fatal error executing the query.\n");
        }
    }
    //Echo-like string, remove if you want to
    sender->write("Siren::SQL> ");
}

QStringList SirenShell::fromStringVector(std::vector<std::string> input){

    QStringList answer;

    for (size_t x = 0; x < input.size(); x++){
        answer.append(input[x].c_str());
    }

    return answer;
}

QString SirenShell::logResultErrors(QStringList errors){

    QString aux;
    QString answer;

    console.write("==== BEGIN ERROR QUERY ====");

    for (int x = 0; x < errors.size(); x++){
        aux = errors.at(x);
        answer += aux;
        answer += "\n";
        console.write(aux.toStdString());
    }
    console.write("==== END ERROR QUERY ====");

    return answer;
}

QString SirenShell::logResultSet(QMap<QString, QStringList> resultSet){

    QString aux;
    QString answer;

    console.write("==== BEGIN RESULT SET ====");
    if (!resultSet.empty()){
        aux.clear();
        for (int i = 0; i < db->fetchAttributesNames().size(); i++){
            aux += db->fetchAttributesNames().at(i);
            if (i < db->fetchAttributesNames().size() - 1){
                aux += ", ";
            }
        }
        console.write(aux.toStdString());
        answer += aux + "\n";

        aux.clear();
        int cardinality = resultSet.constBegin().value().size();

        for (int i = 0; i < cardinality; i++){
            aux.clear();
            for (auto iter = resultSet.constBegin(); iter != resultSet.constEnd(); ++iter) {
                aux += iter.value().at(i);
                if (iter != resultSet.constEnd() - 1)
                    aux += ", ";
            }
            console.write(aux.toStdString());
            answer += aux + "\n";
        }
    }
    console.write("==== END RESULT SET ====");

    return answer;
}
