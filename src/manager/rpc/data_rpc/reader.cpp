#include "manager/rpc/data_rpc/reader.h"


namespace SDS {

    Status DataBoxReader::SetDataSource(DataboxObject* dbObject) {

        if(dbObject == nullptr) {
            return Status::NotImplemented("DataBox Reader have no data source");
        }

        databox_ = dbObject;
        step = 0;
        stepCount = dbObject->getBatchNum();
        return Status::OK();
    
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
        } else{
            *out = nullptr;
        }
        return Status::OK();
        
    }
         


}