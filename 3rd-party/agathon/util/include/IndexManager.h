#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

//Stl includes
#include <iostream>
#include <fstream>

//Arboretum includes
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stSlimNode.h>
#include <arboretum/stSlimTree.h>
#include <arboretum/stDummyTree.h>

/* The data structure for feature vectors */
#include <BasicArrayObject.h>

/* Distance functions structure */
#include <Evaluator.h>

/* Types definition */
typedef Evaluator<FeatureVector> MetricDistanceFunction;
typedef std::vector<FeatureVector> FeatureVectorList;

typedef stSlimTree < FeatureVector, MetricDistanceFunction > SlimTree;
typedef stDummyTree< FeatureVector, MetricDistanceFunction > DummyTree;

typedef stResult < FeatureVector > ComplexResult;
typedef std::vector<ComplexResult *> ComplexResultList;
typedef stJoinedResult < FeatureVector > ComplexJoinedResult;


class IndexManager{

    private:
        SlimTree *slimTree;
        DummyTree *dummyTree;
        WrapperDF mdf;
        std::string indexName;
        stPlainDiskPageManager *pageManager;
        std::string filePath;

    public:
        static const u_int16_t SLIM_TREE = 1;
        static const u_int16_t DUMMY_TREE = 2;
        static const u_int32_t PAGE_SIZE = 65536;

    private:
        void setIndexName(std::string indexName);
        void setDistanceFunction(MetricDistanceFunction df);
        bool fileExists(std::string indexName = "");
        std::string getIndexName();
        std::string getSlimFileName();
        std::string getDummyFileName();

        void createSlimTree(const u_int32_t pageSize);
        void openSlimTree();
        void closeSlimTree();


        void createDummyTree(const u_int32_t pageSize);
        void openDummyTree();
        void closeDummyTree();


    public:
        IndexManager(std::string indexName, MetricDistanceFunction distanceFunction, bool createDummyOnly = false, u_int32_t pageSize = IndexManager::PAGE_SIZE, bool openDummyOnly = false);
        ~IndexManager();

        void close();
        void openIndex(const u_int32_t pageSize);

        void addItem(FeatureVector *item) throw (std::exception*);
        bool itemExists(FeatureVector *item);

        ComplexResultList nearestSelectQuery(FeatureVector* center, u_int32_t k, bool tie = false);
        ComplexResultList nearestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie = false);
        ComplexResultList nearestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie = false);
        ComplexResultList nearestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie = false);
        ComplexResultList rangeSelectQuery(FeatureVector* center, double radius);
        ComplexResultList rangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius);
        ComplexResultList rangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius);
        ComplexResultList rangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius);
        ComplexResultList farthestSelectQuery(FeatureVector* center, double k, bool tie);
        ComplexResultList farthestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie);
        ComplexResultList farthestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie);
        ComplexResultList farthestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie);
        ComplexResultList reverseRangeSelectQuery(FeatureVector* center, double radius);
        ComplexResultList reverseRangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius);
        ComplexResultList reverseRangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius);
        ComplexResultList reverseRangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius);

        ComplexResultList diverseNearestQuery(FeatureVector* center, u_int32_t k1, u_int32_t k2 = 1);

        u_int16_t getTreeHeight();
        MetricDistanceFunction *getMetricDistanceFunction();
        u_int32_t getNumberOfElements();

        void dropDummyTree() throw (std::exception*);
        void dropSlimTree() throw (std::exception*);
};

#endif // INDEXMANAGER_H
