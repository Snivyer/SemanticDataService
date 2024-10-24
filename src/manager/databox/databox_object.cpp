#include "manager/databox/databox_object.h"

namespace SDS {

    DataboxObject::DataboxObject() {
        filled = false;
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



    arrow::Status DataboxObject::fillData(std::shared_ptr<Adaptor> adaptor) {

        if(!adaptor) {
             return arrow::Status::OK();
        }

        // fill the data recordbatch
        adaptor->getVarDescList(dataPath_, meta_.varList);

        if(meta_.varList.size() == 0) {
            return arrow::Status::UnknownError("test");
        }

        for(std::string filepath : dataPath_.fileNames) {
            std::string path = combinePath(dataPath_.dirPath, filepath);
            std::shared_ptr<arrow::RecordBatch> batch;
            arrow::ArrayVector arrayData;
            std::shared_ptr<arrow::Schema> schema;
            adaptor->readVarList(path, meta_.varList, arrayData);
            schema = makeSchema(meta_.varList);
            batch = arrow::RecordBatch::Make(schema, arrayData.size(), arrayData);
            batchs_.push_back(batch);
        }

        // fill the metadata
        meta_.stepCount = batchs_.size();
        meta_.varCount = meta_.varList.size();
        meta_.varLen = meta_.varList[0].varLen;

        filled = true;
        return arrow::Status::OK();
    }

    arrow::Status DataboxObject::removeData() {

    }

    void  DataboxObject::print() {

        // print metadata firstly
        std::cout << "Step count:" << meta_.stepCount << std::endl;
        std::cout << "Var count:" << meta_.varCount << std::endl;
        std::cout << "Var grid size:" << meta_.varLen << std::endl;

        // print recordbatch
        for(auto item : batchs_) {
            std::cout << "----------------------------"  << std::endl;
            std::cout << item->ToString() << std::endl;
        }
    }



}