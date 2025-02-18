#pragma once
#include "abstract/adaptor/NC_pluge.h"
#include "abstract/adaptor/HDF_pluge.h"
#include "abstract/meta/sto_meta_template.h"


namespace SDS
{
    class PlugeFactory {
        public:

        static SDS::NCPluge* getNCPluge();
        static SDS::HDFPluge* getHDFPluge(); 
    };


}