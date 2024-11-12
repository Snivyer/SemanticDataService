#pragma once

#include <arrow/api.h>
#include <arrow/flight/api.h>
#include <arrow/filesystem/api.h>
#include "manager/databox/databox_object.h"

namespace flight = arrow::flight;
using arrow::Status;
using namespace arrow::ipc;

namespace SDS {

    class DataBoxReader : public RecordBatchReader {
        public:

            std::shared_ptr<Schema> schema() const override;
            Status ReadNext(std::shared_ptr<RecordBatch>* out) override;
            Status SetDataSource( DataboxObject* dbObject);
        
        private:
            DataboxObject* databox_;
            int stepCount;
            int step;

    };
}