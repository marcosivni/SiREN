#include "InsertIntoSyntactic.h"

InsertIntoSyntactic::~InsertIntoSyntactic(){

    destroyAuxiliaryStructures();
}

void InsertIntoSyntactic::destroyAuxiliaryStructures(){

    tableName.clear();
    for (size_t x = 0; x < columns.size(); x++)
        columns[x].clear();
    columns.clear();
    for (size_t x = 0; x < values.size(); x++)
        values[x].clear();
    values.clear();
}

void InsertIntoSyntactic::insert_column(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (!dictionary()->isValidColumn(getTableName().c_str(), currentToken().getLexem().c_str())){
            addError("Column '" + currentToken().getLexem() + "' does not exists in the table " + getTableName() + ".");
        } else {
            columns.push_back(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void InsertIntoSyntactic::insert_column_list(){

    insert_column();
    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        insert_column_list();
    }
}

void InsertIntoSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isValidTable(currentToken().getLexem().c_str())){
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Table '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

std::string InsertIntoSyntactic::build_particulate(){

    std::string answer;

    if ((currentToken().getTokenType() == Token::TK_FLOATING_POINT)
        || (currentToken().getTokenType() == Token::TK_INTEGER)){
        answer += currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Either integer or floating point expected after '{'. '" + currentToken().getLexem() + "' found instead.");
    }

    return answer;
}

std::string InsertIntoSyntactic::build_particulate_list(size_t *count){

    std::string answer;

    answer += build_particulate();
    (*count)++;
    if (currentToken().getTokenType() == Token::TK_COMMA) {
        answer += currentToken().getLexem();
        getParser()->match(",", Token::TK_COMMA);
        answer += build_particulate_list(count);
    }

    return answer;
}

void InsertIntoSyntactic::insert_value(size_t *position){

    if ((currentToken().getTokenType() == Token::TK_INTEGER)  ||
        (currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
        (currentToken().getTokenType() == Token::TK_STRING) ||
        (currentToken().toUpperLexem() == "NULL") ){
        values.push_back(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().getTokenType() == Token::TK_OPEN_BLOCK){
            getParser()->match(getParser()->getCurrentToken());
            size_t count = 0;
            values.push_back(build_particulate_list(&count));
            if (count != dictionary()->numberOfParticulateDimensions(tableName.c_str(), columns[(*position)].c_str())){
                addError("A different number of dimensions was provided. '" + QString::number(dictionary()->numberOfParticulateDimensions(tableName.c_str(), columns[(*position)].c_str())).toStdString() + "' dimensions expected, "+ QString::number(count).toStdString() + " provided.");
            } else {
                if (currentToken().getTokenType() == Token::TK_CLOSE_BLOCK){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("Sign '}' expected. '" + currentToken().getLexem() + "' found instead.");
                }
            }
        } else {
            addError("Integer, floating point or string expected. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

void InsertIntoSyntactic::insert_value_list(size_t *position){

    insert_value(position);
    (*position)++;
    if (currentToken().getTokenType() == Token::TK_COMMA) {
        getParser()->match(",", Token::TK_COMMA);
        insert_value_list(position);
    }
}

void InsertIntoSyntactic::load_all_columns(){

    QStringList cols =  dictionary()->allColumnNamesFromTable(getTableName().c_str());

    for (int x = 0; x < cols.size(); x++){
        columns.push_back(cols.at(x).toStdString());
    }
    cols.clear();
}

void InsertIntoSyntactic::insert_specification(){

    try{
        if (currentToken().toUpperLexem() == "INSERT"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INTO"){
                getParser()->match(getParser()->getCurrentToken());
                table_name();
                if (currentToken().getLexem() == "("){
                    getParser()->match(getParser()->getCurrentToken());
                    insert_column_list();
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                } else {
                    load_all_columns();
                }
                if (currentToken().toUpperLexem() == "VALUES"){
                    getParser()->match(getParser()->getCurrentToken());
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    size_t position = 0;
                    insert_value_list(&position);
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                    if (!endOfStatement()){
                        addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                    }
                } else {
                    addError("VALUES expected '" + currentToken().getLexem() + "' found instead");
                }
            } else {
                addError("INTO expected '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("INSERT INTO statement expected '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...){
        addError("Syntax error when processing INSERT INTO near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

std::string InsertIntoSyntactic::getTableName(){

    return tableName;
}

void InsertIntoSyntactic::checkColumnsAndValues(){

    if (columns.size() != values.size()){
        addError("Missing or extra values to insert.");
        return;
    }

    bool found = false;
    bool pkCheck = false;
    for (size_t x = 0; x < columns.size(); x++){
        if (dictionary()->isPrimaryKey(getTableName().c_str(), columns[x].c_str())){
            found = true;
            if (!dictionary()->isValidPrimaryKey(values[x].c_str(), getTableName().c_str(), columns[x].c_str())){
                pkCheck = (pkCheck || false);
            } else {
                pkCheck = true;
            }
        }
    }

    if (!found){
        addError("Missing primary key value to insert.");
        return;
    }

    if (!pkCheck){
        addError("Duplicate entry for primary-key specification.");
        return;
    }

    QPair<QStringList, QStringList> nullable = dictionary()->allNullableColumnsInATable(getTableName().c_str());

    for (int x = 0; x < nullable.first.size(); x++){
        if (!nullable.second.at(x).toInt()){
            bool found = false;
            for (size_t y = 0; y < columns.size(); y++){
                if (columns[y] == nullable.first.at(x).toStdString()){
                    found = true;
                }
            }
            if (!found){
                addError("Column '" + nullable.first.at(x).toStdString() + "' is defined as not null. Please, insert a valid value.");
            }
        }
    }

    nullable.first.clear();
    nullable.second.clear();

    for (size_t x = 0; x < columns.size(); x++){
        if (dictionary()->isComplexAttributeReference(getTableName().c_str(), columns[x].c_str())){
                QString particulate = values[x].c_str();
                QStringList dimensions = particulate.split(QLatin1Char(','), QString::SkipEmptyParts);
                if (dimensions.size() != dictionary()->numberOfParticulateDimensions(tableName.c_str(), columns[x].c_str())){
                    addError(QString::number(dictionary()->numberOfParticulateDimensions(tableName.c_str(), columns[x].c_str())).toStdString() + " dimensions are expected for " + columns[x] + ". " + QString::number(dimensions.size()).toStdString() + " dimensions were provided.");
                    addError("Data: " + values[x] + " is not a valid SIREN data type.");
                }
                dimensions.clear();
        }
    }
}

bool InsertIntoSyntactic::isValid(std::string sqlStatement){

    destroyAuxiliaryStructures();
    getParser()->parse(sqlStatement);
    insert_specification();
    return (!hasErrors());
}

