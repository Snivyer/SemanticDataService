#include "manager/semanticspace/semantic_space.h"


namespace SDS
{

    SemanticSpaceManager::SemanticSpaceManager() {   
        _metaStore = new MetaStore();
        _metaManager = new ContentMeta();
        _spaceIndex = new SpaceIndex();
        _timeIndex = new TimeIndex();
        _varIndex = new VarIndex();
        _metaManager->setMetaStore(_metaStore);
        _metaManager->setSSDescFormat();

        // toDO: 从存储系统中加载已有的语义空间


        // toDO: 从存储系统中加载已有的索引结构




    }

    SemanticSpaceManager::~SemanticSpaceManager() {

        // toDO: 将新创建和修改的语义空间写回到存储系统中
        auto start = _spaceIDMap.begin();
        while(start != _spaceIDMap.end()) {
            if(start->second->status != SpaceStatus::read) {
                _metaManager->putSSDesc(start->second->cntDesc, start->second->SSName, start->second->getCompleteSpaceID());
            }
            start++;
        }

        _metaManager->putMetaWithCSV("./metadata/SSDesc/");
        _spaceIDMap.clear();
        _spaceNameMap.clear();


        // toDO: 将更新后的索引结构更新回存储系统中
        delete _spaceIndex;
    }


    // create a semantic space
    std::string SemanticSpaceManager::createSemanticSpace(std::string SSName, std::vector<std::string> &geoNames)
    {
        SemanticSpace* space;
        space = getSpaceByName(SSName);

        if(space) {
            return space->getCompleteSpaceID();
        }

        space = new SemanticSpace(SSName);
        _metaManager->extractSSDesc(space->cntDesc, geoNames);

        // choose the adimistrator code as the search term
        SearchTerm term;
        ResultSet result;
        SpaceNode* node;
        std::string adcode = removeTrailingZeros(space->cntDesc.ssDesc.adCode);
        term.push_back(adcode);

        /// insert into space index
        if(_spaceIndex->insert(term, result)) {
            if(_spaceIndex->getResult(result, node)) {
                space->init(node);
                _spaceNameMap.insert({SSName, space});
                _spaceIDMap.insert({space->getCompleteSpaceID(), space});
                return space->getCompleteSpaceID();
            }  
        } else {
            if(_spaceIndex->getResult(result, node)) {
                space->init(node, SpaceStatus::read);
                _spaceNameMap.insert({SSName, space});
                _spaceIDMap.insert({space->getCompleteSpaceID(), space});
                return node->getCompleteSpaceID();
            }
        }
        return "";
    }

    void SemanticSpaceManager::printSpaceDesc(SemanticSpace* space) {
        
        std::cout << "---------------------------------" << std::endl;
        std::cout << "语义空间名为:"  << space->SSName.data() << std::endl;
        std::cout << "语义空间ID为:" << space->getCompleteSpaceID().data() << std::endl;
        space->cntID.print();
        _metaManager->printSSDesc(space->cntDesc);
        std::cout << "---------------------------------" << std::endl;
    }

    ContentMeta* SemanticSpaceManager::getContentMeta() {
        return _metaManager;
    }
     

    SpaceIndex* SemanticSpaceManager::getSpaceIndex() {
        return _spaceIndex;
    }

    TimeIndex* SemanticSpaceManager::getTimeIndex() {
        return _timeIndex;
    }
     
    VarIndex* SemanticSpaceManager::getVarIndex() {
        return _varIndex;
    }

    SemanticSpace* SemanticSpaceManager::getSpaceByName(std::string spaceName) {
        auto ret = _spaceNameMap.find(spaceName);
        if(ret != _spaceNameMap.end()) {
            return ret->second;
        }
        return nullptr;
    }
    
    SemanticSpace* SemanticSpaceManager::getSpaceByID(std::string spaceID) {
        auto ret = _spaceIDMap.find(spaceID);
        if(ret != _spaceIDMap.end()) {
            return ret->second;
        }
        return nullptr;
    }

    bool SemanticSpaceManager::createTimeIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath) {
        
        SemanticSpace* space = getSpaceByID(spaceID);
        TimeSlotNode* node = nullptr;
       
        _metaManager->setAdaptor(adaptor);
        if(_metaManager->extractTSDesc(space->cntDesc, dirPath)) {
            auto tsDesc = space->cntDesc.tsDesc;
            time_t reportT = mktime(&(tsDesc.reportT));
    
            if( _timeIndex->insert(reportT, node)) {
                TimeList *timeList = new TimeList();
                time_t startTime = mktime(&(tsDesc.startT));
                timeList->endTime = mktime(&(tsDesc.endT));

                for(int i = 0; i < tsDesc.count; i++) {
                    timeList->insertTimestamp(startTime);
                    startTime += tsDesc.interval;
                }

                node->insertTimeList(tsDesc.interval,timeList);
                space->cntID.setTimeID(node->getTimeSlotID());
                return true;
            }
        }

        return false;
    }


    bool SemanticSpaceManager::createVarIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath, std::string groupName) {
        SemanticSpace* space = getSpaceByID(spaceID);

        // choose the adimistrator code as the search term
        VarListNode* node = nullptr;

        _metaManager->setAdaptor(adaptor);
        if(_metaManager->extractVLDesc(space->cntDesc, dirPath)) {
            if(_varIndex->insert(groupName, node)) {

                auto varListDesc = space->cntDesc.vlDesc;
                space->cntDesc.vlDesc.groupName = groupName;
                for(auto item : varListDesc.desc) {
                    node->insertVarList(item.varName);
                }
                node->varNum = varListDesc.desc.size();
                space->cntID.setVarID(node->getVarListID());
                return true;
            }
        }
        return false;
    }
}