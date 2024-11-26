/* Author: Snivyer
*  Create on: 2024/11/18
*  Description:
   like raylet in ray, SDSlet provide the local semantic data service for raydata client
*/

#pragma once

#include "API/dataspace/databox_store.h"
#include "API/dataspace/meta_service.h"
#include "abstract/adaptor/local_adaptor.h"

#include <signal.h>

using namespace SDS;
using arrow::Status;
using namespace std;

namespace SDSlet
{
   class SDSlet {
      public:
         ~SDSlet();
         static std::shared_ptr<SDSlet> createSDSlet(int64_t shareMemorySize, std::string storeSocketName, std::string metaSocketName);

         Status init();
         Status run();
         
      private:
         class Impl;
         std::shared_ptr<Impl> impl_;
         explicit SDSlet(std::shared_ptr<Impl> impl);
         Status createRpcServer(int port = 4444);
         Status createRpcDataServer(std::string ip, int port, int serverPort = 4444);
         Status createDBStoreServer();
         Status createMetaServer();
         

   };
}