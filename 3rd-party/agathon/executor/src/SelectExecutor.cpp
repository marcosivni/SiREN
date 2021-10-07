#include "SelectExecutor.h"

SelectExecutor::~SelectExecutor(){

    destroyAuxiliaryStructures();
}

void SelectExecutor::destroyAuxiliaryStructures(){

    if (metadata != nullptr){
        delete (metadata);
    }
    if (pjListMetadata != nullptr){
        delete (pjListMetadata);
    }
}

bool SelectExecutor::isWhereGroupOrderWord(std::string word){

    bool answer = false;
    std::vector<std::string> reservedWords = {"WHERE", "GROUP", "ORDER"};

    for (size_t x = 0; (x < reservedWords.size() && !answer); x++){
        answer = (word == reservedWords[x]);
    }

    return answer;
}

bool SelectExecutor::isJoinType(std::string word){

    bool answer = false;

    for (size_t x = 0; (x < joinTypes.size() && !answer); x++){
        answer = (word == joinTypes[x]);
    }

    return answer;
}

bool SelectExecutor::isPredicateType(std::string word){

    bool answer = false;

    for (size_t x = 0; (x < predicateTypes.size() && !answer); x++){
        answer = (word == predicateTypes[x]);
    }

    return answer;
}

bool SelectExecutor::isAggregationType(std::string word){

    bool answer = false;

    for (size_t x = 0; (x < aggregationTypes.size() && !answer); x++){
        answer = (word == aggregationTypes[x]);
    }

    return answer;
}

void SelectExecutor::buildFullInfoQueryMetaTree(){

    std::string tableName, tableAlias, attName, attAlias, vTableName, vAttName;
    QStringList columns;
    QueryMetaTree *auxMetadata = new QueryMetaTree();
    size_t it = metadata->size();

    for (size_t x = 0; x < it; x++){
        tableName = metadata->tables()[x];
        tableAlias = metadata->aliasesOfTables()[x];

        if (!metadata->isVirtual(x)){
            if (dictionary()->isValidTable(tableName.c_str())){
                columns = dictionary()->allColumnNamesFromTable(tableName.c_str());
            } else {
                QMap<QString, QStringList>* resultSet = dbManager()->runSelect(tableName.c_str());
                if (resultSet != nullptr){
                    delete (resultSet);
                }
                columns = dbManager()->fetchAttributesNames();
            }

            for (int y = 0; y < columns.size(); y++){
                vTableName.clear();
                vAttName.clear();
                if (dictionary()->isComplexAttributeReference(tableName.c_str(), columns.at(y))){
                    vTableName = tableName;
                    vAttName = columns.at(y).toStdString();
                }
                auxMetadata->add(tableName, tableAlias, columns.at(y).toStdString(), columns.at(y).toStdString(), vTableName, vAttName);
            }
        } else {
            attName = metadata->attributes()[x];
            attAlias = metadata->aliasesOfAttributes()[x];
            vTableName = metadata->caTables()[x];
            vAttName = metadata->cAttributes()[x];
            auxMetadata->add(tableName, tableAlias, attName, attAlias, vTableName, vAttName);
        }
    }

    if (metadata != nullptr){
        delete (metadata);
    }

    metadata = new QueryMetaTree(*auxMetadata);
    if (auxMetadata != nullptr){
        delete (auxMetadata);
    }
}

void SelectExecutor::buildInfoQueryMetaTree(){

    QueryMetaTree *auxMetadata = new QueryMetaTree();
    size_t location;
    std::string tableName, attAlias, attName;

    for (size_t x = 0; x < pjListMetadata->size(); x++){
        tableName = pjListMetadata->aliasesOfTables()[x];
        attAlias = pjListMetadata->aliasesOfAttributes()[x];
        attName = pjListMetadata->attributes()[x];

        if (!tableName.empty() && !attAlias.empty()){
            location = metadata->matchAttribute(attName, attAlias, tableName);
        } else {
            if (!attAlias.empty()){
                location = metadata->matchAttribute(attName, attAlias, false);
            } else {
                if (!tableName.empty()){
                    location = metadata->matchAttribute(attName, tableName, true);
                } else {
                    location = metadata->matchAttribute(attName);
                }
            }
        }
        auxMetadata->add(metadata->tables()[location],
                         metadata->aliasesOfTables()[location],
                         metadata->aliasesOfAttributes()[location],
                         metadata->aliasesOfAttributes()[location],
                         metadata->caTables()[location],
                         metadata->cAttributes()[location]);
    }

    if (metadata != nullptr){
        delete (metadata);
    }

    metadata = new QueryMetaTree(*auxMetadata);
    if (auxMetadata != nullptr){
        delete (auxMetadata);
    }
}

void SelectExecutor::renameMetadataAttributes(){

    std::string tableName, attAlias;
    size_t location;
    for (size_t x = 0; x < pjListMetadata->size(); x++){
        tableName = pjListMetadata->aliasesOfTables()[x];
        attAlias = pjListMetadata->aliasesOfAttributes()[x];
        if (!attAlias.empty()){
            if (!tableName.empty()){
                location = metadata->matchAttribute(pjListMetadata->attributes()[x], tableName);
            } else {
                location = metadata->matchAttribute(pjListMetadata->attributes()[x]);
            }
            metadata->modifyAttributeAlias(attAlias, location);
        }
    }
}

QueryMetaTree SelectExecutor::fetchMetadata(){

    return *metadata;
}


void SelectExecutor::populateSequentialScan(IndexManager *idx, QMap<QString, QStringList>* tempResultSet) throw (std::exception *){

    //Abort execution
    if (tempResultSet->isEmpty()){
        throw new std::runtime_error("Nested subselect query returning an empty set.");
    }
    //

    QStringList aux = tempResultSet->constBegin().value();
    for (int x = 0; x < aux.size(); x++){
        FeatureVector fv;
        fv.unserializeFromString(FeatureVector::fromBase64(aux.at(x).toStdString()));
        idx->addItem(&fv);
    }
}



SelectExecutor* SelectExecutor::reshapeVirtualTable(){

    Token *t;
    std::vector<Token *> tList;
    int bef, after;

    bef = getParser()->getCurrentPosition();
    std::string txt = fetchSubselectStatement();
    after = getParser()->getCurrentPosition();

    for (int x = 0; x < (after-bef); x++){
        getParser()->removeToken(bef);
    }

    //Translate possible extended SQL
    SelectExecutor *aux = new SelectExecutor(txt, dbManager());
    txt = aux->translateToRegularSQL(true);

    //Reorganize tokens to generate a standard SQL
    Parser *p = new Parser(txt);
    for (size_t x = 0; x < p->countTokens(); x++){
        t = new Token(*p->getToken(x));
        tList.push_back(t);
    }
    getParser()->insertTokenList(tList, bef);

    for (size_t x = (after - bef); x < tList.size(); x++){
        getParser()->match(getParser()->getCurrentToken());
    }

    //Destroy auxiliary structures
    delete (p);
    tList.clear();
    txt.clear();

    return aux;
}

std::string SelectExecutor::reshapeSelectBasedWhereCondition(){

    Token *t;
    std::vector<Token *> tList;

    int bef, after;
    bef = getParser()->getCurrentPosition();
    std::string txt = fetchSubselectStatement();
    after = getParser()->getCurrentPosition();

    for (int x = 0; x < (after-bef); x++){
        getParser()->removeToken(bef);
    }

    //Translate possible extended SQL
    if (!metadata->alreadyTranslatedStatement(txt)){
        SelectExecutor *aux = new SelectExecutor(txt, dbManager());
        txt = aux->translateToRegularSQL(true);
        delete (aux);
    }

    //Reorganize tokens to generate a standard SQL
    Parser *p = new Parser(txt);
    for (size_t x = 0; x < p->countTokens(); x++){
        t = new Token(*p->getToken(x));
        tList.push_back(t);
    }
    getParser()->insertTokenList(tList, bef);

    for (size_t x = (after - bef); x < tList.size(); x++){
        getParser()->match(getParser()->getCurrentToken());
    }

    //Destroy auxiliary structures
    delete (p);
    tList.clear();

    return txt;
}

////<query_obj_definition> ::= { [_valor_,]* _valor }
void SelectExecutor::query_obj_definition() {

    getParser()->match(getParser()->getCurrentToken());//Skip {

    getParser()->match(getParser()->getCurrentToken());
    while (currentToken().getTokenType() != Token::TK_CLOSE_BLOCK){
        if (currentToken().getTokenType() == Token::TK_COMMA){
            getParser()->match(",", Token::TK_COMMA);
            getParser()->match(getParser()->getCurrentToken());
        }
    }

    getParser()->match(getParser()->getCurrentToken());//Skip }

}


//// <value_expression> ::= ( subselect ) | (<value_check>) | <value_check>
void SelectExecutor::value_expression(){

    if (currentToken().getLexem() == "(" ){
        getParser()->match("(",Token::TK_OPEN_BRACE);
        if (currentToken().toUpperLexem() == "SELECT"){
            reshapeSelectBasedWhereCondition();
        } else {
            getParser()->match(getParser()->getCurrentToken());
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        getParser()->match(getParser()->getCurrentToken());
    }
}

//// <between_clause> ::= <value_check> AND <value_check>
void SelectExecutor::between_clause(){

    getParser()->match(getParser()->getCurrentToken());
    getParser()->match(getParser()->getCurrentToken());
    getParser()->match(getParser()->getCurrentToken());
}

//// <in_clause> ::=  ( subselect ) | (<value_check>) | ([<value_check>,]*<value_check>)
void SelectExecutor::in_clause(){

    getParser()->match("(",Token::TK_OPEN_BRACE);
    std::string aux;

    if (currentToken().toUpperLexem() == "SELECT"){
        aux = reshapeSelectBasedWhereCondition();
    } else {
        getParser()->match(getParser()->getCurrentToken());
        while (currentToken().getLexem() != ")"){
            getParser()->match(getParser()->getCurrentToken());//comma
            getParser()->match(getParser()->getCurrentToken());
        }
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    }

}

//// <join_predicate> ::= <column_reference><regular_predicate>
void SelectExecutor::join_predicate(){

    column_reference();
    regular_predicate();
}

//// <regular_predicate> ::=  > <column_reference>   | > <value_expression>
///                         | = <column_reference>   | = <value_expression>
///                         | != <column_reference>  | != <value_expression>
///                         | LIKE <column_reference>  | LIKE <value_expression>
///                         | <> <column_reference>  | <> <value_expression>
///                         | >= <column_reference>  | >= <value_expression>
///                         | <  <column_reference>  | < <value_expression>
///                         | <= <column_reference>  | <= <value_expression>
///                         | NOT IN <in_clause>  | IN <in_clause>
///                         | BETWEEN <between_clause>  | IN <between_clause>
void SelectExecutor::regular_predicate(){

    // equals operator
    if (currentToken().getLexem() == "=") {
        getParser()->match("=", Token::TK_EQUAL);
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            column_reference();
        } else {
            value_expression();
        }
    } else {
        // not equals operator
        if (currentToken().getLexem() == "!=") {
            getParser()->match("!=", Token::TK_NE);
            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                column_reference();
            } else {
                value_expression();
            }
        } else {
            // LIKE operator
            if (currentToken().toUpperLexem() == "LIKE") {
                getParser()->match(getParser()->getCurrentToken());
                value_expression();
            } else {
                // NOT EQUALS operator
                if (currentToken().getLexem() == "<>") {
                    getParser()->match("<>", Token::TK_N2);
                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                        column_reference();
                    } else {
                        value_expression();
                    }
                } else {
                    // less than operator
                    if (currentToken().getLexem() == "<") {
                        getParser()->match("<", Token::TK_LQ);
                        if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                            column_reference();
                        } else {
                            value_expression();
                        }
                    } else {
                        // less than or equals operator
                        if (currentToken().getLexem() == "<=") {
                            getParser()->match("<=", Token::TK_LE);
                            if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                column_reference();
                            } else {
                                value_expression();
                            }
                        } else {
                            // greater than operator
                            if (currentToken().getLexem() == ">") {
                                getParser()->match(">", Token::TK_GQ);
                                if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
                                    column_reference();
                                } else {
                                    value_expression();
                                }
                            } else {
                                // greater than or equals operator
                                if (currentToken().getLexem() == ">=") {
                                    getParser()->match(">=", Token::TK_GE);
                                    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
                                        column_reference();
                                    } else {
                                        value_expression();
                                    }
                                }  else {
                                    // IN-list Operator
                                    if (currentToken().toUpperLexem() == "NOT"){
                                        getParser()->match(getParser()->getCurrentToken());
                                        if (currentToken().toUpperLexem() == "IN"){
                                            getParser()->match(getParser()->getCurrentToken());
                                            in_clause();
                                        }
                                    } else {
                                        if (currentToken().toUpperLexem() == "IN"){
                                            getParser()->match(getParser()->getCurrentToken());
                                            in_clause();
                                        } else {
                                            // BETWEEN operator
                                            if (currentToken().toUpperLexem() == "BETWEEN"){
                                                getParser()->match(getParser()->getCurrentToken());
                                                between_clause();
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//// <a> := SUM  | AVG | ALL | EVERY | ANY | MAX
//// <t> := NEAR | FAR
//// <similarity_predicate>  := DIVERSITY <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSITY <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSITY <t> <a> (subselect) BY <metric_name>
///                        | DIVERSITY <t> <a> (subselect)
///                        | DIVERSITY <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSITY <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSITY <t> <a> (column_reference) BY <metric_name>
///                        | DIVERSITY <t> <a> (column_reference)
///                        | DIVERSITY <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSITY <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSITY <t> <a> <query_obj_definition> BY <metric_name>
///                        | DIVERSITY <t> <a> <query_obj_definition>
///                        | DIVERSITY <t> <a> (subselect) BY <metric_name> RANGE <val_range>
///                        | DIVERSITY <t> <a> (column_reference) BY <metric_name> RANGE <val_range>
///                        | DIVERSITY <t> <a> <query_obj_definition> BY <metric_name> RANGE <val_range>
///                        | DIVERSIFIED <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSIFIED <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSIFIED <t> <a> (subselect) BY <metric_name>
///                        | DIVERSIFIED <t> <a> (subselect)
///                        | DIVERSIFIED <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSIFIED <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSIFIED <t> <a> (column_reference) BY <metric_name>
///                        | DIVERSIFIED <t> <a> (column_reference)
///                        | DIVERSIFIED <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | DIVERSIFIED <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest>
///                        | DIVERSIFIED <t> <a> <query_obj_definition> BY <metric_name>
///                        | DIVERSIFIED <t> <a> <query_obj_definition>
///                        | DIVERSIFIED <t> <a> (subselect) BY <metric_name> RANGE <val_range>
///                        | DIVERSIFIED <t> <a> (column_reference) BY <metric_name> RANGE <val_range>
///                        | DIVERSIFIED <t> <a> <query_obj_definition> BY <metric_name> RANGE <val_range>
///                        | <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | <t> <a> (subselect) BY <metric_name> STOP AFTER <k_nearest>
///                        | <t> <a> (subselect) BY <metric_name>
///                        | <t> <a> (subselect)
///                        | <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | <t> <a> (column_reference) BY <metric_name> STOP AFTER <k_nearest>
///                        | <t> <a> (column_reference) BY <metric_name>
///                        | <t> <a> (column_reference)
///                        | <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest> WITH <tie_list>
///                        | <t> <a> <query_obj_definition> BY <metric_name> STOP AFTER <k_nearest>
///                        | <t> <a> <query_obj_definition> BY <metric_name>
///                        | <t> <a> <query_obj_definition>
///                        | <t> <a> (subselect) BY <metric_name> RANGE <val_range>
///                        | <t> <a> (column_reference) BY <metric_name> RANGE <val_range>
///                        | <t> <a> <query_obj_definition> BY <metric_name> RANGE <val_range>
void SelectExecutor::similarity_predicate(){

    if (currentToken().toUpperLexem() == "DIVERSIFIED" || currentToken().toUpperLexem() == "DIVERSITY"){
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        getParser()->match(getParser()->getCurrentToken());
    }

    // Optional similarity grouping
    if (isAggregationType(currentToken().toUpperLexem())) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());//Skip (
        if (currentToken().toUpperLexem() == "SELECT"){
            reshapeSelectBasedWhereCondition();
            getParser()->match(getParser()->getCurrentToken());//Skip )
        } else {
            column_reference();
            getParser()->match(getParser()->getCurrentToken());//Skip )
        }
    } else {
        query_obj_definition();
    }


    // Optional BY clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "BY")) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    }
    // Optional RANGE clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "RANGE")) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    }
    // Optional STOP AFTER clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "STOP")) {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());

        // optional WITH clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WITH")) {
            getParser()->match(getParser()->getCurrentToken());
            getParser()->match(getParser()->getCurrentToken());//Skip WITH
            getParser()->match(getParser()->getCurrentToken());//Skipe TIE
        }
    }
}

//// <predicate> ::=  _identifier_ <regular_predicate> | _identifier_._identifier_ <regular_predicate>
///                 | _identifier_ <similarity_predicate> | _identifier_._identifier_ <similarity_predicate>
void SelectExecutor::predicate(){

    std::string tableName, attributeName;
    bool check_attr_ref;

    attributeName = getParser()->getCurrentToken()->getLexem();
    getParser()->match(getParser()->getCurrentToken());
    if (currentToken().getTokenType() == Token::TK_PERIOD) {
        getParser()->match(getParser()->getCurrentToken());
        tableName = attributeName;
        attributeName = getParser()->getCurrentToken()->getLexem();
        getParser()->match(getParser()->getCurrentToken());
    }
    if (tableName.empty()){
        tableName = discoverTableName(attributeName);
    }
    check_attr_ref = dictionary()->isComplexAttributeReference(tableName.c_str(), attributeName.c_str()) || isVirtualComplexAttribute(tableName, attributeName);

    if (check_attr_ref){
        similarity_predicate();
    } else {
        regular_predicate();
    }
}


////<search_condition> ::= NOT <predicate> | <predicate> | AND <search_condition> | OR <search_condition>
void SelectExecutor::search_condition(){

    if (currentToken().toUpperLexem() == "NOT") {
        getParser()->match(getParser()->getCurrentToken());
    }
    predicate();

    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "AND") || (currentToken().toUpperLexem() == "OR"))){
        getParser()->match(getParser()->getCurrentToken());
        search_condition();
    }
}

////having_clause ::= HAVING <search_condition>
void SelectExecutor::having_clause(){

    getParser()->match(getParser()->getCurrentToken());
    search_condition();
}


////<groupby_clause> ::= GROUP BY (<column_reference>) | GROUP BY ([<column_reference>,]*<column_reference>)
void SelectExecutor::groupby_clause(){

    if (!endOfStatement() && currentToken().toUpperLexem() == "GROUP"){
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    }
    getParser()->match("(", Token::TK_OPEN_BRACE);

    column_reference();
    while (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        column_reference();
    }
    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

////<where_clause> ::= WHERE <search_condition>
void SelectExecutor::where_clause(){

    if (currentToken().toUpperLexem() == "WHERE"){
        getParser()->match(getParser()->getCurrentToken());
    }
    search_condition();
}

//// <table_reference_list> ::= | <read_table>
///                             | <read_table>,<table_reference_list>
///                             | <read_table><typejoin_list><read_table><on_clause_list>
///                             | <read_table><typejoin_list><table_reference_list>
void SelectExecutor::table_reference_list(){

    //Read and rename table
    read_table();

    //Join
    if (!endOfStatement() && isJoinType(currentToken().toUpperLexem())){
        typejoin_list();
        read_table();
        on_clause_list();
    }

    //Cartesian Product
    if (!endOfStatement() && currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        table_reference_list();
    }
}

//Fetches a string within a parenthesis (subselect)
std::string SelectExecutor::fetchSubselectStatement(){

    std::string aux;
    int matching = 0;

    matching++;

    // While parenthisation is not finished, build the string
    while (matching > 0){

        if (getParser()->getCurrentToken()->getLexem() == "("){
            matching ++;
        }
        if (getParser()->getCurrentToken()->getLexem() == ")"){
            matching --;
        }

        if (matching > 0){
            aux.append(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
            while (currentToken().getTokenType() == Token::TK_PERIOD) {
                aux.append(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
                aux.append(currentToken().getLexem());
                getParser()->match(getParser()->getCurrentToken());
            }
            aux.append(" ");
        }
    }

    return aux;
}

//// <read_table> ::=   _identifier_
////                  | _identifier_ <as_clause>
///                   | ( query_specification ) <as_clause>
void SelectExecutor::read_table(){

    if (currentToken().getTokenType() == Token::TK_OPEN_BRACE){
        getParser()->match("(",Token::TK_OPEN_BRACE);


        SelectExecutor *vTable = reshapeVirtualTable();
        std::string alias;
        QueryMetaTree mData = vTable->fetchMetadata();

        getParser()->match(getParser()->getCurrentToken());//Skip }
        if (currentToken().toUpperLexem() == "AS"){
            getParser()->match(getParser()->getCurrentToken());
        }
        alias = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());

        for (size_t x = 0; x < mData.size(); x++){
            metadata->add(vTable->getParser()->getOriginal(),
                          alias,
                          mData.attributes()[x],
                          mData.aliasesOfAttributes()[x],
                          mData.caTables()[x],
                          mData.cAttributes()[x]);
        }
        if (vTable != nullptr){
            delete (vTable);
        }
    } else {
        metadata->add(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
        if (!endOfStatement() && !isJoinType(currentToken().toUpperLexem()) && !isWhereGroupOrderWord(currentToken().toUpperLexem()) && (currentToken().getTokenType() != Token::TK_COMMA)){
            as_clause(true, false);
        }
    }
}

//// <from_clause> ::= FROM <table_reference_list>
void SelectExecutor::from_clause(){

    getParser()->match(getParser()->getCurrentToken());//Skip FROM
    table_reference_list();
}

//// <table_expression> ::= from_clause
////                      | where_clause
////                      | group_by_clause
////                      | having_clause
////                      | order_by_clause
void SelectExecutor::table_expression_list(){

    from_clause();

    // Optional WHERE clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "WHERE")){
        where_clause();
    }

    // Optional GROUP BY clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "GROUP")){
        groupby_clause();
        // Optional HAVING clause
        if ((!endOfStatement()) && (currentToken().toUpperLexem() == "HAVING")){
            having_clause();
        }
    }

    // Optional ORDER BY clause
    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "ORDER")){
        order_by_clause();
    }
}

//// <order_by_clause> ::=   ORDER BY ([<column_reference>,]* <column_reference>)
////                       | ORDER BY ([<column_reference>,]* <column_reference>) ASC
////                       | ORDER BY ([<column_reference>,]* <column_reference>) DESC
void SelectExecutor::order_by_clause(){

    getParser()->match(getParser()->getCurrentToken()); //skip order
    getParser()->match(getParser()->getCurrentToken()); //skip by
    column_reference();
    while ((!endOfStatement()) && currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        column_reference();
    }
    if ((!endOfStatement()) && ((currentToken().toUpperLexem() == "ASC") || (currentToken().toUpperLexem() == "DESC"))){
        getParser()->match(getParser()->getCurrentToken());
    }

}

//// <as_clause> ::= AS _identifier | _identifier_
void SelectExecutor::as_clause(bool fetchTableMetada, bool fetchPjMetadata){

    if (currentToken().toUpperLexem() == "AS"){
        getParser()->match(getParser()->getCurrentToken());
    }
    if (fetchTableMetada){
        metadata->addAliasToLastTable(currentToken().getLexem());
    }
    if (fetchPjMetadata){
        pjListMetadata->addAliasToLastTable(currentToken().getLexem());
    }
    getParser()->match(getParser()->getCurrentToken());
}

//// <typejoin_list> ::=  FULL OUTER JOIN | INNER JOIN | NATURAL JOIN | JOIN
///                     | LEFT JOIN  | LEFT OUTER JOIN
///                     | RIGHT JOIN | RIGHT OUTER JOIN
void SelectExecutor::typejoin_list(){

    if (currentToken().toUpperLexem() == "FULL"){
        getParser()->match(getParser()->getCurrentToken());//Skip full
        getParser()->match(getParser()->getCurrentToken());//Skip outer
    } else {
        if (currentToken().toUpperLexem() == "LEFT" ||
                currentToken().toUpperLexem() == "RIGHT"){
            getParser()->match(getParser()->getCurrentToken());//Skip left|right
        } else {
            getParser()->match(getParser()->getCurrentToken());//Skip inner|outer|natural
        }
    }
    getParser()->match(getParser()->getCurrentToken());//Skip JOIN
}

//// <on_clause_list> ::= _lambda_ | ON <predicate>
void SelectExecutor::on_clause_list(){

    if ((!endOfStatement()) && (currentToken().toUpperLexem() == "ON")){
        getParser()->match(getParser()->getCurrentToken());
        join_predicate();
    }
}

//// <column_reference> ::=   COUNT(*) | COUNT(_identifier_) | COUNT(_identifier_._identifier_)
///                         | MIN(*)   | MIN(_identifier_)   | MIN(_identifier_._identifier_)
///                         | MAX(*)   | MAX(_identifier_)   | MAX(_identifier_._identifier_)
///                         | SUM(*)   | SUM(_identifier_)   | SUM(_identifier_._identifier_)
///                         | AVG(*)   | AVG(_identifier_)   | AVG(_identifier_._identifier_)
///                         | _identifier_
///                         | _identifier_._identifier_
void SelectExecutor::column_reference(bool fetchPjMetadata){

    std::string att, table;

    if ((currentToken().toUpperLexem() == "COUNT") || (currentToken().toUpperLexem() == "MIN") || (currentToken().toUpperLexem() == "MAX") || (currentToken().toUpperLexem() == "SUM") || (currentToken().toUpperLexem() == "AVG")) {
        att = currentToken().getLexem() + " ( ";
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match("(", Token::TK_OPEN_BRACE);
        if (currentToken().getLexem() == "*"){
            getParser()->match(getParser()->getCurrentToken());
            att += "*";
        } else {
            table = currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
            if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
                getParser()->match(getParser()->getCurrentToken());
                att += currentToken().getLexem();
                getParser()->match(getParser()->getCurrentToken());
            } else {
                att += table;
                table.clear();
            }
        }
        att += " )";
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } else {
        table = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken());
        if ((!endOfStatement()) && (currentToken().getTokenType() == Token::TK_PERIOD)) {
            getParser()->match(getParser()->getCurrentToken());
            att += currentToken().getLexem();
            getParser()->match(getParser()->getCurrentToken());
        }  else {
            att += table;
            table.clear();
        }
    }

    if (fetchPjMetadata){
        if (table.empty()){
            pjListMetadata->add(att);
        } else {
            pjListMetadata->add(table, att);
        }
    }
}


////<column_reference_list> ::=    <column_reference>
///                              | <column_reference><as_clause>
///                              | <column_reference>, <column_reference_list>
///                              | <column_reference><as_clause>,<column_reference_list>
void SelectExecutor::column_reference_list(bool fetchPjMetadata){

    column_reference(fetchPjMetadata);

    if ((currentToken().toUpperLexem() != "FROM") && (currentToken().getTokenType() != Token::TK_COMMA)){
        as_clause(false, fetchPjMetadata);
    }

    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(",", Token::TK_COMMA);
        if (currentToken().toUpperLexem() != "FROM"){
            column_reference_list(fetchPjMetadata);
        }
    }
}

////<select_list()> ::=  * | <column_reference_list>
void SelectExecutor::select_list(){

    if ((currentToken().toUpperLexem() == "DISTINCT") || (currentToken().toUpperLexem() == "ALL")){
        getParser()->match(getParser()->getCurrentToken());
        column_reference_list(true);
    } else {
        if (currentToken().getLexem() == "*"){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            column_reference_list(true);
        }
    }
}

ComplexResultList SelectExecutor::executeKNN(IndexManager *idx, size_t *tokenInitialPosition, SimilaritySelectionAggregation aggPattern, SimilaritySelectionSearchType searchType, FeatureVector center, FeatureVectorList centers){


    ComplexResultList resultSet;
    bool tie = false;
    int k2;

    *tokenInitialPosition += 2;
    double k = atoi(getParser()->getToken(*tokenInitialPosition)->getLexem().c_str());
    k2 = k+1;
    *tokenInitialPosition += 1;

    if ((*tokenInitialPosition + 1 < getParser()->countTokens()) && (getParser()->getToken(*tokenInitialPosition)->toUpperLexem() == "BRIDGE")){
        k2 = atoi(getParser()->getToken(*tokenInitialPosition + 1)->getLexem().c_str()) + 1;
        *tokenInitialPosition += 2;
    }


    if ((*tokenInitialPosition < getParser()->countTokens()) && (getParser()->getToken(*tokenInitialPosition)->toUpperLexem() == "WITH")){
        tie = true;
        *tokenInitialPosition += 3;
    }

    switch (searchType){
    case DiversityNear:
    {
        if (centers.size()){
            switch (aggPattern){
            case MAX:
                resultSet = idx->nearestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->nearestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->nearestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->diverseNearestQuery(&center, k, 1);
            break;
        }
    }

    case DiversityFar:
    {
        if (centers.size()){
            switch (aggPattern){
            case MAX:
                resultSet = idx->farthestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->farthestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->farthestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->farthestSelectQuery(&center, k, tie);
            break;
        }
    }

    case DiversifiedNear:
    {
        if (centers.size()){
            switch (aggPattern){
            case MAX:
                resultSet = idx->nearestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->nearestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->nearestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->diverseNearestQuery(&center, k, k2);
            break;
        }
    }

    case DiversifiedFar:
    {
        if (centers.size()){
            switch (aggPattern){
            case MAX:
                resultSet = idx->farthestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->farthestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->farthestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->farthestSelectQuery(&center, k, tie);
            break;
        }
    }

    case Far:
    {
        if (centers.size()){
            switch (aggPattern){
            case MAX:
                resultSet = idx->farthestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->farthestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->farthestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->farthestSelectQuery(&center, k, tie);
            break;
        }
    }

    default:
    {
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                resultSet = idx->nearestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                resultSet = idx->nearestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                resultSet = idx->nearestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            resultSet = idx->nearestSelectQuery(&center, k, tie);
            break;
        }
    }
    }

    return resultSet;
}

ComplexResultList SelectExecutor::executeRange(IndexManager *idx, size_t *tokenInitialPosition, SimilaritySelectionAggregation aggPattern, SimilaritySelectionSearchType searchType, FeatureVector center, FeatureVectorList centers){

    ComplexResultList resultSet;

    *tokenInitialPosition += 1;
    double range = atof(getParser()->getToken(*tokenInitialPosition)->getLexem().c_str());
    *tokenInitialPosition += 1;

    switch (searchType){

    case DiversityNear:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                //@to-do
                //     resultSet = idx->nearestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                //@to-do
          //      resultSet = idx->nearestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                //@to-do
            //    resultSet = idx->nearestAllSelectQuery(centers, centers.size(), k, tie);
                break;
            };
            break;
        } else {
            //@to-do
            //resultSet = idx->diverseNearestQuery(&center, k);
            break;
        }

    case DiversityFar:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                //@to-do
               // resultSet = idx->farthestMaxSelectQuery(centers, centers.size(), k, tie);
                break;
            case SUM:
                //@to-do
               // resultSet = idx->farthestSumSelectQuery(centers, centers.size(), k, tie);
                break;
            case ALL:
                //@to-do
               // resultSet = idx->farthestAllSelectQuery(centers, centers.size(), k, tie);
                break;;
            };
            break;
        } else {
            //@to-do
            //resultSet = idx->farthestSelectQuery(&center, k, tie);
            break;
        }

    case DiversifiedNear:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                resultSet = idx->rangeMaxSelectQuery(centers, centers.size(), range);
                break;
            case SUM:
                resultSet = idx->rangeSumSelectQuery(centers, centers.size(), range);
                break;
            case ALL:
                resultSet = idx->rangeAllSelectQuery(centers, centers.size(), range);
                break;
            };
            break;
        } else {
            break;
        }

    case DiversifiedFar:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                resultSet = idx->reverseRangeMaxSelectQuery(centers, centers.size(), range);
                break;
            case SUM:
                resultSet = idx->reverseRangeSumSelectQuery(centers, centers.size(), range);
                break;
            case ALL:
                resultSet = idx->reverseRangeAllSelectQuery(centers, centers.size(), range);
                break;
            };
            break;
        } else {
            break;
        }

    case Far:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                resultSet = idx->reverseRangeMaxSelectQuery(centers, centers.size(), range);
                break;
            case SUM:
                resultSet = idx->reverseRangeSumSelectQuery(centers, centers.size(), range);
                break;
            case ALL:
                resultSet = idx->reverseRangeAllSelectQuery(centers, centers.size(), range);
                break;
            };
            break;
        } else {
            break;
        }

    default:
        if (centers.size() > 0){
            switch (aggPattern){
            case MAX:
                resultSet = idx->rangeMaxSelectQuery(centers, centers.size(), range);
                break;
            case SUM:
                resultSet = idx->rangeSumSelectQuery(centers, centers.size(), range);
                break;
            case ALL:
                resultSet = idx->rangeAllSelectQuery(centers, centers.size(), range);
                break;
            };
            break;
        } else {
            resultSet = idx->rangeSelectQuery(&center, range);
            break;
        }
    }

    return resultSet;
}

void SelectExecutor::updateTokenList(size_t *begin, size_t *currentPosition, std::string tableAlias, std::string attribute, std::vector<std::string> rowIds, SimilaritySelectionSearchType searchType, bool vTable){

    Token *t = nullptr;
    std::vector<Token *> tList;


    for (size_t k = *begin; k < *currentPosition; k++){
        getParser()->removeToken(*begin);
    }

    if (searchType == SimilaritySelectionSearchType::DiversifiedNear || searchType == SimilaritySelectionSearchType::DiversifiedFar){
        if (vTable){
            t = new Token("(" + tableAlias + "." +attribute + "_id, " + tableAlias + "$bridged." + attribute + "_id)" , Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        } else {
            t = new Token("(" + tableAlias + "." + attribute + ", " + tableAlias + "$bridged." + attribute + ")", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        }
    } else {
        t = new Token(tableAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        tList.push_back(t);
        t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
        tList.push_back(t);
        if (vTable){
            t = new Token(attribute + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        } else {
            t = new Token(attribute, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
        }
    }
    tList.push_back(t);
    t = new Token("IN", Token::TK_IN, Lexical::LK_UNDEFINED);
    tList.push_back(t);
    t = new Token("(", Token::TK_OPEN_BRACE, Lexical::LK_UNDEFINED);
    tList.push_back(t);
    for (size_t k = 0; k < rowIds.size(); k++){
        if (k > 0){
            t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
            tList.push_back(t);
        }
        t = new Token(rowIds[k], Token::TK_INTEGER, Lexical::LK_UNDEFINED);
        tList.push_back(t);
    }
    t = new Token(")", Token::TK_CLOSE_BRACE, Lexical::LK_UNDEFINED);
    tList.push_back(t);

    getParser()->insertTokenList(tList, *begin);
    *currentPosition += tList.size();

    tList.clear();
}

void SelectExecutor::transformSimilarityIntoRegularSQL(){

    //Search operator (similarity)
    SimilaritySelectionSearchType searchType;
    //Get table name, alias and atribute
    std::string tableName, tableAlias, attributeName, attributeAlias, caTable, cAttribute;
    //Internal attribute/subselect
    std::string internalAttribute;
    //Parenthesis matching numbers
    size_t matchParenthesis;
    //Temporary sql statement for a vTable result set
    std::string tSql;
    //Temp result set
    QMap<QString, QStringList>* subQuery = nullptr;
    //Aggregation type (multiple centers)
    SimilaritySelectionAggregation aggPattern = SimilaritySelectionAggregation::MAX;
    //Query center
    FeatureVector centerQuery;
    //Multiple query centers
    FeatureVectorList centers;
    //Marks a virtual table
    bool vTable;
    //Metric info
    std::string metricName, metricCode, distanceCode;
    //Index handler
    IndexManager *idx = nullptr;
    //Distance function (wrapped)
    MetricDistanceFunction *eval = nullptr;
    //Index file name
    std::string idxName;
    //Result set (for vTables)
    ComplexResultList resultSet;
    //Row id list
    std::vector<std::string> rowIds;
    //Keep track of token with similarity predicate
    size_t pos = 0;
    //Scan token list
    size_t x = 0;


    try{
        while (getParser()->getToken(x)->toUpperLexem() != "WHERE"){
            x++;
        }
        x++;

        while (getParser()->getToken(x)->toUpperLexem() != "GROUP" ||
               getParser()->getToken(x)->toUpperLexem() != "ORDER"){

            while ((getParser()->getToken(x)->toUpperLexem() != "DIVERSITY") &&
                   (getParser()->getToken(x)->toUpperLexem() != "DIVERSIFIED") &&
                   (getParser()->getToken(x)->toUpperLexem() != "NEAR") &&
                   (getParser()->getToken(x)->toUpperLexem() != "FAR")){

                if (getParser()->getToken(x)->getTokenType() == Token::TK_PERIOD){
                    tableAlias = getParser()->getToken(x-1)->getLexem();
                    pos = x - 1;
                    x++;
                    attributeAlias = getParser()->getToken(x)->getLexem();
                    x++;
                } else {
                    pos = x;
                    attributeAlias = getParser()->getToken(x)->getLexem();
                    x++;
                }
            }
            if (tableAlias.empty()){
                tableAlias = discoverTableName(attributeAlias);
            }

            //Load table related to the similarity-selection
            size_t location = locateAttribute(tableAlias, attributeAlias);
            tableName = metadata->tables()[location];
            attributeName = metadata->attributes()[location];
            caTable = metadata->caTables()[location];
            cAttribute = metadata->cAttributes()[location];

            //Identifies sim operator
            if ((getParser()->getToken(x)->toUpperLexem() == "NEAR") ||
                (getParser()->getToken(x)->toUpperLexem() == "FAR") ||
                (getParser()->getToken(x)->toUpperLexem() == "DIVERSIFIED") ||
                (getParser()->getToken(x)->toUpperLexem() == "DIVERSITY")){

                if (getParser()->getToken(x)->toUpperLexem() == "DIVERSIFIED"){
                    x++;
                    if (getParser()->getToken(x)->toUpperLexem() == "NEAR"){
                        searchType = SimilaritySelectionSearchType::DiversifiedNear;
                    } else {
                        searchType = SimilaritySelectionSearchType::DiversifiedFar;
                    }
                } else {
                    if (getParser()->getToken(x)->toUpperLexem() == "DIVERSITY"){
                        x++;
                        if (getParser()->getToken(x)->toUpperLexem() == "NEAR"){
                            searchType = SimilaritySelectionSearchType::DiversityNear;
                        } else {
                            searchType = SimilaritySelectionSearchType::DiversityFar;
                        }
                    } else {
                        if (getParser()->getToken(x)->toUpperLexem() == "NEAR"){
                            searchType = SimilaritySelectionSearchType::Near;
                        } else {
                            searchType = SimilaritySelectionSearchType::Far;
                        }
                    }
                }
                x++;

                //--- Start - Loading query center(s) ---
                //If there is an aggreagation condition, so we do expect multiple centers
                if (isAggregationType(getParser()->getToken(x)->toUpperLexem())){
                    if (getParser()->getToken(x)->toUpperLexem() == "SUM"){
                        aggPattern = SimilaritySelectionAggregation::SUM;
                    } else {
                        if (getParser()->getToken(x)->toUpperLexem() == "ALL"){
                            aggPattern = SimilaritySelectionAggregation::ALL;
                        } else {
                            aggPattern = SimilaritySelectionAggregation::MAX;
                        }
                    }

                    matchParenthesis = 0;

                    x++;//Skip <aggregation_condition> token
                    x++;//Skip Token::TK_OPEN_BRACE

                    //Matches internal attribute or subselect
                    matchParenthesis++;
                    while (matchParenthesis > 0){
                        if (getParser()->getToken(x)->getLexem() == "("){
                            matchParenthesis++;
                        }
                        if (getParser()->getToken(x)->getLexem() == ")"){
                            matchParenthesis--;
                        }
                        if (matchParenthesis> 0){
                            internalAttribute.append(getParser()->getToken(x)->getLexem());
                            internalAttribute.append(" ");
                        }
                        x++;
                    }

                    //Build multiple centers
                    if (metadata->isVirtual(internalAttribute)){
                        //Define vTable for multiple centers
                        tSql = internalAttribute;
                        if (!metadata->alreadyTranslatedStatement(internalAttribute)){
                            SelectExecutor *tSel = new SelectExecutor(tSql, dbManager());
                            tSql = tSel->translateToRegularSQL(true);
                            delete (tSel);
                        }
                    } else {
                        QStringList tablA = QString(internalAttribute.c_str()).split(".");
                        size_t locate;
                        if (tablA.size() > 1){
                            locate = locateAttribute(tablA.at(0).simplified().toStdString(), tablA.at(1).simplified().toStdString());
                        } else {
                            locate = locateAttribute(metadata->aliasesOfTables()[metadata->findFirstPosition(internalAttribute)], internalAttribute);
                        }
                        std::string hiddenJoinTableName  = dictionary()->tablePrefix(
                                    dictionary()->columnDataType(metadata->tables()[locate].c_str(), metadata->attributes()[locate].c_str())).toStdString()
                                    + "$" + metadata->tables()[locate] + "_" + metadata->attributes()[locate];
                        tSql = "SELECT " + hiddenJoinTableName + "." + metadata->attributes()[locate] + " AS " + metadata->attributes()[locate] + " FROM ( " + metadata->tables()[locate] + " ) JOIN " + hiddenJoinTableName + " ON " + metadata->tables()[locate] + "." + metadata->attributes()[locate] + " = " + hiddenJoinTableName + "." + metadata->attributes()[locate] + "_id";
                    }
                    subQuery = dbManager()->runSelect(tSql.c_str());
                    for (int long x = 0; x < subQuery->size(); x++){
                        FeatureVector fv;
                        fv.unserializeFromString(FeatureVector::fromBase64(subQuery->constBegin().value().at(x).toStdString()));
                        centers.push_back(fv);
                    }
                    if (subQuery != nullptr){
                        delete (subQuery);
                    }
                    tSql.clear();
                    internalAttribute.clear();
                } else {
                    //Only one query center is expected. Load it.
                    x++;//Skip {
                    while (getParser()->getToken(x)->getTokenType() != Token::TK_CLOSE_BLOCK){
                        centerQuery.add(QString(getParser()->getToken(x)->getLexem().c_str()).toFloat());
                        x++;
                        if (getParser()->getToken(x)->getTokenType() == Token::TK_COMMA){
                            x++; //Skip,
                        }
                    }
                    x++;//Skip }
                }
                //--- End - Loading query center(s) ---

                //The compared attribute comes from a virtual table?
                vTable = isVirtualComplexAttribute(tableAlias, attributeAlias);

                //--- Start - Finding metric code ---
                if (getParser()->getToken(x)->toUpperLexem() == "BY"){
                    x++;
                    metricName = getParser()->getToken(x)->getLexem();
                    x++;
                } else {
                    if (!vTable){
                        metricName = dictionary()->getDefaultMetric(tableName.c_str(), attributeName.c_str()).toStdString();
                    } else {
                        metricName = dictionary()->getDefaultMetric(caTable.c_str(), cAttribute.c_str()).toStdString();
                    }
                }
                //--- End - Finding metric code ---


                //--- Start - Build index ---
                metricCode = dictionary()->getMetricCode(metricName.c_str()).toStdString();
                if (!vTable){
                    distanceCode = dictionary()->getDistanceFunction(tableName.c_str(), attributeName.c_str(), metricName.c_str()).toStdString();
                    idxName = dictionary()->getIndexFile(tableName.c_str(), attributeName.c_str(), metricCode.c_str()).toStdString();
                } else {
                    distanceCode = dictionary()->getDistanceFunction(caTable.c_str(), cAttribute.c_str(), metricName.c_str()).toStdString();
                    //-- Begin -- It may generate a race condition for two parallel queries with same idxName - @todo - Properly fix with mutexes - Future.
                    idxName = "temp_" + QString::number(qrand()).toStdString() + "_" + QDateTime::currentDateTime().toString("dd.MM.yyyy").toStdString() + "_" + tableAlias + "_" + attributeAlias + "_" + QString::number(x).toStdString();
                    //-- End --
                }

                //--- Abort execution to avoid fatal error (Arboretum-related)
                if (distanceCode.empty()){
                    throw new std::runtime_error("Invalid metric code.");
                }
                //--- Abort execution to avoid fatal error (Arboretum-related


                eval = new MetricDistanceFunction(QString::fromStdString(distanceCode).toInt());
                idx = new IndexManager(idxName, *eval, vTable, IndexManager::PAGE_SIZE, true);

                if (vTable){
                    tSql = "SELECT " + tableAlias + "." + attributeAlias + " FROM ( " + tableName + " ) AS " + tableAlias;// + " JOIN " + hiddenJoinTableName + " ON " + tableAlias + "." + attribute + " = " + hiddenJoinTableName + "." + attribute + "_id";

                    SelectExecutor *tSel = new SelectExecutor(tSql, dbManager());
                    tSql = tSel->translateToRegularSQL(true);
                    delete (tSel);
                    subQuery = dbManager()->runSelect(tSql.c_str());

                    populateSequentialScan(idx, subQuery);

                    if (subQuery!= nullptr){
                        delete (subQuery);
                    }
                }
                //--- End - Build index ---

                //--- Start - processing the similarity query ---
                if (getParser()->getToken(x)->toUpperLexem() == "STOP"){// knn
                    resultSet = executeKNN(idx, &x, aggPattern, searchType, centerQuery, centers);
                } else { // range
                    resultSet = executeRange(idx, &x, aggPattern, searchType, centerQuery, centers);
                }
                //--- End - processing the similarity query ---

                //--- Start - Replace row ids by an IN clause ---
                if (resultSet.size()){
                    if (searchType == SimilaritySelectionSearchType::Near || searchType == SimilaritySelectionSearchType::Far){
                        for (size_t m = 0; m < resultSet[0]->GetNumOfEntries(); m++){
                            FeatureVector *qq = (FeatureVector *)  ((*resultSet[0])[m].GetObject());
                            rowIds.push_back(QString::number(qq->getOID()).toStdString());
                        }
                    } else {
                        if (searchType == SimilaritySelectionSearchType::DiversityNear || searchType == SimilaritySelectionSearchType::DiversityFar){
                            for (size_t rX = 0; rX < resultSet.size(); rX++){
                                ComplexResult* tmpResult = resultSet[rX];
                                FeatureVector *qq = (FeatureVector *)  ((*tmpResult)[0].GetObject());
                                rowIds.push_back(QString::number(qq->getOID()).toStdString());
                            }
                        } else {
                            //---- Start - Adding cartesian product for bringing bridged attributes (...$bridged)
                            size_t xAdd = getParser()->countTokens() - 1;
                            size_t firstXAdd = 0;

                            //Stack mode
                            //---- Start - Locate insert position (token list)
                            while (xAdd){
                                if (getParser()->getToken(xAdd)->toUpperLexem() == "ORDER"){
                                    firstXAdd = xAdd;
                                }
                                if (getParser()->getToken(xAdd)->toUpperLexem() == "GROUP"){
                                    firstXAdd = xAdd;
                                }
                                if (getParser()->getToken(xAdd)->toUpperLexem() == "WHERE"){
                                    firstXAdd = xAdd;
                                    break;
                                }
                                xAdd--;
                            }

                            if (!firstXAdd){
                                firstXAdd = getParser()->countTokens() - 1;
                            }
                            xAdd = firstXAdd;
                            //---- End - Locate insert position (token list)

                            std::vector<Token *> tListAdd;
                            Token *t;
                            t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            if (vTable){
                                SelectExecutor *tSel = new SelectExecutor(tableName, dbManager());
                                Parser ap(tSel->translateToRegularSQL(true));
                                delete (tSel);
                                t = new Token("(", Token::TK_OPEN_BRACE, Lexical::LK_UNDEFINED);
                                tListAdd.push_back(t);
                                for (size_t tx = 0; tx < ap.countTokens(); tx++){
                                    t = new Token(ap.getToken(tx)->getLexem(), ap.getToken(tx)->getTokenType(), Lexical::LK_UNDEFINED);
                                    tListAdd.push_back(t);
                                }
                                t = new Token(")", Token::TK_CLOSE_BRACE, Lexical::LK_UNDEFINED);
                                tListAdd.push_back(t);
                            } else {
                                t = new Token(caTable, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                                tListAdd.push_back(t);
                            }
                            t = new Token("AS", Token::TK_UNDEFINED, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token(tableAlias + "$bridged", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);

                            getParser()->insertTokenList(tListAdd, xAdd);
                            pos += tListAdd.size();
                            x += tListAdd.size();
                            //---- End - Adding cartesian product for bringing bridged attributes (...$bridged)

                            //Adding cartesian product and bridged attribute in the join and projection token list
                            tListAdd.clear();
                            if (!vTable){
                                xAdd = getParser()->countTokens() - 1;
                            } else {
                                xAdd = 0;
                                while (xAdd < getParser()->countTokens() && getParser()->getToken(xAdd)->getLexem() != "("){
                                    xAdd++;
                                }
                            }
                            while (xAdd > 1 && getParser()->getToken(xAdd)->toUpperLexem() != "FROM"){
                                xAdd--;
                            }
                            t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token(tableAlias + "$bridged", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token(cAttribute, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token("AS", Token::TK_UNDEFINED, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            t = new Token(attributeAlias + "$bridged", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                            tListAdd.push_back(t);
                            getParser()->insertTokenList(tListAdd, xAdd);
                            pos += tListAdd.size();
                            x += tListAdd.size();

                            //Update metadata
                            metadata->add(caTable, tableAlias + "$bridged", cAttribute, attributeAlias + "$bridged", caTable, cAttribute);

                            //Create (double) IN clause
                            for (size_t rX = 0; rX < resultSet.size(); rX++){
                                ComplexResult* tmpResult = resultSet[rX];
                                for (size_t rY = 0; rY < tmpResult->GetNumOfEntries(); rY++){
                                    FeatureVector *qq = (FeatureVector *)  ((*tmpResult)[0].GetObject());
                                    FeatureVector *qp = (FeatureVector *)  ((*tmpResult)[rY].GetObject());
                                    rowIds.push_back(" (" + QString::number(qq->getOID()).toStdString() + "," + QString::number(qp->getOID()).toStdString() +") ");
                                }
                            }
                        }

                    }
                }

                updateTokenList(&pos, &x, tableAlias, attributeAlias, rowIds, searchType, vTable);
                if ((vTable) && (idx != nullptr)){
                    idx->dropDummyTree();
                }
                //--- End - Replace row ids by an IN clause ---

            }
            rowIds.clear();
            centers.clear();
            idxName.clear();

            for (size_t x = 0; x < resultSet.size(); x++){
                if (resultSet[x] != nullptr){
                    delete resultSet[x];
                    resultSet[x] = nullptr;
                }
            }
            resultSet.clear();

            if (eval != nullptr){
                delete (eval);
            }
            if (idx != nullptr){
                delete (idx);
            }

        }
    } catch (...) {
    }
}

void SelectExecutor::transformAsteriskIntoColumnList(){

    std::string tableName, tableAlias, columnAlias;
    QStringList columns;
    QueryMetaTree *auxMetadata;
    size_t it;

    int pos = 1;

    if (getParser()->getToken(1)->getLexem() == "*"){

        auxMetadata = new QueryMetaTree();
        it = metadata->size();
        for (size_t x = 0; x < it; x++){
            if (metadata->aliasesOfAttributes()[x].size() > 0){
                    auxMetadata->add(metadata->tables()[x],
                                     metadata->aliasesOfTables()[x],
                                     metadata->attributes()[x],
                                     metadata->aliasesOfAttributes()[x],
                                     metadata->caTables()[x],
                                     metadata->cAttributes()[x]);
            }
        }


        while (getParser()->getToken(pos)->toUpperLexem() != "FROM"){
            getParser()->removeToken(pos);
        }

        //Rewrite the query by replacing * to the real columns name
        Token *t;
        std::vector<Token *> tList;

        for (size_t x = 0; x < auxMetadata->size(); x++){
            if (x > 0){
                t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                tList.push_back(t);
            }

            tableAlias = auxMetadata->aliasesOfTables()[x];
            columnAlias = auxMetadata->aliasesOfAttributes()[x];

            t = new Token(tableAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
            tList.push_back(t);
            t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
            tList.push_back(t);
            t = new Token(columnAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
            tList.push_back(t);

            pjListMetadata->add(tableAlias, auxMetadata->attributes()[x], columnAlias);
        }

        getParser()->insertTokenList(tList, 1);
        tList.clear();

        if (metadata != nullptr){
            delete (metadata);
        }
        metadata = new QueryMetaTree(*auxMetadata);
        if (auxMetadata != nullptr){
            delete (auxMetadata);
        }
    }
}


size_t SelectExecutor::locateVirtualComplexAttribute(std::string aliasTableName, std::string aliasColumnName){

    size_t answer = 0;

    if (!isVirtualComplexAttribute(aliasTableName, aliasColumnName)){
        throw new std::exception();
    } else {
        for (size_t x = 0; (x < metadata->size() && !answer); x++){
            if (metadata->aliasesOfTables()[x] == aliasTableName
                    && metadata->aliasesOfAttributes()[x] == aliasColumnName
                    && metadata->isVirtual(x))
                answer = x;
        }
    }

    return answer;
}

size_t SelectExecutor::locateAttribute(std::string aliasTableName, std::string aliasColumnName){

    size_t answer = 0;

    for (size_t x = 0; (x < metadata->size() && (!answer)); x++){
        if (metadata->aliasesOfTables()[x] == aliasTableName
                && metadata->aliasesOfAttributes()[x] == aliasColumnName)
            answer = x;
    }

    return answer;
}

bool SelectExecutor::isVirtualComplexAttribute(std::string aliasTableName, std::string aliasColumnName){

    bool answer = false;

    for (size_t x = 0; (x < metadata->size() && (!answer)); x++){
        answer = metadata->aliasesOfTables()[x] == aliasTableName
                && metadata->aliasesOfAttributes()[x] == aliasColumnName
                && metadata->isVirtual(x);
    }

    return answer;
}

std::string SelectExecutor::discoverTableName(std::string columnName){

    std::string answer, table;

    for (size_t x = 0; x < metadata->size(); x++){
        if (columnName == metadata->aliasesOfAttributes()[x]){
            answer = metadata->aliasesOfTables()[x];
        }
    }
    return answer;
}

void SelectExecutor::transformSimilarityIntoRegularAttribute(bool isSubselect){

    //Global iterator token list
    int x = 1;
    //Local iterator token list
    int y = 0;
    //Position of the table containing a similarity attribute
    int pos = 0;
    //Nome e alias de tabela e coluna
    std::string tableName, tableAlias, attributeName, attributeAlias;
    bool newAlias;

    Token *t;
    std::vector<Token *> tList;

    while (getParser()->getToken(x)->toUpperLexem() != "FROM"){
        if ((getParser()->getToken(x+1)->getTokenType() == Token::TK_COMMA) || (getParser()->getToken(x+1)->toUpperLexem() == "FROM")){
            //Clear attribute values
            attributeAlias = attributeName = tableName = tableAlias = "";
            y = x;
            newAlias = false;

            //Fetch attribute alias
            //Caso SELECT <att> FROM <tbl>
            attributeName = getParser()->getToken(y)->getLexem();
            attributeAlias = attributeName;
            pos = y;
            y--;

            //Avoid the basic case with a single and unrenamed attribute in the projection list
            //Fetch attribute name and table aliases
            if (y > 0 && (getParser()->getToken(y)->getTokenType() != Token::TK_COMMA)){
                //Caso SELECT <tbl.att> FROM <tbl>
                if (getParser()->getToken(y)->getTokenType() == Token::TK_PERIOD){
                    //attributeAlias = attributeName;
                    tableAlias = getParser()->getToken(y-1)->getLexem();
                    pos = y-1;
                } else {
                    //Caso 1 SELECT <tbl.att> AS <att_alias> FROM <tbl>
                    //Caso 2 SELECT <tbl.att> <att_alias> FROM <tbl>
                    //Caso 3 SELECT <att> <att_alias> FROM <tbl>
                    //Caso 3 SELECT <att> AS <att_alias> FROM <tbl>
                    newAlias = true;
                    if (getParser()->getToken(y)->toUpperLexem() == "AS"){
                        y--;
                    }
                    attributeAlias = attributeName;
                    attributeName = getParser()->getToken(y)->getLexem();
                    pos = y;
                    y--;
                    if ((y > 0) && (getParser()->getToken(y)->getTokenType() == Token::TK_PERIOD)){
                        tableAlias = getParser()->getToken(y-1)->getLexem();
                        pos = y-1;
                    }
                }
            }
            if (tableAlias.empty()){
                tableAlias = discoverTableName(attributeAlias);
            }

            //Recover the real name of pair <table, attribute>
            size_t location;
            if (newAlias){
                location = locateAttribute(tableAlias, attributeName);
                metadata->modifyAttributeAlias(attributeAlias, location);
            } else {
                location = locateAttribute(tableAlias, attributeAlias);
                attributeName = metadata->attributes()[location];
            }

            //Update metadata values
            tableName = metadata->tables()[location];

            if ((!isVirtualComplexAttribute(tableAlias, attributeAlias)) && dictionary()->isComplexAttributeReference(tableName.c_str(), attributeName.c_str())){

                for (int m = 0; m <= (x - pos); m++){
                    getParser()->removeToken(pos);
                }
                x = x - (x-pos);

                t = new Token(tableAlias+dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeName.c_str())).toStdString() + "$" + tableName + "_" + attributeName, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(attributeName, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                t = new Token(attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (isSubselect){
                    t = new Token(",", Token::TK_COMMA, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(tableAlias+dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeName.c_str())).toStdString() + "$" + tableName + "_" + attributeName, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(attributeName + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(attributeAlias + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }

                getParser()->insertTokenList(tList, pos);
                x = x - 1 + tList.size();
                tList.clear();

                joinHiddenTableOfSimilarityAttribute(tableName, tableAlias, attributeName);
            }
        }
        x++;
    }
}

void SelectExecutor::joinHiddenTableOfSimilarityAttribute(std::string tableName, std::string tableAlias, std::string attributeAlias){

    int x = 0;
    bool found = false;
    Token *t;
    std::vector<Token *> tList;

    while (getParser()->getToken(x)->toUpperLexem() != "FROM"){
        x++;
    }
    x++;

    while (!found){

        if (getParser()->getToken(x)->getLexem() == tableName){
            int pos = x;
            if (tableAlias != tableName){
                if ((getParser()->countTokens() > (x+2)) && getParser()->getToken(x+1)->toUpperLexem() == "AS"){
                    pos = x+2;
                } else {
                    pos = x+1;
                }
                if (getParser()->getToken(pos)->getLexem() == tableAlias){
                    found = true;
                }
            } else {
                if ((getParser()->countTokens() > (x+2)) && getParser()->getToken(x+1)->toUpperLexem() == "AS"){
                    if (getParser()->getToken(x+2)->getLexem() == tableAlias){
                        pos = x+2;
                    }
                } else {
                    if ((getParser()->countTokens() > (x+1)) && getParser()->getToken(x+1)->getLexem() == tableAlias){
                        pos = x+1;
                    }
                }
                found = true;
            }

            if (found){
                getParser()->removeToken(x);
                for (int k = x; k < pos; k++){
                    getParser()->removeToken(x);
                }

                t = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (!tableAlias.empty()){
                    t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(tableAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }

                t = new Token("JOIN", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);



                t = new Token(dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeAlias.c_str())).toStdString() + "$" + tableName + "_" + attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                //bb
                if (!tableAlias.empty()){
                    t = new Token("AS", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                    t = new Token(tableAlias + dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeAlias.c_str())).toStdString() + "$" + tableName + "_" + attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }
                //bb

                t = new Token("ON", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (!tableAlias.empty()){
                    t = new Token(tableAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                } else {
                    t = new Token(tableName, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                    tList.push_back(t);
                }

                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                if (isVirtualComplexAttribute(tableAlias, attributeAlias)){
                    t = new Token(attributeAlias + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                } else {
                    t = new Token(attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                }
                tList.push_back(t);

                t = new Token("=", Token::TK_EQUAL, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                //bb
                if (!tableAlias.empty()){
                    t = new Token(tableAlias + dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeAlias.c_str())).toStdString() + "$" + tableName + "_" + attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                } else {
                //bb
                t = new Token(dictionary()->tablePrefix(dictionary()->columnDataType(tableName.c_str(), attributeAlias.c_str())).toStdString() + "$" + tableName + "_" + attributeAlias, Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);
                }

                t = new Token(".", Token::TK_PERIOD, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                t = new Token(attributeAlias + "_id", Token::TK_IDENTIFIER, Lexical::LK_UNDEFINED);
                tList.push_back(t);

                getParser()->insertTokenList(tList, x);
                tList.clear();
            }
        }
        x++;
    }
}

std::string SelectExecutor::translateToRegularSQL(bool isSubselect){

    std::string aux;

    destroyAuxiliaryStructures();
    metadata = new QueryMetaTree();
    pjListMetadata = new ProjectionListMetaInfo();

    getParser()->match(getParser()->getCurrentToken());
    select_list();
    table_expression_list();
    buildFullInfoQueryMetaTree();
    renameMetadataAttributes();

    transformSimilarityIntoRegularSQL();

    transformAsteriskIntoColumnList();
    transformSimilarityIntoRegularAttribute(isSubselect);
    buildInfoQueryMetaTree();

    aux.clear();
    for (size_t x = 0; x < getParser()->countTokens(); x++){
        if (aux.size() > 0)
            aux += " ";
        aux += getParser()->getToken(x)->getLexem();
    }

    return aux;
}

std::vector<std::string> SelectExecutor::translate(std::string sqlStatement){

    std::vector<std::string> commands;

    if (sqlStatement.size() > 0){
        getParser()->parse(sqlStatement);
    }


    std::string aux = translateToRegularSQL();
    commands.push_back(aux);

    getParser()->resetParser();



    return commands;
}

