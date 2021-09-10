#ifndef DROPMETRICSYNTACTIC_H
#define DROPMETRICSYNTACTIC_H

#include <SQLSyntactic.h>

class DropMetricSyntactic : public SQLSyntactic{

    private:
        void metric_name();
        void drop_metric_specification();

    public:
        DropMetricSyntactic(DatabaseManager *dbmg = nullptr) : SQLSyntactic (dbmg){
        }

        DropMetricSyntactic(std::string sql, DatabaseManager *dbmg) : SQLSyntactic (sql, dbmg){
            drop_metric_specification();
        }

        virtual ~DropMetricSyntactic();

        bool isValid(std::string sqlStatement);
};

#endif // DROPMETRICSYNTACTIC_H
