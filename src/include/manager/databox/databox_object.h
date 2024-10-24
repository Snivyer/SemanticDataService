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
        size_t stepCount;
        size_t varCount;
        size_t varLen;
        std::vector<VarDesc> varList;
    } DBMeta; 

    class  DataboxObject {
        public:
            bool filled;

            DataboxObject();
            ~DataboxObject();

   

            void setDataPath(FilePathList &dataPath);
            arrow::Status fillData(std::shared_ptr<Adaptor> adaptor);
            arrow::Status removeData();
            void print();

        private:
            std::vector<std::shared_ptr<arrow::RecordBatch>> batchs_;
            DBMeta meta_;
            struct FilePathList dataPath_;
            std::shared_ptr<arrow::Schema> makeSchema(std::vector<VarDesc> &descList);
 
    };






}