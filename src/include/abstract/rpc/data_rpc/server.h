#include <arrow/api.h>
#include <arrow/flight/api.h>
#include <arrow/filesystem/api.h>

#pragma once

namespace flight = arrow::flight;
using arrow::Status;

namespace SDS {

    class BasicDataServer : public flight::FlightServerBase 
    {
    public:
        ~BasicDataServer();

        Status ListFlights(const flight::ServerCallContext &, 
                            const flight::Criteria*,
		                    std::unique_ptr<flight::FlightListing> *listings) override;

        Status GetFlightInfo(const flight::ServerCallContext &context, 
                                const flight::FlightDescriptor &descriptor,
                                std::unique_ptr<flight::FlightInfo> *info) override;

        Status PollFlightInfo(const flight::ServerCallContext& context,
                                const flight::FlightDescriptor& request,
                                std::unique_ptr<flight::PollInfo>* info) override;

        Status GetSchema(const flight::ServerCallContext& context,
                           const flight::FlightDescriptor& request,
                           std::unique_ptr<flight::SchemaResult>* schema) override;

        Status DoPut(const flight::ServerCallContext &,
						std::unique_ptr<flight::FlightMessageReader> reader,
						std::unique_ptr<flight::FlightMetadataWriter>) override;

        Status DoGet(const flight::ServerCallContext &context,
                        const flight::Ticket &request,
                        std::unique_ptr<flight::FlightDataStream> *stream) override; 

        Status DoExchange(const flight::ServerCallContext& context,
                            std::unique_ptr<flight::FlightMessageReader> reader,
                            std::unique_ptr<flight::FlightMessageWriter> writer) override;

        Status ListActions(const flight::ServerCallContext &context,
                            std::vector<flight::ActionType> *actions) override;

        Status DoAction(const flight::ServerCallContext &context,
                        const flight::Action &action,
                        std::unique_ptr<flight::ResultStream> *result) override;    


    private:
        class Impl;
        std::shared_ptr<Impl> impl_;
        explicit BasicDataServer(std::shared_ptr<Impl> impl);

    };

}

