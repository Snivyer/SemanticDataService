#pragma once

#include <arrow/api.h>
#include <arrow/flight/api.h>
#include <arrow/filesystem/api.h>
#include "manager/databox/databox_store.h"

namespace flight = arrow::flight;
using arrow::Status;
using namespace arrow::ipc;

namespace SDS {

    class DataBoxReader : public RecordBatchReader {
        public:
            static std::shared_ptr<DataBoxReader> Create(
                const std::shared_ptr<DataboxObject> databox);

            std::shared_ptr<Schema> schema() const override;
            Status ReadNext(std::shared_ptr<RecordBatch>* out) override;
        
        private:
            std::shared_ptr<DataboxObject> databox_;
            int stepCount;
            int step;
            DataBoxReader(std::shared_ptr<DataboxObject> dbObject);

    };
}