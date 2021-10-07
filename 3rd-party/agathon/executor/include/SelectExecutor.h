#ifndef SELECTEXECUTOR_HPP
#define SELECTEXECUTOR_HPP

#include <SQLExecutor.h>
#include <IndexManager.h>
#include <SelectSemanticStructs.h>
#include <QDateTime>

class SelectExecutor : public SQLExecutor{

    private:
        QueryMetaTree *metadata;
        ProjectionListMetaInfo *pjListMetadata;
        std::vector<std::string> joinTypes = {"JOIN", "INNER", "LEFT", "RIGHT", "FULL", "NATURAL", "CLOSEST", "NEAREST", "RANGE", "ON"};
        std::vector<std::string> predicateTypes = { "=", "!=", "LIKE", "<>", "<", "<=", ">", ">=", "NOT", "IN", "BETWEEN", "DIVERSIFIED", "NEAR", "FAR"};
        std::vector<std::string> aggregationTypes = { "MAX", "SUM", "ALL"};

    public:
        enum SimilaritySelectionAggregation {MAX, SUM, ALL};
        enum SimilaritySelectionSearchType {DiversityNear, DiversityFar, DiversifiedNear, DiversifiedFar, Near, Far};

    private:

        void destroyAuxiliaryStructures();
        std::string fetchSubselectStatement();
        SelectExecutor* reshapeVirtualTable();
        std::string reshapeSelectBasedWhereCondition();

        bool isWhereGroupOrderWord(std::string word);
        bool isJoinType(std::string word);
        bool isPredicateType(std::string word);
        bool isAggregationType(std::string word);
        void populateSequentialScan(IndexManager *idx, QMap<QString, QStringList>* tempResultSet) throw (std::exception *);
        void buildFullInfoQueryMetaTree();
        void buildInfoQueryMetaTree();
        void renameMetadataAttributes();
        QueryMetaTree fetchMetadata();


        //Projection-related methods
        void select_list();
        void column_reference_list(bool fetchPjMetadata = false);
        void column_reference(bool fetchPjMetadata = false);
        void as_clause(bool fetchTableMetada = false, bool fetchPjMetadata = false);

        //Shared predicates selection-join
        void regular_predicate();
        void value_expression();
        void in_clause();
        void between_clause();

        //Join-related methods
        void table_expression_list();
        void from_clause();
        void table_reference_list();
        void read_table();
        void typejoin_list();
        void on_clause_list();
        void join_predicate();


        //Selection-related methods
        void where_clause();
        void search_condition();
        void predicate();
        void similarity_predicate();
        void query_obj_definition();

        //Group-by related clauses
        void groupby_clause();
        void having_clause();

        //Order-by related clauses
        void order_by_clause();

        //Translated Methods
        void transformSimilarityIntoRegularSQL();
        void transformAsteriskIntoColumnList();
        void transformSimilarityIntoRegularAttribute(bool isSubselect = false);
        void joinHiddenTableOfSimilarityAttribute(std::string tableName, std::string tableAlias, std::string attributeAlias);

        std::string discoverTableName(std::string columnName);
        size_t locateVirtualComplexAttribute(std::string aliasTableName, std::string aliasColumnName);
        size_t locateAttribute(std::string aliasTableName, std::string aliasColumnName);
        bool isVirtualComplexAttribute(std::string aliasTableName, std::string aliasColumnName);

        ComplexResultList executeKNN(IndexManager *idx, size_t *tokenInitialPosition, SimilaritySelectionAggregation aggPattern, SimilaritySelectionSearchType searchType, FeatureVector center, FeatureVectorList centers);
        ComplexResultList executeRange(IndexManager *idx, size_t *tokenInitialPosition, SimilaritySelectionAggregation aggPattern, SimilaritySelectionSearchType searchType, FeatureVector center, FeatureVectorList centers);
        void updateTokenList(size_t *begin, size_t *currentPosition, std::string tableAlias, std::string attribute, std::vector<std::string> rowIds, SimilaritySelectionSearchType searchType = SimilaritySelectionSearchType::Near, bool vTable = false);

    public:

        SelectExecutor(DatabaseManager *dbmg = nullptr) : SQLExecutor(dbmg){
            metadata = new QueryMetaTree();
            pjListMetadata = new ProjectionListMetaInfo();
        }

        SelectExecutor(std::string sirenSqlStatement, DatabaseManager *dbmg) : SQLExecutor(sirenSqlStatement, dbmg){
            metadata = new QueryMetaTree();
            pjListMetadata = new ProjectionListMetaInfo();
        }

        virtual ~SelectExecutor();

        std::vector<std::string> translate(std::string sqlStatement);
        std::string translateToRegularSQL(bool isSubselect = false);
};

#endif // SELECTEXECUTOR_HPP
