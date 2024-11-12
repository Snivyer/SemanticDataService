#include "manager/rpc/data_rpc/meta_server.h"

namespace SDS {

class BasicMetaServer::Impl {
    private:
        // a map of databox ID to FilghtClient
        std::unordered_map<std::string, std::shared_ptr<BasicDataServer>> clientMap;

        // a map of databox ID to Flight
        std::unordered_map<std::string, std::vector<flight::FlightInfo>> flightMap;
    
    public:

        explicit Impl() {}

        void addClient(std::string spaceID, std::shared_ptr<BasicDataServer> client) {
            auto ret = clientMap.find(spaceID);
            if(ret == clientMap.end()) {
                clientMap[spaceID] = client;
            }
        }

        std::shared_ptr<BasicDataServer> getClient(std::string spaceID) {
            auto ret = clientMap.find(spaceID);
            if(ret != clientMap.end()) {
                return clientMap[spaceID];
            }
            std::shared_ptr<BasicDataServer> clientPtr(nullptr);
            return clientPtr;
        }

        void releaseCache(std::string spaceID) {

            auto ret1 = flightMap.find(spaceID);
            if(ret1 != flightMap.end()) {
                flightMap.erase(ret1);
            }

            auto ret2 = clientMap.find(spaceID);
            if(ret2 != clientMap.end()) {
                clientMap.erase(ret2);
            }
        }

        void cacheFlights(std::string spaceID, std::vector<flight::FlightInfo> flights) {

            auto ret = flightMap.find(spaceID);
            if(ret != flightMap.end()) {
                ret->second.clear();
                ret->second = flights;
            } else {
                flightMap[spaceID] = flights;
            }
        }

        void getFlights(std::vector<flight::FlightInfo> &flights) {
            for (auto info : flightMap) {
                for(auto flight : info.second) {
                    flights.push_back(flight);
                }
            }
        }

};


BasicMetaServer::BasicMetaServer(std::shared_ptr<Impl> impl): impl_(std::move(impl)) {}

BasicMetaServer::~BasicMetaServer() = default;

BasicMetaServer::BasicMetaServer(BasicMetaServer &&) noexcept {};

std::shared_ptr< BasicMetaServer> BasicMetaServer::Create() {

    std::shared_ptr<BasicMetaServer::Impl> impl = std::make_shared<BasicMetaServer::Impl>();
    std::shared_ptr<BasicMetaServer> service(new BasicMetaServer(impl));
    return service;
}


Status BasicMetaServer::ListFlights(const flight::ServerCallContext &, 
                            const flight::Criteria*,
		                    std::unique_ptr<flight::FlightListing> *listings) {

    ARROW_LOG(INFO) << "MetaServer: Received a ListFlights Request";    
    std::vector<flight::FlightInfo> flights;
    impl_->getFlights(flights);

    *listings = std::unique_ptr<flight::FlightListing>(new flight::SimpleFlightListing(std::move(flights)));
    return Status::OK();
}


Status BasicMetaServer::ListActions(const flight::ServerCallContext &context,
                            std::vector<flight::ActionType> *actions) {
    *actions = {kActionSpaceFilter};
    return Status::OK();
}

Status BasicMetaServer::DoAction(const flight::ServerCallContext &context,
                        const flight::Action &action,
                        std::unique_ptr<flight::ResultStream> *result) {
    
    if(action.type == kActionSpaceFilter.type) {
        *result = nullptr;
        return DoActionSpaceFilter(action.body->ToString());
    } else if (action.type == kActionSpaceRemove.type) {
        *result = nullptr;
        return DoActionSpaceRemove(action.body->ToString());
    }




    return Status::NotImplemented("Unknown action type:", action.type);
} 


Status  BasicMetaServer::DoActionSpaceFilter(const std::string &spaceID) {

    // Call listFlights against other server
    std::shared_ptr<BasicDataServer> client = impl_->getClient(spaceID);
    ARROW_LOG(INFO) << "MetaServer: Received a DoActionSpaceFilter Request";

    std::vector<flight::FlightInfo> flights;
    auto listResult = client->listFlights();
    auto listing = std::move(listResult);

    while(true) {
        auto flightInfoResult = listing->Next();
        if(flightInfoResult == nullptr) {
            break;
        }
        flights.push_back(*flightInfoResult.ValueOrDie());
    }
    impl_->cacheFlights(spaceID, flights);
    return Status::OK();
}

Status BasicMetaServer::DoActionSpaceRemove(const std::string &spaceID) {

    // Call listFlights against other server
    ARROW_LOG(INFO) << "MetaServer: Received a DoActionSpaceRemove Request";

    // release cache
    impl_->releaseCache(spaceID);
    return Status::OK();



}

Status BasicMetaServer::CreateRent(std::string spaceID, std::shared_ptr<BasicDataServer> client) {
   impl_->addClient(spaceID, client);
   return Status::OK();
}

std::shared_ptr<BasicDataServer> BasicMetaServer::RentOne(std::string spaceID) {
    return impl_->getClient(spaceID);
}

};