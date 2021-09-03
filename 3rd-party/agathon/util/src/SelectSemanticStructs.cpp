#include "SelectSemanticStructs.h"


QueryMetaTree::QueryMetaTree(){
}

QueryMetaTree::~QueryMetaTree(){

    for (size_t x = 0; x < tableNames.size(); x++){
        tableNames[x].clear();
        tableAliases[x].clear();
        attributeAliases[x].clear();
        attributeNames[x].clear();
        originalCATables[x].clear();
        originalCAs[x].clear();
    }
    tableNames.clear();
    tableAliases.clear();
    attributeAliases.clear();
    attributeNames.clear();
    originalCATables.clear();
    originalCAs.clear();
}

void QueryMetaTree::add(std::string tableName, std::string tableAlias, std::string attributeName, std::string attributeAlias, std::string originalCATable, std::string originalCA){

    tableNames.push_back(tableName);
    tableAliases.push_back(tableAlias);
    attributeAliases.push_back(attributeAlias);
    attributeNames.push_back(attributeName);
    originalCATables.push_back(originalCATable);
    originalCAs.push_back(originalCA);
}

void QueryMetaTree::add(std::string tableName, std::string tableAlias, std::string attributeName){

    tableNames.push_back(tableName);
    tableAliases.push_back(tableAlias);
    attributeAliases.push_back("");
    attributeNames.push_back(attributeName);
    originalCATables.push_back("");
    originalCAs.push_back("");
}


void QueryMetaTree::add(std::string tableName){

    tableNames.push_back(tableName);
    tableAliases.push_back(tableName);
    attributeAliases.push_back("");
    attributeNames.push_back("");
    originalCATables.push_back("");
    originalCAs.push_back("");
}

void QueryMetaTree::addAliasToLastTable(std::string tableAlias){

    tableAliases[tableAliases.size()-1] = tableAlias;
}

void QueryMetaTree::modifyAttributeAlias(std::string attributeAlias, size_t pos){

    attributeAliases[pos] = attributeAlias;
}

std::vector<std::string> QueryMetaTree::tables(){

    return tableNames;
}

std::vector<std::string> QueryMetaTree::aliasesOfTables(){

    return tableAliases;
}

std::vector<std::string> QueryMetaTree::attributes(){

    return attributeNames;
}

std::vector<std::string> QueryMetaTree::aliasesOfAttributes(){

    return attributeAliases;
}

size_t QueryMetaTree::size(){

    return tableNames.size();
}

std::vector<std::string> QueryMetaTree::caTables(){

    return originalCATables;
}

std::vector<std::string> QueryMetaTree::cAttributes(){

    return originalCAs;
}

bool QueryMetaTree::isVirtual(std::string tableName){

    return QString(tableName.c_str()).contains("Select ", Qt::CaseInsensitive);
}

//Reserved word
bool QueryMetaTree::alreadyTranslatedStatement(std::string tableName){

    return QString(tableName.c_str()).contains("$", Qt::CaseInsensitive);
}

bool QueryMetaTree::isVirtual(size_t pos){

    return QString(tableNames[pos].c_str()).contains("Select ", Qt::CaseInsensitive);
}

bool QueryMetaTree::isSimilarityAttribute(size_t pos){

    return ((!caTables()[pos].empty()) && (!cAttributes()[pos].empty()));
}


bool QueryMetaTree::exists(std::string tableAlias){

    bool answer = false;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        answer |= (((tableAliases[x].empty()) && (tableNames[x] == tableAlias)) || (tableAliases[x] == tableAlias));
    }

    return answer;
}

size_t QueryMetaTree::findFirst(std::string tableAlias){

    if (!size()){
        throw new std::runtime_error("Invalid table: '" + tableAlias + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (((tableAliases[x].empty()) && (tableNames[x] == tableAlias)) || (tableAliases[x] == tableAlias)){
            answer = x;
        }
    }

    return answer;
}

size_t QueryMetaTree::findFirstPosition(std::string attributeAlias){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeAlias + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (((attributeAliases[x].empty()) && (attributeNames[x] == attributeAlias)) || (attributeAliases[x] == attributeAlias)){
            answer = x;
        }
    }

    return answer;
}

size_t QueryMetaTree::matchAttribute(std::string attributeName){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (attributeNames[x] == attributeName){
            answer = x;
        }
    }

    return answer;
}

size_t QueryMetaTree::matchAttribute(std::string attributeName, std::string tableOrAttributeAlias, bool attNameTblAlias){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    if (attNameTblAlias){
        for (size_t x = 0; ((x < size()) && !answer); x++){
            if (attributeNames[x] == attributeName && tableAliases[x] == tableOrAttributeAlias){
                answer = x;
            }
        }
    } else {
        for (size_t x = 0; ((x < size()) && !answer); x++){
            if (attributeNames[x] == attributeName && attributeAliases[x] == tableOrAttributeAlias){
                answer = x;
            }
        }
    }

    return answer;
}

size_t QueryMetaTree::matchAttribute(std::string attributeName, std::string attributeAlias, std::string tableAlias){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (attributeNames[x] == attributeName && attributeAliases[x] == attributeAlias && tableAliases[x] == tableAlias){
            answer = x;
        }
    }

    return answer;
}

void QueryMetaTree::print(){

    for (size_t x = 0; x < tableNames.size(); x++){
        std::cout << tableNames[x] << " | " << tableAliases[x] << " | " << attributeNames[x] << " | " << attributeAliases[x] << " | " << caTables()[x] << " | " << cAttributes()[x] << std::endl;
    }
}

ProjectionListMetaInfo::ProjectionListMetaInfo(){
}

ProjectionListMetaInfo::~ProjectionListMetaInfo(){

    attributeAliases.clear();
    tableAliases.clear();
    attributeNames.clear();
}

void ProjectionListMetaInfo::add(std::string tableAlias, std::string attributeName, std::string attributeAlias){

    attributeAliases.push_back(attributeAlias);
    attributeNames.push_back(attributeName);
    tableAliases.push_back(tableAlias);
}

void ProjectionListMetaInfo::add(std::string tableAlias, std::string attributeName){

    attributeAliases.push_back("");
    attributeNames.push_back(attributeName);
    tableAliases.push_back(tableAlias);
}

void ProjectionListMetaInfo::add(std::string attributeName){

    attributeAliases.push_back("");
    attributeNames.push_back(attributeName);
    tableAliases.push_back("");
}

void ProjectionListMetaInfo::addAliasToLastTable(std::string attributeAlias){

    attributeAliases[attributeAliases.size()-1] = attributeAlias;
}

void ProjectionListMetaInfo::addAliasToLastTable(std::string tableAlias, std::string attributeAlias){

    attributeAliases[attributeAliases.size()-1] = attributeAlias;
    tableAliases[tableAliases.size()-1] = tableAlias;
}

std::vector<std::string> ProjectionListMetaInfo::aliasesOfTables(){

    return tableAliases;
}

std::vector<std::string> ProjectionListMetaInfo::attributes(){

    return attributeNames;
}

std::vector<std::string> ProjectionListMetaInfo::aliasesOfAttributes(){

    return attributeAliases;
}

size_t ProjectionListMetaInfo::size(){

    return attributeNames.size();
}

size_t ProjectionListMetaInfo::findFirstPosition(std::string attributeName){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (attributeNames[x] == attributeName){
            answer = x;
        }
    }

    return answer;
}

size_t ProjectionListMetaInfo::findFirstPosition(std::string attributeName, std::string attributeAlias){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (attributeNames[x] == attributeName && attributeAliases[x] == attributeAlias){
            answer = x;
        }
    }

    return answer;
}

size_t ProjectionListMetaInfo::findFirstPosition(std::string tableAlias, std::string attributeName, std::string attributeAlias){

    if (!size()){
        throw new std::runtime_error("Invalid attribute: '" + attributeName + "'.");
    }

    size_t answer = 0;

    for (size_t x = 0; ((x < size()) && !answer); x++){
        if (attributeNames[x] == attributeName && attributeAliases[x] == attributeAlias && tableAliases[x] == tableAlias){
            answer = x;
        }
    }

    return answer;
}

void ProjectionListMetaInfo::print(){

    for (size_t x = 0; x < size(); x++){
        std::cout << tableAliases[x] << " | " << attributeNames[x] << " | " << attributeAliases[x] << std::endl;
    }
}

