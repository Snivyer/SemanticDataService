#include "manager/rpc/data_rpc/reader.h"


namespace SDS {

    std::shared_ptr<DataBoxReader>  DataBoxReader::Create(
                const std::shared_ptr<DataboxObject> dbObject) {

        std::shared_ptr<DataBoxReader> reader(new DataBoxReader(std::move(dbObject)));
        return reader;
    }

    std::shared_ptr<Schema> DataBoxReader::schema() const {
        ARROW_LOG(INFO) << "Received a schema Request";
        std::shared_ptr<Schema> schema =  databox_->getSchema();
        return schema;
    }


    Status DataBoxReader::ReadNext(std::shared_ptr<RecordBatch>* out) {

        ARROW_LOG(INFO) << "Received a ReadNext Request";
        if(step < stepCount) {
            *out = databox_->getData(step);
            ARROW_RETURN_NOT_OK(out->get()->Validate());
            step += 1;
        } else {
            *out = nullptr;
        }
        return Status::OK();
        
    }
        
  
    DataBoxReader::DataBoxReader(std::shared_ptr<DataboxObject> dbObject) {
        databox_ = std::move(dbObject);
        stepCount = databox_->getBatchNum();
        step = 0;
    }




}