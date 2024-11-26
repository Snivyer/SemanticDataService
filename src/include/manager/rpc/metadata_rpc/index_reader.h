
#pragma once
#include <grpcpp/grpcpp.h>
#include "manager/proto/index.grpc.pb.h"
#include "arrow/status.h"
#include "arrow/util/logging.h"
#include "abstract/meta/cnt_ID.h"
#include "abstract/meta/sto_meta_template.h"

using arrow::Status;
using IndexRpc::indexService;
using IndexRpc::IndexSend;
using IndexRpc::IndexReply;

namespace SDS {


class IndexClient {
public:
    IndexClient(std::shared_ptr<grpc::Channel> channel);
    bool GetIndex(std::string cntDesc, ContentID &cntID, std::vector<FilePathList> filepath);   


private:
    std::unique_ptr<indexService::Stub> stub_;
    

};

}