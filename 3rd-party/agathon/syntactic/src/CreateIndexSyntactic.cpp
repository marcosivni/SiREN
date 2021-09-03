#include "CreateIndexSyntactic.h"

CreateIndexSyntactic::~CreateIndexSyntactic(){

    destroyAuxiliaryStructures();
}

void CreateIndexSyntactic::destroyAuxiliaryStructures(){

    tableName.clear();
    indexName.clear();
    for (size_t x = 0; x < columnName.size(); x++){
        columnName[x].clear();
    }
    columnName.clear();
}

std::string CreateIndexSyntactic::getTableName(){

    return tableName;
}

std::string CreateIndexSyntactic::getColumnName(size_t pos) throw (std::exception*){

    if (pos < columnName.size())
        return columnName[pos];
    else
        throw (new std::exception());
}

std::string CreateIndexSyntactic::getIndexName(){

    return indexName;
}

void CreateIndexSyntactic::index_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        indexName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("Identifier expected after CREATE INDEX. '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateIndexSyntactic::table_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if ((!(dictionary()->isComplexAttributeReference(currentToken().getLexem().c_str(), getIndexName().c_str()))) &&
                ((dictionary()->isComplexAttributeIndex(currentToken().getLexem().c_str(), getIndexName().c_str()) ||
                  (dictionary()->isValidIndex(currentToken().getLexem().c_str(), getIndexName().c_str()))))){
            addError("Index '" + getIndexName() + "' on '" + currentToken().getLexem() + "' already exists!");
        } else {
            tableName = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Table name expected after ON. '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateIndexSyntactic::column_name(){

    bool first = true;
    while ((currentToken().getLexem() != ")") || (first)){
        first = false;
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(getParser()->getCurrentToken());
        }
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
            if ((dictionary()->isValidColumn(getTableName().c_str(), currentToken().getLexem().c_str())) ||
                    dictionary()->isComplexAttributeReference(getTableName().c_str(), currentToken().getLexem().c_str())){
                columnName.push_back(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            } else {
                addError("Column " + currentToken().getLexem() + " does not exists in table " + getTableName() + ".");
                return;
            }
        } else {
            addError("Column name expected after '('. '" + currentToken().getLexem() + "' found instead.");
        }
    }
}

void CreateIndexSyntactic::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dictionary()->isMetric(currentToken().getLexem().c_str())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError(currentToken().getLexem() + " metric does not exists.");
        }
    } else {
        addError("Identifier expected after USING '" + currentToken().getLexem() + "' found instead.");
    }
}

void CreateIndexSyntactic::create_index_specification(){

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "INDEX"){
                getParser()->match(getParser()->getCurrentToken());
                index_name();
                if (currentToken().toUpperLexem() == "ON"){
                    getParser()->match(getParser()->getCurrentToken());
                } else {
                    addError("ON clause expected, '" + currentToken().getLexem() + "' found instead.");
                }
                table_name();
                getParser()->match("(", Token::TK_OPEN_BRACE);
                column_name();
                getParser()->match(")", Token::TK_CLOSE_BRACE);

                for (size_t k = 0; ((k < columnName.size()) && (endOfStatement())); k++){
                    if(dictionary()->isComplexAttributeReference(getTableName().c_str(), getColumnName(k).c_str())){
                        addError("'" + getColumnName(k) + "' is a complex attribute. The <metric_name> clause is missing .");
                    }
                }

                if ((!endOfStatement()) && (currentToken().toUpperLexem() == "USING")){
                    getParser()->match(getParser()->getCurrentToken());
                    if ((currentToken().toUpperLexem() == "BTREE") ||
                            (currentToken().toUpperLexem() == "HASH")){
                        getParser()->match(getParser()->getCurrentToken());
                    } else {
                        for (size_t k = 0; k < columnName.size(); k++){
                            if (dictionary()->isComplexAttributeReference(getTableName().c_str(), getColumnName(k).c_str())){
                                addError("'" + getColumnName(k) + "' is not a complex attribute.");
                            } else {
                                if (k > 0){
                                    addError("There are more than one column for the complex index definition: " + getColumnName(k) + "'. Only one complex attribute is allowed.");
                                }
                                if (dictionary()->isValidMetric(currentToken().getLexem().c_str(), getTableName().c_str(), getColumnName(k).c_str())){
                                    addError("An index for metric " + currentToken().getLexem() + " already exists.");
                                } else {
                                    metric_name();
                                }
                            }
                        }
                    }
                }

                int count;
                for (size_t x = 0; x < columnName.size(); x++){
                    count = 0;
                    for (size_t y = 0; y < columnName.size(); y++){
                        if (getColumnName(x) == getColumnName(y))
                            count++;
                    }
                    if (count > 1){
                        addError("Duplicate column on index reference: '" + getColumnName(x) + "'.");
                        return;
                    }
                }

                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("Missing INDEX after CREATE clause.");
            }
        } else {
            addError("Missing CREATE clause.");
        }
    } catch (...){
        addError("Syntax error when processing CREATE INDEX near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool CreateIndexSyntactic::isValid(std::string sqlStatement){

    destroyAuxiliaryStructures();
    getParser()->parse(sqlStatement);
    create_index_specification();
    return (!hasErrors());
}
