/* Author: Snivyer
*  Create on: 2024/3/26
*  Description:
    define various content metadata template
*/

#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "abstract/utils/string_operation.h"

namespace SDS {

    // 存储系统类型
    enum StoreSpaceKind 
    {
        Ceph,
        Lustre,
        BB,
        Local,
        None,
    };




    // 存储站点信息
    struct StoreSite {
        std::string siteName;
        std::string siteVal;
        std::vector<StoreSite*> subSites;

        StoreSite* addChildSite(std::string siteName, std::string siteVal) {
            StoreSite* site = new StoreSite;
            site->siteName = siteName;
            site->siteVal = siteVal;
            subSites.push_back(site);
            return site;
        }


    };



    // 存储系统连接符
    struct ConnectConfig {
        std::string userName;
        std::string confFile;        
        std::string poolName;
        std::string rootPath;

        void setConfig(ConnectConfig &config) {
            userName = config.userName;
            confFile = config.confFile;
            poolName = config.poolName;
            rootPath = config.rootPath;
        }
    };

    // 文件路径列表
    struct FilePathList
    {
        std::string dirPath;
        std::vector<StoreSite*> sites;
        std::string sitePath;
        std::vector<std::string> fileNames;
        std::string fileType;

        FilePathList() {}
        
        FilePathList(FilePathList *list) {
            dirPath = list->dirPath;
            sitePath = list->sitePath;
            for(auto ptr: list->sites) {
                sites.push_back(ptr);
            }
        }

        std::string getCompletePath() {
            std::string path = combinePath(dirPath, sitePath);
            return path;
        }

        bool extractStoreSiteDesc(std::string path) {
            std::vector<std::string> siteNames = splitString(path, '/');
            if(siteNames.size() == 0) {
                return false;
            }
    
            StoreSite* site = new  StoreSite();
            site->siteName = siteNames[0];
            site->siteVal = siteNames[0];
            sites.push_back(site);
            sitePath = path;
            for(int i = 1; i < siteNames.size(); i++) {
                site = site->addChildSite(siteNames[i], siteNames[i]);
            }
            return true;
        }

        void printWithTreeModel(StoreSite *site, int &level) {
            for(int i = 0; i < level; i++) {
                std::cout << "  ";
            }

            std::cout << "├─ " << site->siteName.data() << "(" << site->siteVal.data() << ")" << std::endl;
            for(auto subSite : site->subSites) {
                level += 1;
                printWithTreeModel(subSite, level);
            }
        }

        void printWithTreeModel() {
            std::cout << "  "; 
            std::cout << "├─ " << "根目录" << "(" + dirPath + ")" << std::endl;

            int level = 2;

            for(auto site : sites) {
                printWithTreeModel(site, level);
            }

            for(auto fileName : fileNames) {
                for(int i = 0; i < level + 1; i++) {
                    std::cout << "  "; 
                }
                std::cout << "├─ " << fileName << std::endl;
            }
        }

    };

    // 存储描述符
    struct SystemDesc
    {
        ConnectConfig conConf;
        FilePathList  fileList;

        void print() {
            std::cout << "连接配置池名为:"  << conConf.poolName.data() << std::endl;
            std::cout << "连接配置根路径为:"  << conConf.rootPath.data() << std::endl;
        }
    };

    // 存储元数据
    struct StoreDesc
    {
        std::string SSName;
        bool writable;
        size_t size;
        size_t capacity;
        StoreSpaceKind  kind;
        SystemDesc sysDesc;

        StoreDesc():size(0),capacity(2) {};

        std::string getStoreKind() {
            switch(kind) {
                case StoreSpaceKind::Ceph:
                    return "Ceph";
                case StoreSpaceKind::Lustre:
                    return "Lustre";
                case StoreSpaceKind::BB:
                    return "BB";
                case StoreSpaceKind::Local:
                    return "Local";
                default:
                    return "None";
            }
        }


        void print() {
            std::cout << "存储空间名为:"  << SSName.data() << std::endl;
            std::cout << "存储空间大小为:"  << size << std::endl;
            std::cout << "存储空间容量为:"  << capacity << std::endl;
            std::cout << "存储空间类型为:"  << getStoreKind() << std::endl;

            switch (writable) {
                case true:
                    std::cout << "存储空间状态为:可写" << std::endl;
                    break;
                case false:
                    std::cout << "存储空间状态为:只读" << std::endl;
                    break;
                default:
                    std::cout << "存储空间状态为:未知" << std::endl;
                    break;
            }
            sysDesc.print();
        }

        void printWithTreeModel() {
            std::cout << SSName.data() << "(" << getStoreKind() << ")" << std::endl;
            sysDesc.fileList.printWithTreeModel();
        }
    };

    // 存储模板
    struct StoreTemplate
    {
        std::string SSName;
        bool writable;
        StoreSpaceKind kind;
        struct ConnectConfig connConf;
        size_t spaceSize;

        StoreTemplate() {
            spaceSize = 100;
            writable = true;
            connConf.rootPath = "";
            connConf.poolName = "";
            connConf.userName = "";
            connConf.confFile = "";
        }

        void setStoreKind(std::string type) {
            if( type == "Ceph") {
                kind = StoreSpaceKind::Ceph;
            } else if( type == "Lustre") {
                kind = StoreSpaceKind::Lustre;
            } else if( type == "BB") {
                kind = StoreSpaceKind::BB;
            } else if( type == "Local") {
                kind = StoreSpaceKind::Local;
            } else {
                kind = StoreSpaceKind::None;
            }
        }

        void setPath(std::string path) {
            if(kind == StoreSpaceKind::Ceph) {
                connConf.poolName = path;
            } else if(kind == StoreSpaceKind::Lustre) {
                connConf.rootPath = path;
            } else if(kind == StoreSpaceKind::Local) {
                connConf.rootPath = path;
            } else {
                // todo: 判断是否只有这些存储系统
            }
        }
    };

}