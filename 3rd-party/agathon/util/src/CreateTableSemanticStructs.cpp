#include "CreateTableSemanticStructs.h"

CheckValidColumnsAndTypes::CheckValidColumnsAndTypes(DataDictionary *dic){

    this->dic = dic;
}

CheckValidColumnsAndTypes::~CheckValidColumnsAndTypes(){


    for (size_t x = 0; x < listColumns.size(); x++){
        listColumns[x].first.clear();
        listColumns[x].second.clear();
    }
    listColumns.clear();
    error.clear();
}

void CheckValidColumnsAndTypes::setDataType(size_t pos, std::string dataType) throw (std::exception*){

    if (pos < getSize()){
        listColumns[pos].second = dataType;
    } else {
        throw (new std::exception());
    }
}

void CheckValidColumnsAndTypes::addNewColumn(std::string columnName, std::string dataType){

    std::pair<std::string, std::string> aux(columnName, dataType);
    listColumns.push_back(aux);
}

void CheckValidColumnsAndTypes::addNewColumn(std::string columnName){

    std::pair<std::string, std::string> aux(columnName, "");
    listColumns.push_back(aux);
}

void CheckValidColumnsAndTypes::addNewDataType(std::string dataType){

    if (listColumns.size() > 0){
        listColumns[listColumns.size()-1].second = dataType;
    }
}

void CheckValidColumnsAndTypes::addError(std::string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }

    this->error += error;
}

bool CheckValidColumnsAndTypes::isComplexColumn(std::string columnName){

    for (size_t x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName){
            return dic->isValidComplexDataType(listColumns[x].second.c_str());
        }
    }
    return false;
}

int CheckValidColumnsAndTypes::countColumns(std::string columnName){

    int count = 0;
    for (size_t x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName)
            count++;
    }

    return count;
}

bool CheckValidColumnsAndTypes::isValid(){

    if (!isUniqueColumnNames())
        return false;

    for (size_t x = 0; x < listColumns.size(); x++){
        if (!dic->isValidDataType(listColumns[x].second.c_str())){
            addError("Invalid data-type: '" + listColumns[x].second + "'.");
            return false;
        }
    }

    return true;
}

bool CheckValidColumnsAndTypes::isUniqueColumnNames(){

    for (size_t x = 0; x < listColumns.size(); x++){
        if (countColumns(listColumns[x].first) != 1){
            addError("Duplicate column name: '" + listColumns[x].first + "'.");
            return false;
        }
    }

    return true;
}

std::string CheckValidColumnsAndTypes::dataType(std::string columnName){

    std::string answer = "NULL";
    for (size_t x = 0; x < listColumns.size(); x++){
        if (listColumns[x].first == columnName)
            return listColumns[x].second;
    }

    return answer;
}

std::string CheckValidColumnsAndTypes::simplifiedDataType(size_t pos){

    std::string aux, answer;
    aux = listColumns[pos].second;

    for (size_t x = 0; x < aux.size(); x++){
        if ((aux[x] != '(' && aux[x] != ' '))
            answer += aux[x];
        else
            break;
    }

    return answer;
}

std::string CheckValidColumnsAndTypes::dimensionalityOfDataType(size_t pos){

    std::string aux, answer;
    aux = listColumns[pos].second;

    for (size_t x = 0; x < aux.size(); x++){
        if (aux[x] == '('){
            size_t y = x + 1;
            while ((y < aux.size() && aux[y] != ')')){
                if (aux[y] != ' ')
                    answer += aux[y];
                y++;
            }
            break;
        }
    }

    return answer;
}

size_t CheckValidColumnsAndTypes::getSize(){

    return listColumns.size();
}

std::pair<std::string, std::string> CheckValidColumnsAndTypes::getColumnNameAndDataType(size_t pos) throw (std::exception*){

    if (pos < getSize()){
        return listColumns[pos];
    } else {
        throw new std::exception();
    }
}

std::string CheckValidColumnsAndTypes::getError(){

    return error;
}

std::string CheckValidColumnsAndTypes::toString(){

    std::string answer =  "COLUMN || DATA TYPE \n";
    for (size_t x = 0; x < listColumns.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += listColumns[x].first + " || " + listColumns[x].second;
    }

    return answer;
}

Constraint::Constraint(){

    setType(-1);
}

Constraint::~Constraint(){

    clear();
}

void Constraint::clear(){

    setType(-1);
    name.clear();
    attributes.clear();
    referenceTableName.clear();
    usingMetricName.clear();
    referenceAttributes.clear();
}

void Constraint::setName(std::string name){

    this->name = name;
}

void Constraint::setType(unsigned char type){

    this->type = type;
}

void Constraint::addAttribute(std::string attribute){

    attributes.push_back(attribute);
}

void Constraint::setReferenceTableName(std::string referenceTableName){

    this->referenceTableName = referenceTableName;
}

void Constraint::addMetricName(std::pair<std::string, bool> usingMetricName){

    this->usingMetricName.push_back(usingMetricName);
}

void Constraint::addReferenceAttribute(std::string referenceAttribute){

    referenceAttributes.push_back(referenceAttribute);
}

bool Constraint::isValid(){

    if (!((getType() == Constraint::FOREIGN_KEY) ||
          (getType() == Constraint::PRIMARY_KEY) ||
          (getType() == Constraint::METRIC) ))
        return false;

    if (getType() == Constraint::PRIMARY_KEY){
        if ((!getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() > 0) ||
                (getMetricsNames().size() > 0))
            return false;
    }

    if (getType() == Constraint::FOREIGN_KEY){
        if ((getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() == 0) ||
                (getMetricsNames().size() > 0))
            return false;
    }

    if (getType() == Constraint::METRIC){
        if ((!getReferenceTableName().empty()) ||
                (getReferenceAttributes().size() < 0) ||
                (getMetricsNames().size() == 0))
            return false;
    }
    return false;
}

bool Constraint::isEqual(Constraint constraint){

    bool answer = true;

    if ((name == constraint.getName()) &&
          (type == constraint.getType()) &&
          (referenceTableName == constraint.getReferenceTableName()) &&
          (attributes.size() == constraint.getAttributes().size()) &&
          (usingMetricName.size() == constraint.getMetricsNames().size()) &&
          (referenceAttributes.size() == constraint.getReferenceAttributes().size())){


        for (size_t x = 0; x < attributes.size(); x++){
            if (attributes[x] != constraint.getAttributes()[x])
                answer = false;
        }
        for (size_t x = 0; x < referenceAttributes.size(); x++){
            if (referenceAttributes[x] != constraint.getReferenceAttributes()[x])
                answer = false;
        }

        for (size_t x = 0; x < usingMetricName.size(); x++){
            if ((usingMetricName[x].first != constraint.getMetricsNames()[x].first) ||
                    (usingMetricName[x].second != constraint.getMetricsNames()[x].second))
                answer = false;
        }

    } else {
        answer = false;
    }

    return answer;
}

std::string Constraint::getName(){

    return name;
}

unsigned char Constraint::getType(){

    return type;
}

std::vector<std::string> Constraint::getAttributes(){

    return attributes;
}

std::string Constraint::getReferenceTableName(){

    return referenceTableName;
}

std::vector< std::pair<std::string, bool> > Constraint::getMetricsNames(){

    bool def = false;

    for (size_t x = 0; x < usingMetricName.size(); x++){
        if (usingMetricName[x].second)
            def = true;
    }
    if (!def){
        if (usingMetricName.size() > 0)
            usingMetricName[0].second = true;
    }
    return usingMetricName;
}

std::vector<std::string> Constraint::getReferenceAttributes(){

    return referenceAttributes;
}

CheckConstraints::CheckConstraints(DataDictionary *dic){

    this->dic = dic;
}

CheckConstraints::~CheckConstraints(){

    constraintList.clear();
    error.clear();
}

void CheckConstraints::addNewConstraint(Constraint constraint){

    constraintList.push_back(constraint);
}

void CheckConstraints::addError(std::string error){

    if (this->error.size() > 0){
        this->error += "\n";
    }
    this->error += error;

}

int CheckConstraints::countNames(std::string constraintName){

    int count = 0;
    for (size_t x = 0; x < constraintList.size(); x++){
        if ((!constraintName.empty()) && (constraintList[x].getName() == constraintName)){
            count++;
        }
    }

    return count;
}

int CheckConstraints::countMetrics(Constraint constraint){

    int count = 0;

    for (size_t x = 0; x < constraint.getMetricsNames().size(); x++){
        count = 0;
        for (size_t y = 0; y < constraint.getMetricsNames().size(); y++){
            if (constraint.getMetricsNames()[x].first == constraint.getMetricsNames()[y].first){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countDefaults(Constraint constraint){

    int count = 0;

    for (size_t x = 0; x < constraint.getMetricsNames().size(); x++){
        if (constraint.getMetricsNames()[x].second){
            count++;
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countPrimaryKey(){

    int count = 0;
    for (size_t x = 0; x < constraintList.size(); x++){
        if (constraintList[x].getType() == Constraint::PRIMARY_KEY){
            count++;
        }
    }

    return count;
}

int CheckConstraints::countUnique(Constraint constraint){

    int count = 0;

    for (size_t x = 0; x < constraint.getAttributes().size(); x++){
        count = 0;
        for (size_t y = 0; y < constraint.getAttributes().size(); y++){
            if (constraint.getAttributes()[x] == constraint.getAttributes()[y]){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

int CheckConstraints::countForeignKey(Constraint constraint){

    int count = 0;

    for (size_t x = 0; x < constraint.getReferenceAttributes().size(); x++){
        count = 0;
        for (size_t y = 0; y < constraint.getReferenceAttributes().size(); y++){
            if (constraint.getReferenceAttributes()[x] == constraint.getReferenceAttributes()[y]){
                count++;
            }
        }
        if (count > 1){
            return count;
        }
    }

    return count;
}

bool CheckConstraints::checkAttributes(CheckValidColumnsAndTypes *columnsAndTypes){

    for (size_t x = 0; x < constraintList.size(); x++){
        for (size_t y = 0; y < constraintList[x].getAttributes().size(); y++){
            bool found = false;
            for (size_t z = 0; z < columnsAndTypes->getSize(); z++){
                if (constraintList[x].getAttributes()[y] == columnsAndTypes->getColumnNameAndDataType(z).first){
                    found = true;
                }
            }
            if (!found){
                addError("Attribute: '" + constraintList[x].getAttributes()[y] + "' has not been defined and cannot be used in a constraint expression.");
                return false;
            }
        }
    }
    return true;
}

bool CheckConstraints::isValid(CheckValidColumnsAndTypes *columnsAndTypes){

    if (!checkAttributes(columnsAndTypes))
        return false;

    if (!isConstraint())
        return false;

    for (size_t x = 0; x < constraintList.size(); x++){

        Constraint aux = constraintList[x];
        int count = 0;

        for (size_t y = 0; y < constraintList.size(); y++){
            if (aux.isEqual(constraintList[y])){
                count++;
            }
        }

        if (count > 1){
            addError("Duplicate definition of constraint for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (aux.getType() == Constraint::FOREIGN_KEY){
            for (size_t y = 0; y < aux.getAttributes().size(); y++){
                for (size_t z = 0; z < columnsAndTypes->getSize(); z++){
                    bool b1 = columnsAndTypes->getColumnNameAndDataType(z).first == aux.getAttributes()[y];
                    bool b2 = columnsAndTypes->getColumnNameAndDataType(z).second != dic->columnDataType(aux.getReferenceTableName().c_str(), aux.getReferenceAttributes()[y].c_str()).toStdString();
                    if (b1 && b2){
                        addError("Invalid constraint reference for: '" + columnsAndTypes->getColumnNameAndDataType(z).first + "'. Data-type of the reference column should be: '" + columnsAndTypes->getColumnNameAndDataType(z).second + "'.");
                        return false;
                    }
                }
            }
        }

        if (aux.getType() == Constraint::METRIC){
            for (size_t z = 0; z < columnsAndTypes->getSize(); z++){
                for (size_t y = 0; y < aux.getMetricsNames().size(); y++){
                    bool b1 = columnsAndTypes->getColumnNameAndDataType(z).first == aux.getAttributes()[y];
                    bool b2 = columnsAndTypes->getColumnNameAndDataType(z).second != dic->getMetricType(dic->getMetricCode(aux.getMetricsNames()[y].first.c_str())).toStdString();
                    if (b1 && b2){
                        addError("Metric '" + aux.getMetricsNames()[y].first + "' does not provide support to " + columnsAndTypes->getColumnNameAndDataType(z).second);
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//rem column_name
bool CheckConstraints::isConstraint(){

    if (countPrimaryKey() > 1){
        addError("Duplicate primary key.");
        return false;
    }

    if (countPrimaryKey() < 1){
        addError("You must specify a primary key.");
        return false;
    }

    for (size_t x = 0; x < constraintList.size(); x++){
        Constraint aux = constraintList[x];

        if (countNames(aux.getName()) > 1){
            addError("Duplicate name for constraint: '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countMetrics(aux) > 1){
            addError("Duplicate metric for attribute '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countDefaults(aux) > 1){
            addError("There are more than one default metric for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countUnique(aux) > 1){
            addError("Duplicate unique columns for '" + aux.getAttributes()[0] + "'.");
            return false;
        }

        if (countForeignKey(aux) > 1){
            addError("Ambiguous definition for the foreign key on attribute: '" + aux.getAttributes()[0] + "'.");
            return false;
        }
    }
    return true;
}

Constraint CheckConstraints::getConstraint(size_t pos)  throw (std::exception*){

    if (pos < getSize()){
        return constraintList[pos];
    } else {
        throw new std::exception();
    }
}

size_t CheckConstraints::getSize(){

    return constraintList.size();
}

std::string CheckConstraints::getError(){

    return error;
}

DataDictionary* CheckConstraints::dictionary(){

    return dic;
}

std::string CheckConstraints::toString(){


    std::string answer =  "CONSTRAINT NAME ||  TYPE || AT LIST || TB || AT REF || METRIC REF \n";
    for (size_t x = 0; x < constraintList.size(); x++){
        if (x > 0)
            answer += "\n";
        answer += constraintList[x].getName() + " || ";
        if (constraintList[x].getType() == Constraint::PRIMARY_KEY){
            answer += "PK";
        }
        if (constraintList[x].getType() == Constraint::FOREIGN_KEY){
            answer += "FK";
        }
        if (constraintList[x].getType() == Constraint::UNIQUE){
            answer += "UE";
        }
        if (constraintList[x].getType() == Constraint::METRIC){
            answer += "MC";
        }

        if (constraintList[x].getType() == Constraint::NOT_NULL){
            answer += "NN";
        }
        answer += " || ";

        for (size_t y = 0; y < constraintList[x].getAttributes().size(); y++){
            answer += constraintList[x].getAttributes()[y] + " - ";
        }
        answer += " || ";

        answer += constraintList[x].getReferenceTableName();
        answer += " || ";

        for (size_t y = 0; y < constraintList[x].getReferenceAttributes().size(); y++){
            answer += constraintList[x].getReferenceAttributes()[y] + " - ";
        }
        answer += " || ";

        for (size_t y = 0; y < constraintList[x].getMetricsNames().size(); y++){
            answer += constraintList[x].getMetricsNames()[y].first + " - ";
        }
    }

    return answer;
}
