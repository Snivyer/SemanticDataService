#include "abstract/rpc/data_rpc/server.h"

namespace SDS {

class BasicDataServer::Impl {
    private:
        int client;      // 数据箱子服务客户端
    
    public:
        explicit Impl() {}
        
        // transform a databox into a filghtInfo
        arrow::Result<flight::FlightInfo> MakeFlightInfo(
            const arrow::fs::FileInfo& file_info) {
        }

        // transform a fightDescription into a 
        Status DoActionDropDataset(const std::string& key) {}

};


BasicDataServer::BasicDataServer(std::shared_ptr<Impl> impl): impl_(std::move(impl)) {}

BasicDataServer::~BasicDataServer() = default;


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