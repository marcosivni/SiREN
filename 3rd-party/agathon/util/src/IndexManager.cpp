#include "IndexManager.h"

IndexManager::IndexManager(std::string indexName, MetricDistanceFunction distanceFunction, bool createDummyOnly, u_int32_t pageSize, bool openDummyOnly){

    setIndexName(indexName);
    setDistanceFunction(distanceFunction);

    slimTree = nullptr;
    dummyTree = nullptr;

    if (createDummyOnly){
        dropDummyTree();
        createDummyTree(pageSize);
    } else {
        if (!fileExists(getSlimFileName())){
            createSlimTree(pageSize);
            if (!fileExists(getDummyFileName())){
                createDummyTree(pageSize);
            }
        } else {
            if (!fileExists(getDummyFileName())){
                createDummyTree(pageSize);
            } else {
                if (!openDummyOnly){
                    openSlimTree();
                }
                openDummyTree();
            }
        }
    }
}

IndexManager::~IndexManager(){

    if (slimTree != nullptr){
        delete (slimTree);
    }
    if (dummyTree != nullptr){
        delete (dummyTree);
    }
}

void IndexManager::setDistanceFunction(MetricDistanceFunction df){

    mdf = df;
}

void IndexManager::setIndexName(std::string indexName){

    this->indexName = indexName;
}

bool IndexManager::fileExists(std::string indexName){

    if (indexName.empty()){
        std::fstream file(getIndexName(), std::ios_base::in);
        return file.good();
    } else {
        std::fstream file(indexName, std::ios_base::in);
        return file.good();
    }
}

std::string IndexManager::getIndexName(){

    return indexName;
}

std::string IndexManager::getSlimFileName(){

    return filePath + "_slim_" + getIndexName();
}

std::string IndexManager::getDummyFileName(){

    return filePath + "_dummy_" + getIndexName();
}

void IndexManager::createSlimTree(const u_int32_t pageSize){

    closeSlimTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getSlimFileName().c_str(), pageSize);

    slimTree = new SlimTree(pageManager);
    slimTree->SetChooseMethod(SlimTree::cmMINDIST);
    slimTree->SetSplitMethod(SlimTree::smMINMAX);

    //Set the distance function
    slimTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

void IndexManager::openSlimTree(){

    closeSlimTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getSlimFileName().c_str());

    slimTree = new SlimTree(pageManager);
    slimTree->SetChooseMethod(SlimTree::cmMINDIST);
    slimTree->SetSplitMethod(SlimTree::smMINMAX);

    //Set the distance function
    slimTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

void IndexManager::closeSlimTree(){

    if (slimTree != nullptr){
        delete (slimTree);
    }
    slimTree = nullptr;
}

void IndexManager::dropSlimTree() throw (std::exception*){

    if (remove(getSlimFileName().c_str()) != 0)
        throw new std::exception();
}

void IndexManager::createDummyTree(const u_int32_t pageSize){

    closeDummyTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getDummyFileName().c_str(), pageSize);

    dummyTree = new DummyTree(pageManager);

    //Set the distance function
    dummyTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());
}

void IndexManager::openDummyTree(){

    closeDummyTree();

    stPlainDiskPageManager *pageManager = new stPlainDiskPageManager(getDummyFileName().c_str());

    dummyTree = new DummyTree(pageManager);

    //Set the distance function
    dummyTree->GetMetricEvaluator()->setDistanceFunction(mdf.distanceType());

}

void IndexManager::closeDummyTree(){

    if (dummyTree != nullptr){
        delete (dummyTree);
    }
    dummyTree = NULL;
}

void IndexManager::dropDummyTree() throw (std::exception*){

    if (fileExists(getDummyFileName())){
        if (remove(getDummyFileName().c_str()) != 0)
            throw new std::exception();
    }
}

void IndexManager::close(){

    closeSlimTree();
    closeDummyTree();
}

void IndexManager::openIndex(const u_int32_t pageSize){


    if (fileExists(indexName)){
        openSlimTree();
        openDummyTree();
    } else {
        createDummyTree(pageSize);
        createSlimTree(pageSize);
    }

}

void IndexManager::addItem(FeatureVector *item) throw (std::exception*){

    if (!itemExists(item)){
        if (slimTree != nullptr){
            slimTree->Add(item);
            if (dummyTree != nullptr)
                dummyTree->Add(item);
        } else {
            if (dummyTree != nullptr){
                dummyTree->Add(item);
            } else {
                throw new std::exception();
            }
        }
    }
}

bool IndexManager::itemExists(FeatureVector *item){

    ComplexResult *result = nullptr;
    if (slimTree != nullptr){
        result = slimTree->RangeQuery(item, 0);
    } else {
        if (dummyTree != nullptr){
            result = dummyTree->RangeQuery(item, 0);
        }
    }

    if (result != nullptr){
        if (result->GetNumOfEntries() > 0){
            delete (result);
            return true;
        }
    }
    return false;
}

ComplexResultList IndexManager::nearestSelectQuery(FeatureVector* center, u_int32_t k, bool tie){

    ComplexResultList answer;
    ComplexResult *resultSet;

    if (slimTree != nullptr){
        resultSet = slimTree->NearestQuery(center, k, tie);
    } else {
        resultSet = dummyTree->NearestQuery(center, k, tie);
    }

    answer.push_back(resultSet);
    return answer;
}

ComplexResultList IndexManager::nearestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->AllNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::nearestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->MaxNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::nearestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->SumNearestQuery(sqs, centersSize, k, tie);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::rangeSelectQuery(FeatureVector* center, double radius){

    ComplexResultList answer ;
    ComplexResult *resultSet;

    if (slimTree != nullptr){
        resultSet = slimTree->RangeQuery(center, radius);
    } else {
        resultSet = dummyTree->RangeQuery(center, radius);
    }

    answer[0] = resultSet;
    return answer;
}

ComplexResultList IndexManager::rangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->AllRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::rangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet  = dummyTree->MaxRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::rangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != NULL){
        FeatureVector **sqs = new FeatureVector*[centers.size()];
        for (size_t x = 0; x < centers.size(); x++){
            sqs[x] = new FeatureVector(centers[x]);
        }
        rSet = dummyTree->SumRangeQuery(sqs, centersSize, radius);
        for (size_t x = 0; x < centers.size(); x++){
            delete (sqs[x]);
        }
        delete[] (sqs);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestSelectQuery(FeatureVector* center, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr){
        rSet = dummyTree->FarthestQuery(center, k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestAllSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestMaxSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet =  dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::farthestSumSelectQuery(FeatureVectorList centers, int centersSize, double k, bool tie){

    ComplexResultList answer ;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet =  dummyTree->FarthestQuery(&centers[0], k, tie);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeSelectQuery(FeatureVector* center, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr){
        rSet = dummyTree->ReversedRangeQuery(center, radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeAllSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeMaxSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

ComplexResultList IndexManager::reverseRangeSumSelectQuery(FeatureVectorList centers, int centersSize, double radius){

    ComplexResultList answer;
    ComplexResult* rSet = nullptr;

    if (dummyTree != nullptr && centers.size() > 0){
        rSet = dummyTree->ReversedRangeQuery(&centers[0], radius);
    }

    answer.push_back(rSet);
    return answer;
}

u_int16_t IndexManager::getTreeHeight(){

    if (slimTree != nullptr){
        return slimTree->GetHeight();
    }
    return 0;
}

MetricDistanceFunction * IndexManager::getMetricDistanceFunction(){

    return &mdf;
}

u_int32_t IndexManager::getNumberOfElements(){

    if (dummyTree != NULL){
        return dummyTree->GetNumberOfObjects();
    }
    return 0;
}

ComplexResultList IndexManager::diverseNearestQuery(FeatureVector* center, u_int32_t k1, u_int32_t k2){

    ComplexResultList resultSet;
    ComplexResult *tmpResult;

    resultSet = dummyTree->diverseBRIDGEQuery(center, k1);
    for (size_t x = 0; x < resultSet.size(); x++){
        tmpResult = resultSet[x];
        while (tmpResult->GetNumOfEntries() > k2){
            tmpResult->RemoveLast();
        }
    }

    return resultSet;
}
