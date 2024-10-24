#pragma once

#include <memory>
#include <vector>

#include "abstract/meta/cnt_meta_template.h"
#include "abstract/event/events.h"
#include "manager/databox/databox_manager.h"


namespace SDS
{

    struct GetRequest {

        




    }




    class DataBoxStore {
        public:
        ~DataBoxStore();

        
        

        private:
            class Impl;
            std::shared_ptr<Impl> impl_;
            explicit DataBoxStore(std::shared_ptr<_Impl> impl);

    }

   


}