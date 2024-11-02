#include "manager/rpc/data_rpc/server.h"

namespace SDS {

class BasicDataServer::Impl {
    private:
        std::shared_ptr<DataboxObject> dbObject_;
        
    
    public:
        explicit Impl(DataboxObject *dbObject) {
            setDataBoxObject(dbObject);
        }
  
        // transform a databox into a filghtInfo
        arrow::Result<flight::FlightInfo> MakeFlightInfo(
            const arrow::fs::FileInfo& file_info) {
        }

        // transform a fightDescription into a 
        Status DoActionDropDataset(const std::string& key) {}

        void setDataBoxObject(DataboxObject *dbObject) {
            dbObject_.reset(dbObject);
        }

        std::shared_ptr<DataboxObject>& getDataBoxObject() {
            return dbObject_;
        }

};


BasicDataServer::BasicDataServer(std::shared_ptr<Impl> impl): impl_(std::move(impl)) {}

BasicDataServer::~BasicDataServer() = default;

std::shared_ptr<BasicDataServer> BasicDataServer::Create(DataboxObject *dbObject) {

    std::shared_ptr<BasicDataServer::Impl> impl(new BasicDataServer::Impl(dbObject));
    std::shared_ptr<BasicDataServer> service(new BasicDataServer(impl));
    return service;

}


Status BasicDataServer::ListFlights(const flight::ServerCallContext &, 
                            const flight::Criteria*,
		                    std::unique_ptr<flight::FlightListing> *listings) {

}

Status BasicDataServer::GetFlightInfo(const flight::ServerCallContext &context, 
                                const flight::FlightDescriptor &descriptor,
                                std::unique_ptr<flight::FlightInfo> *info) {

}

Status BasicDataServer::PollFlightInfo(const flight::ServerCallContext& context,
                                const flight::FlightDescriptor& request,
                                std::unique_ptr<flight::PollInfo>* info) {

}

Status BasicDataServer::GetSchema(const flight::ServerCallContext& context,
                           const flight::FlightDescriptor& request,
                           std::unique_ptr<flight::SchemaResult>* schema) {

}

Status BasicDataServer::DoPut(const flight::ServerCallContext &,
						std::unique_ptr<flight::FlightMessageReader> reader,
						std::unique_ptr<flight::FlightMetadataWriter>) {

    

}

Status BasicDataServer::DoGet(const flight::ServerCallContext &context,
                        const flight::Ticket &request,
                        std::unique_ptr<flight::FlightDataStream> *stream) {

    ARROW_LOG(INFO) << "Received a DoGet Request";

    std::shared_ptr<DataboxObject> dbObject = impl_->getDataBoxObject();

    auto reader = DataBoxReader::Create(dbObject);
    *stream = std::unique_ptr<arrow::flight::FlightDataStream>(
            new arrow::flight::RecordBatchStream(std::move(reader)));
    
    return arrow::Status::OK();
}

Status BasicDataServer::DoExchange(const flight::ServerCallContext& context,
                            std::unique_ptr<flight::FlightMessageReader> reader,
                            std::unique_ptr<flight::FlightMessageWriter> writer) {

}

Status BasicDataServer::ListActions(const flight::ServerCallContext &context,
                            std::vector<flight::ActionType> *actions) {

}

Status BasicDataServer::DoAction(const flight::ServerCallContext &context,
                        const flight::Action &action,
                        std::unique_ptr<flight::ResultStream> *result) {

} 



};