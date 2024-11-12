#pragma once

#include <arrow/api.h>
#include <arrow/flight/api.h>
#include "arrow/util/logging.h"
#include "manager/rpc/data_rpc/server.h"

namespace flight = arrow::flight;
using arrow::Status;


namespace SDS {

    class BasicMetaServer : public flight::FlightServerBase {
        public:
            const flight::ActionType kActionSpaceRemove{"space_remove", "Space remove!"};
            const flight::ActionType kActionSpaceFilter{"space_filter", "Space Filter."};

            ~BasicMetaServer();
            BasicMetaServer(BasicMetaServer &&)  noexcept;

            static std::shared_ptr< BasicMetaServer> Create();

            Status ListFlights(const flight::ServerCallContext &, 
                        const flight::Criteria*,
		                std::unique_ptr<flight::FlightListing> *listings) override;

            Status ListActions(const flight::ServerCallContext &context,
                            std::vector<flight::ActionType> *actions) override;

            Status DoAction(const flight::ServerCallContext &context,
                            const flight::Action &action,
                            std::unique_ptr<flight::ResultStream> *result) override; 

            Status CreateRent(std::string spaceID, std::shared_ptr<BasicDataServer> client);
            std::shared_ptr<BasicDataServer> RentOne(std::string spaceID);



        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            Status DoActionSpaceFilter(const std::string &spaceID);
            Status DoActionSpaceRemove(const std::string &spaceID);
            explicit BasicMetaServer(std::shared_ptr<Impl> impl);
    };






}