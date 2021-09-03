#ifndef CREATEMETRICSYNTACTIC_H
#define CREATEMETRICSYNTACTIC_H

#include <SQLSyntactic.h>

class CreateMetricSyntactic : public SQLSyntactic{

    private:
        void metric_name();
        void distance_name();
        void datatype_name();
        void association();
        void create_metric_specification();

    public:
        CreateMetricSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic(dbmg){
        }

        CreateMetricSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic(sql, dbmg){
            create_metric_specification();
        }

        virtual ~CreateMetricSyntactic();

        bool isValid(std::string sqlStatement);


};

#endif // CREATEMETRICSYNTACTIC_H
