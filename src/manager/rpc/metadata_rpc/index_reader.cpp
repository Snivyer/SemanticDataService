#include "manager/rpc/metadata_rpc/index_reader.h"

namespace SDS {

    IndexClient::IndexClient(std::shared_ptr<grpc::Channel> channel): 
                stub_(indexService::NewStub(channel)) {}

    bool IndexClient::GetIndex(std::string cntDesc, ContentID &cntID, std::vector<FilePathList> filepath) {
        
        IndexSend request;
        request.set_contentdesc(cntDesc);

        IndexReply response;
        grpc::ClientContext context;

        grpc::Status status = stub_->GetIndex(&context, request, &response);
        if(status.ok()) {

            ARROW_LOG(INFO) << "Received Index Service Reply";
            cntID.setSpaceID(std::to_string(response.spaceid()));
            cntID.setTimeID(std::to_string(response.timeid()));
            cntID.setVarID(std::to_string(response.varid(0)));

            for(int i = 0; i < response.dirpath_size(); i++) {
                FilePathList pathList;
                pathList.dirPath = response.dirpath(i);
                pathList.fileNames.push_back(response.filepath(i));
                filepath.push_back(pathList);
            } 
        } else {
            ARROW_LOG(INFO) << "Did not Received Index Service Reply!";
        }
        return true;
    }
}


