#include "manager/databox/databox_object.h"

namespace SDS {

    DataboxObject::DataboxObject() {
        meta_.filled = false;
    }

    DataboxObject::~DataboxObject() {

    }

    void DataboxObject::setDataPath(FilePathList &dataPath) {
        dataPath_ = dataPath;
    }

    std::shared_ptr<arrow::Schema> DataboxObject::makeSchema(std::vector<VarDesc> &descList) {

        arrow::FieldVector fileds;
        for(auto varDesc : descList) {
            if(varDesc.ncGroupID != -1) {
                // todo (if there are nc group)
    
            } else {
                if(varDesc.varType == "NC_BYTE") {
                    fileds.push_back(arrow::field(varDesc.varName, arrow::binary()));
                } else if (varDesc.varType == "NC_SHORT") {
                    fileds.push_back(arrow::field(varDesc.varName, arrow::int16()));
                } else if(varDesc.varType == "NC_INT") {
                    fileds.push_back(arrow::field(varDesc.varName, arrow::int32()));
                } else if (varDesc.varType == "NC_FLOAT") {
                    fileds.push_back(arrow::field(varDesc.varName, arrow::float32()));
                } else if (varDesc.varType == "NC_DOUBLE") {
                    fileds.push_back(arrow::field(varDesc.varName, arrow::float64()));
                } else {
                    continue;
                }
            }
        }

        std::shared_ptr<arrow::Schema> schema = arrow::schema(fileds);
        return schema;
    }



    arrow::Status DataboxObject::fillData(Adaptor* adaptor) {

        if(!adaptor) {
             return arrow::Status::OK();
        }

        // fill the data recordbatch
        adaptor->getVarDescList(dataPath_, meta_.varList);

        if(meta_.varList.size() == 0) {
            return arrow::Status::UnknownError("test");
        }

        std::shared_ptr<arrow::Schema> schema;

        for(std::string filepath : dataPath_.fileNames) {
            std::string path = combinePath(dataPath_.dirPath, filepath);
            std::shared_ptr<arrow::RecordBatch> batch;
            arrow::ArrayVector arrayData;

            if(schema.get() == nullptr) {
                schema = makeSchema(meta_.varList);
            }
            adaptor->readVarList(path, meta_.varList, arrayData);

            batch = arrow::RecordBatch::Make(schema, meta_.varList[0].varLen, arrayData);
            batchs_.push_back(batch);
        }

        // fill the metadata
        meta_.stepCount = batchs_.size();
        meta_.varCount = meta_.varList.size();
        meta_.varLen = meta_.varList[0].varLen;

        meta_.filled = true;
        schema_ = makeSchema(meta_.varList);
        return arrow::Status::OK();
    }

    DBMeta&  DataboxObject::getDBMeta() {
        return this->meta_;
    }

    arrow::Status DataboxObject::removeData() {
        batchs_.clear();
        return Status::NotImplemented("Sorry, I don't to release the memory of arrow recordbatch.");
    }

    std::shared_ptr<arrow::Schema> DataboxObject::getSchema() {
        return schema_;
    }

    std::shared_ptr<arrow::RecordBatch> DataboxObject::getData(int step) {
        return batchs_[step];
    }

     int DataboxObject::getBatchNum() {
        return this->meta_.stepCount;
     }

    void  DataboxObject::print() {

        // print metadata firstly
        meta_.print();

        // print recordbatch
        for(auto item : batchs_) {
            std::cout << "----------------------------"  << std::endl;
            std::cout << item->ToString() << std::endl;
        }
    }



}