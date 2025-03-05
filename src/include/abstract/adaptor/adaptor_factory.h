#pragma once
#include "abstract/adaptor/ceph_adaptor.h"
#include "abstract/adaptor/lustre_adaptor.h"
#include "abstract/adaptor/local_adaptor.h"


namespace SDS
{
    class AdaptorFactory {
    public:
            static Adaptor* getAdaptor(StoreSpaceKind kind, ConnectConfig &config, FilePathList *list) {
                if(kind == StoreSpaceKind::Ceph) {
                    // return new CephAdaptor(config, list);
                } else if (kind == StoreSpaceKind::Lustre) {
                    // return new LustreAdaptor(config, list);
                } else if (kind == StoreSpaceKind::Local) {
                    return new LocalAdaptor(config, list);
                } else {
                    return nullptr;
                }
            }
    };

}