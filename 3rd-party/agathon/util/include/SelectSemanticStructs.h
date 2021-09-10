#ifndef SELECTSEMANTICSTRUCTS_HPP
#define SELECTSEMANTICSTRUCTS_HPP

#include <DataDictionary.h>


class QueryMetaTree{

    private:
        std::vector<std::string> tableNames;
        std::vector<std::string> tableAliases;
        std::vector<std::string> attributeNames;
        std::vector<std::string> attributeAliases;
        std::vector<std::string> originalCATables;
        std::vector<std::string> originalCAs;

    public:
        QueryMetaTree();
        ~QueryMetaTree();

        void add(std::string tableName, std::string tableAlias, std::string attributeName, std::string attributeAlias, std::string originalCATable = "", std::string originalCA = "");
        void add(std::string tableName, std::string tableAlias, std::string attributeName);
        void add(std::string tableName);
        void addAliasToLastTable(std::string tableAlias);
        void modifyAttributeAlias(std::string attributeAlias, size_t pos);

        std::vector<std::string> tables();
        std::vector<std::string> aliasesOfTables();

        std::vector<std::string> attributes();
        std::vector<std::string> aliasesOfAttributes();

        size_t size();

        std::vector<std::string> caTables();
        std::vector<std::string> cAttributes();

        bool isVirtual(std::string tableName);
        bool alreadyTranslatedStatement(std::string tableName);
        bool isVirtual(size_t pos);
        bool isSimilarityAttribute(size_t pos);

        bool exists(std::string tableAlias);

        size_t findFirst(std::string tableAlias);
        size_t findFirstPosition(std::string attributeAlias);

        size_t matchAttribute(std::string attributeName);
        size_t matchAttribute(std::string attributeName, std::string tableOrAttributeAlias, bool attNameTblAlias = true);
        size_t matchAttribute(std::string attributeName, std::string attributeAlias, std::string tableAlias);

        void print();
};

class ProjectionListMetaInfo{

    private:
        std::vector<std::string> tableAliases;
        std::vector<std::string> attributeNames;
        std::vector<std::string> attributeAliases;

    public:
        ProjectionListMetaInfo();
        ~ProjectionListMetaInfo();

        void add(std::string tableAlias, std::string attributeName, std::string attributeAlias);
        void add(std::string tableAlias, std::string attributeName);
        void add(std::string attributeName);
        void addAliasToLastTable(std::string attributeAlias);
        void addAliasToLastTable(std::string tableAlias, std::string attributeAlias);

        std::vector<std::string> aliasesOfTables();
        std::vector<std::string> attributes();
        std::vector<std::string> aliasesOfAttributes();

        size_t size();

        size_t findFirstPosition(std::string attributeName);
        size_t findFirstPosition(std::string attributeName, std::string attributeAlias);
        size_t findFirstPosition(std::string tableAlias, std::string attributeName, std::string attributeAlias);

        void print();
};


#endif // SELECTSEMANTICSTRUCTS_HPP
