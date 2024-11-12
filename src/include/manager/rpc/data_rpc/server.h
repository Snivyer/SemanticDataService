#pragma once

#include <arrow/api.h>
#include <arrow/flight/api.h>
#include "manager/rpc/data_rpc/reader.h"


namespace flight = arrow::flight;
using arrow::Status;

namespace SDS {

    class BasicDataServer : public flight::FlightServerBase 
    {
    public:
        const flight::ActionType kActionSpaceRemove{"space_remove", "Space remove!"};
        const flight::ActionType kActionSpaceFilter{"space_filter", "Space Filter."};
        ~BasicDataServer();

        static std::shared_ptr<BasicDataServer> Create(std::string ip);

        Status ListFlights(const flight::ServerCallContext &, 
                            const flight::Criteria*,
		                    std::unique_ptr<flight::FlightListing> *listings) override;

        Status GetFlightInfo(const flight::ServerCallContext &context, 
                                const flight::FlightDescriptor &descriptor,
                                std::unique_ptr<flight::FlightInfo> *info) override;

        Status DoPut(const flight::ServerCallContext &,
						std::unique_ptr<flight::FlightMessageReader> reader,
						std::unique_ptr<flight::FlightMetadataWriter>) override;

        Status DoGet(const flight::ServerCallContext &context,
                        const flight::Ticket &request,
                        std::unique_ptr<flight::FlightDataStream> *stream) override; 


        arrow::Result<flight::FlightInfo> MakeFlightInfo(const ContentID &cntID, int ticket);

        Status Connect(std::string ip, int64_t port);
        
        Status RegisterWithSpaceFilterModel(std::string spaceID);
        Status WithdrawWithSpaceRemoveModel(std::string spaceID);

        Status PrepareSend(const ContentID &cntID, DataboxObject* dbObject);
        Status WithdrawSend(const ContentID &cntID);


        bool DeleteDB(const ContentID &cntID);
        bool BeLive();

        int getRunPort();
        std::string getRunIp();
        std::unique_ptr<flight::FlightListing> listFlights();



    private:
        class Impl;
        std::shared_ptr<Impl> impl_;
        explicit BasicDataServer(std::shared_ptr<Impl> impl);

    };

}

