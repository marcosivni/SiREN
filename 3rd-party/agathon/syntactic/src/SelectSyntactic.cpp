#include "SelectSyntactic.h"

SelectSyntactic::~SelectSyntactic(){

    destroyAuxiliaryStructures();
}

void SelectSyntactic::destroyAuxiliaryStructures(){

    if (translator != nullptr){
        delete (translator);
    }
}

//Go-horse mode
bool SelectSyntactic::isValid(std::string sqlStatement){

    std::vector<std::string> resultSet;
    bool answer = true;
    destroyAuxiliaryStructures();
    translator = new SelectExecutor(dbManager());
    try {
        resultSet = translator->translate(sqlStatement);
        for (size_t x = 0; x < resultSet.size() && answer; x++){
            if (QString(resultSet[x].c_str()).contains("NEAR", Qt::CaseInsensitive)){
                answer = false;
            } else {
                answer &= translator->dbManager()->isValidSelect(resultSet[x].c_str());
            }
            if (!answer){
                addError(translator->dbManager()->error().toStdString());
            }
        }
    } catch (std::runtime_error *ex) {
        addError(ex->what());
        delete (ex);
    } catch (...) {
        addError("Error parsing the query.");
    }

    return answer;
}
