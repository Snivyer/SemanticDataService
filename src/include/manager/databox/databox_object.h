#pragma once

#include <memory>
#include <vector>
#include <arrow/api.h>
#include "arrow/util/logging.h"

#include "abstract/meta/cnt_meta_template.h"
#include "abstract/meta/sto_meta_template.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/adaptor/adaptor.h"

using namespace arrow;



namespace SDS
{

    typedef struct {
        int stepCount;
        int varCount;
        int varLen;
        std::vector<VarDesc> varList;
        bool filled;

        void print() {
            std::cout << "Step count:" << stepCount << std::endl;
            std::cout << "Var count:" << varCount << std::endl;
            std::cout << "Var grid size:" << varLen << std::endl;

            for(auto var : varList) {
                std::cout << var.varName << std::endl;
            }
        }


    } DBMeta; 

    class  DataboxObject {
        public:
          
            DataboxObject();
            ~DataboxObject();

   
            DBMeta& getDBMeta();
            void setDataPath(FilePathList &dataPath);
            arrow::Status fillData(Adaptor *adaptor);
            arrow::Status removeData();
            void print();
            std::shared_ptr<arrow::Schema> getSchema();
            std::shared_ptr<arrow::RecordBatch> getData(int step);
            int getBatchNum();

        private:
            std::vector<std::shared_ptr<arrow::RecordBatch>> batchs_;
            DBMeta meta_;
            FilePathList dataPath_;
            std::shared_ptr<arrow::Schema> schema_;
            std::shared_ptr<arrow::Schema> makeSchema(std::vector<VarDesc> &descList);
 
    };






}