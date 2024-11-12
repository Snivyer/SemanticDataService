#include "manager/rpc/data_rpc/server.h"

namespace SDS {

class BasicDataServer::Impl {
    public:
        explicit Impl(std::string ip) {
            ip_ = ip;
            liveNum_ = 0;
        }

        // second layer index
        std::unordered_map<int, DataboxObject*> ticketMap_;
        std::unordered_map<ContentID, std::unordered_map<int, DataboxObject*>::iterator, 
                                ContentIDHasher> dbMap_;

        std::unique_ptr<flight::FlightClient> client_;
        std::string ip_;
        int liveNum_;
  
        // transform a fightDescription into a 
        Status DoActionDropDataset(const std::string& key) {}

        bool insertDB(const ContentID &cntID, DataboxObject* dbObject) {

            if(dbObject == nullptr) {
                return false;
            }

            int id = dbMap_.size();
            ticketMap_[id] = dbObject;
            auto it = ticketMap_.find(id);
            dbMap_[cntID] = it;
            liveNum_ += 1;
            return true;
        }


        DataboxObject* getDataBoxObject(const ContentID &cntID) {
            auto ret = dbMap_.find(cntID); 
            if(ret != dbMap_.end()) {
                auto it = ret->second;
                if(it != ticketMap_.end()) {
                    return it->second;
                }
            }
            return nullptr;
        }



}; 


BasicDataServer::BasicDataServer(std::shared_ptr<Impl> impl): impl_(std::move(impl)) {}

BasicDataServer::~BasicDataServer() = default;


std::shared_ptr<BasicDataServer> BasicDataServer::Create(std::string ip) {

    std::shared_ptr<BasicDataServer::Impl> impl(new BasicDataServer::Impl(ip));
    std::shared_ptr<BasicDataServer> service(new BasicDataServer(impl));
    return service;

}


std::unique_ptr<flight::FlightListing> BasicDataServer::listFlights() {

    ARROW_LOG(INFO) << "DataServer(" + std::to_string(port()) + "): Received a ListFlights Request"; 
    
    std::vector<flight::FlightInfo> flights;
    for(auto item: impl_->dbMap_) {
        if(item.second != impl_->ticketMap_.end()) {
            if(item.second->second != nullptr) {
                auto info = MakeFlightInfo(item.first, item.second->first);
                flights.push_back(info.ValueOrDie());
            }
        }
    }

    std::unique_ptr<flight::FlightListing> listings = std::unique_ptr<flight::FlightListing>(
        new flight::SimpleFlightListing(std::move(flights)));
    return listings;
}

Status BasicDataServer::ListFlights(const flight::ServerCallContext &, 
                            const flight::Criteria*,
		                    std::unique_ptr<flight::FlightListing> *listings) {
    *listings = listFlights();
    return Status::OK();
}

Status BasicDataServer::GetFlightInfo(const flight::ServerCallContext &context, 
                                const flight::FlightDescriptor &descriptor,
                                std::unique_ptr<flight::FlightInfo> *info) {
    
    ARROW_LOG(INFO) << "DataServer" + std::to_string(port()) + ": Received a getFlights Request:" 
                        "get " + descriptor.ToString(); 
    
    auto cntID = ContentID(descriptor.path);
    auto item = impl_->dbMap_.find(cntID);

    if(item == impl_->dbMap_.end()) {
        return Status::NotImplemented("Can not find the target descriptor");
    }

    if(item->second->second == nullptr) {
        return Status::NotImplemented("Data Box has delete");
    }

    auto flight_info = MakeFlightInfo(item->first, item->second->first);
    *info = std::unique_ptr<flight::FlightInfo>(new flight::FlightInfo(std::move(flight_info.ValueOrDie())));
    return Status::OK();
}


Status BasicDataServer::DoPut(const flight::ServerCallContext &,
						std::unique_ptr<flight::FlightMessageReader> reader,
						std::unique_ptr<flight::FlightMetadataWriter>) {

    

}

Status BasicDataServer::DoGet(const flight::ServerCallContext &context,
                        const flight::Ticket &request,
                        std::unique_ptr<flight::FlightDataStream> *stream) {

    ARROW_LOG(INFO) << "Received a DoGet Request";
    int ticket = std::stoi(request.ticket);

    auto ret = impl_->ticketMap_.find(ticket);

    if(ret == impl_->ticketMap_.end()) {
        return Status::NotImplemented("Unknown ticket:", ticket);
    }

    DataboxObject* object = ret->second;
    if(object == nullptr) {
        return Status::NotImplemented("Data Box has delete");
    }

    std::unique_ptr<DataBoxReader> reader(new DataBoxReader());
    reader->SetDataSource(object);

    *stream = std::unique_ptr<arrow::flight::FlightDataStream>(
            new arrow::flight::RecordBatchStream(std::move(reader)));
    return arrow::Status::OK();
}


   // transform a databox into a filghtInfo
arrow::Result<flight::FlightInfo> BasicDataServer::MakeFlightInfo(const ContentID &cntID, int ticket) {

    std::vector<std::string> identify;
    identify.push_back(cntID.getSpaceID());
    identify.push_back(cntID.getTimeID());
    identify.push_back(cntID.getVarID());

    flight::FlightDescriptor desc = flight::FlightDescriptor::Path(identify);
    flight::FlightEndpoint endpoint;
    endpoint.ticket.ticket = std::to_string(ticket);
    flight::Location location;
    ARROW_ASSIGN_OR_RAISE(location, flight::Location::ForGrpcTcp("0.0.0.0", port()));
    endpoint.locations.push_back(location);
    int64_t totol_records = impl_->getDataBoxObject(cntID)->getDBMeta().stepCount;
    int64_t totol_vars = impl_->getDataBoxObject(cntID)->getDBMeta().varCount;
    std::shared_ptr<arrow::Schema> schema = impl_->getDataBoxObject(cntID)->getSchema();

    return flight::FlightInfo::Make(*schema, desc, {endpoint}, totol_records, totol_vars);

}


Status BasicDataServer::Connect(std::string ip, int64_t port) {

    arrow::flight::Location location;
    auto location_result =
        arrow::flight::Location::ForGrpcTcp(ip, port);
    location = location_result.ValueOrDie();
    auto options = arrow::flight::FlightClientOptions::Defaults();
    auto result = arrow::flight::FlightClient::Connect(location, options);
    impl_->client_ = std::move(result.ValueOrDie());
    ARROW_LOG(INFO) << "data rpc Client have connected  to its Server.";
    return Status::OK();
}


Status BasicDataServer::RegisterWithSpaceFilterModel(std::string spaceID) {
    arrow::flight::Action action;
    action.type = kActionSpaceFilter.type;
    action.body = arrow::Buffer::FromString(spaceID);
    impl_->client_->DoAction(action);
    return Status::OK();
}

Status BasicDataServer::WithdrawWithSpaceRemoveModel(std::string spaceID) {
    
    arrow::flight::Action action;
    action.type = kActionSpaceRemove.type;
    action.body = arrow::Buffer::FromString(spaceID);
    impl_->client_->DoAction(action);
    return Status::OK();
}

Status BasicDataServer::PrepareSend(const ContentID &cntID, DataboxObject* dbObject) {

    impl_->insertDB(cntID, dbObject);
    RegisterWithSpaceFilterModel(cntID.getSpaceID());
    return Status::OK();
}

Status BasicDataServer::WithdrawSend(const ContentID &cntID) {

    impl_->ticketMap_.clear();
    impl_->dbMap_.clear();
    WithdrawWithSpaceRemoveModel(cntID.getSpaceID());

}
int BasicDataServer::getRunPort() {
    return port();
}

std::string BasicDataServer::getRunIp() {
    return  impl_->ip_;
}

bool BasicDataServer::BeLive() {

    if(impl_->liveNum_ == 0) {
        return false;
    }
    return true;
}

bool BasicDataServer::DeleteDB(const ContentID &cntID) {
    auto ret = impl_->dbMap_.find(cntID); 
        if(ret != impl_->dbMap_.end()) {
            auto it = ret->second;
            if(it != impl_->ticketMap_.end()) {
                    it->second = nullptr;
                    impl_->liveNum_ -= 1;
                    return true;
            }
        }
    return false;
}



};