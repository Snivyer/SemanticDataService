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
                _metaManager->putSSDesc(start->second->ssDesc, start->second->SSName, start->second->getCompleteSpaceID());
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
        _metaManager->extractSSDesc(space->ssDesc, geoNames);

        // choose the adimistrator code as the search term
        SearchTerm term;
        ResultSet result;
        SpaceNode* node;
        std::string adcode = removeTrailingZeros(space->ssDesc.adCode);
        term.push_back(adcode);

        /// insert into space index
        if(_spaceIndex->insert(term, result)) {
            if(_spaceIndex->getResult(result, node)) {
                space->init(node);
                _spaceNameMap.insert({SSName, space});
                _spaceIDMap.insert({space->getCompleteSpaceID(), space});
                autoReferDataBox(space);        
                return space->getCompleteSpaceID();
            }  
        } else {
            if(_spaceIndex->getResult(result, node)) {
                space->init(node, SpaceStatus::read);
                _spaceNameMap.insert({SSName, space});
                _spaceIDMap.insert({space->getCompleteSpaceID(), space});
                autoReferDataBox(space);
                return node->getCompleteSpaceID();
            }
        }
        return "";
    }

    void SemanticSpaceManager::printSpaceDesc(SemanticSpace* space) {
        
        std::cout << "---------------------------------" << std::endl;
        std::cout << "语义空间名为:"  << space->SSName.data() << std::endl;
        std::cout << "语义空间ID为:" << space->getCompleteSpaceID().data() << std::endl;
        _metaManager->printSSDesc(space->ssDesc);
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

    void SemanticSpaceManager::autoReferDataBox(SemanticSpace* space) {
        auto childNodes = space->indexNode->getChildNodes();
        for(auto node: childNodes) {
            auto childSpace = getSpaceByID(node->getCompleteSpaceID());
            if(childSpace) {
                for(auto item : childSpace->databoxsIndex) {
                    space->databoxsIndex.insert({item.first, item.second});
                    space->databoxNum += 1;
                    autoAddDataBox(space, item.first, item.second);
                }
            }
        }
    }

    bool SemanticSpaceManager::autoAddDataBox(SemanticSpace* space, const ContentID &cntID, ContentDesc &cntDesc) {
        auto parentNode = space->indexNode->moveUp();
        while(parentNode) {
            auto parentSpace = getSpaceByID(parentNode->getCompleteSpaceID());
            if(parentSpace) {
                parentSpace->databoxsIndex.insert({cntID, cntDesc});
                parentSpace->databoxNum += 1;
                parentNode = parentNode->moveUp();
            } else {
                return true;
            }
        }
    }


    bool SemanticSpaceManager::createDataBoxIndex(Adaptor* adaptor, std::string spaceID, std::string dirPath) {
        // create a new data box 
        ContentID cntID;
        ContentDesc cntDesc;
        SemanticSpace* space = getSpaceByID(spaceID);
        cntID.setSpaceID(spaceID);
        

        // create a time index
        auto TSRet = createTimeIndex(adaptor, space, dirPath, cntID, cntDesc.tsDesc);

        // create a var index
        auto VLRet = createVarIndex(adaptor, space, dirPath, cntID, cntDesc.vlDesc);

        if(TSRet && VLRet) {
            cntDesc.setSpaceDesc(space->ssDesc);
            
            // add the databox into the semantic space
            space->databoxsIndex.insert({cntID, cntDesc});
            space->databoxNum += 1;

            // add the databox into parent semantic space
            autoAddDataBox(space, cntID, cntDesc);

            return true;
        } else {
            return false;
        }

    }

    bool SemanticSpaceManager::createTimeIndex(Adaptor* adaptor, SemanticSpace* space, std::string dirPath,
                                                ContentID &cntID, TSDesc &tsDesc) {
        
        TimeSlotNode* node = nullptr;
        _metaManager->setAdaptor(adaptor);
        if(_metaManager->extractTSDesc(tsDesc, dirPath)) {
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
                cntID.setTimeID(node->getTimeSlotID());
                return true;
            }
        }

        return false;
    }

    bool SemanticSpaceManager::createVarIndex(Adaptor* adaptor, SemanticSpace* space, std::string dirPath, 
                                                ContentID &cntID, VLDesc &vlDesc) {
        
        // choose the adimistrator code as the search term
        VarListNode* node = nullptr;
        _metaManager->setAdaptor(adaptor);
        if(_metaManager->extractVLDesc(vlDesc, dirPath)) {
            if(_varIndex->insert(vlDesc.groupName, node)) {
                for(auto item : vlDesc.desc) {
                    node->insertVarList(item.varName);
                }
                node->varNum = vlDesc.desc.size();
                cntID.setVarID(node->getVarListID());
                return true;
            } else {
                _varIndex->search(vlDesc.groupName, node);
                cntID.setVarID(node->getVarListID());
                return true;
            }
        }
        return false;
    }
}