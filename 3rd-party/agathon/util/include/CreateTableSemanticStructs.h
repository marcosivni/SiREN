#ifndef CREATETABLESEMANTICSTRUCTS_HPP
#define CREATETABLESEMANTICSTRUCTS_HPP

#include <DataDictionary.h>

class CheckValidColumnsAndTypes{

    private:
        std::vector< std::pair<std::string, std::string> > listColumns;
        std::string error;
        DataDictionary *dic;

    public:
        CheckValidColumnsAndTypes(DataDictionary *dic);
        ~CheckValidColumnsAndTypes();

        void setDataType(size_t pos, std::string dataType) throw (std::exception*);
        void addNewColumn(std::string columnName, std::string dataType);
        void addNewColumn(std::string columnName);
        void addNewDataType(std::string dataType);
        void addError(std::string error);

        int countColumns(std::string columnName);

        bool isComplexColumn(std::string columnName);
        bool isValid();
        bool isUniqueColumnNames();

        std::string dataType(std::string columnName);
        std::string simplifiedDataType(size_t pos);
        std::string dimensionalityOfDataType(size_t pos);

        size_t getSize();
        std::pair<std::string, std::string> getColumnNameAndDataType(size_t pos) throw (std::exception*);
        std::string getError();
        std::string toString();
};

class Constraint{

    private:
        std::string name;
        unsigned char type;
        std::vector<std::string> attributes;
        std::string referenceTableName;
        std::vector< std::pair <std::string, bool> > usingMetricName;
        std::vector<std::string> referenceAttributes;

    public:
        static const unsigned char PRIMARY_KEY = 0;
        static const unsigned char FOREIGN_KEY = 1;
        static const unsigned char METRIC = 2;
        static const unsigned char UNIQUE = 3;
        static const unsigned char NOT_NULL = 4;
        static const unsigned char NULL_ = 5;

    public:
        Constraint();
        ~Constraint();

        void clear();

        void setName(std::string name);
        void setType(unsigned char type);
        void addAttribute(std::string attribute);
        void setReferenceTableName(std::string referenceTableName);
        void addMetricName(std::pair<std::string, bool> usingMetricName);
        void addReferenceAttribute(std::string referenceAttribute);

        bool isValid();
        bool isEqual(Constraint constraint);

        std::string getName();
        unsigned char getType();
        std::vector<std::string> getAttributes();
        std::string getReferenceTableName();
        std::vector< std::pair<std::string, bool> > getMetricsNames();
        std::vector<std::string> getReferenceAttributes();
};

class CheckConstraints{

    private:
        std::vector <Constraint> constraintList;
        std::string error;
        DataDictionary *dic;

    public:
        CheckConstraints(DataDictionary *dic);
        ~CheckConstraints();

        void addNewConstraint(Constraint constraint);
        void addError(std::string error);

        int countNames(std::string constraintName);
        int countMetrics(Constraint constraint);
        int countDefaults(Constraint constraint);
        int countPrimaryKey();
        int countForeignKey(Constraint constraint);
        int countUnique(Constraint constraint);

        bool checkAttributes(CheckValidColumnsAndTypes *columnsAndTypes);
        bool isValid(CheckValidColumnsAndTypes *columnsAndTypes);
        bool isConstraint();

        Constraint getConstraint(size_t pos)  throw (std::exception*);
        size_t getSize();
        std::string getError();
        DataDictionary *dictionary();
        std::string toString();
};

#endif // CREATETABLESEMANTICSTRUCTS_HPP
