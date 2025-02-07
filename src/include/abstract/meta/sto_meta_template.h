/* Author: Snivyer
*  Create on: 2024/3/26
*  Description:
    define various content metadata template
*/

#pragma once
#include <vector>
#include <string>
#include <iostream>

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

    // 存储系统连接符
    struct ConnectConfig
    {
      
        std::string userName;
        std::string confFile;         // conf_file文件，用于配置系统的IP、端口和密钥

        // Ceph
        std::string poolName;
        // Luster
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
        std::vector<std::string> fileNames;
    };

    // 存储描述符
    struct SystemDesc
    {
        struct ConnectConfig conConf;
        struct FilePathList  fileList;

        void print() {
            std::cout << "---------------------------------" << std::endl;
            std::cout << "连接配置池名为:"  << conConf.poolName.data() << std::endl;
            std::cout << "连接配置根路径为:"  << conConf.rootPath.data() << std::endl;
            std::cout << "---------------------------------" << std::endl;
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
        struct SystemDesc sysDesc;

        StoreDesc():size(0),capacity(2) {};

        void print() {
            std::cout << "存储空间名为:"  << SSName.data() << std::endl;
            std::cout << "存储空间大小为:"  << size << std::endl;
            std::cout << "存储空间容量为:"  << capacity << std::endl;

            switch(kind) {
                case StoreSpaceKind::Ceph:
                    std::cout << "存储空间类型为:Ceph" << std::endl;
                    break;
                case StoreSpaceKind::Lustre:
                    std::cout << "存储空间类型为:Lustre" << std::endl;
                    break;
                case StoreSpaceKind::BB:
                    std::cout << "存储空间类型为:BB" << std::endl;
                    break;
                case StoreSpaceKind::Local:
                    std::cout << "存储空间类型为:Local" << std::endl;
                    break;
                default:
                    std::cout << "存储空间类型为:None" << std::endl;
                    break;
            }

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