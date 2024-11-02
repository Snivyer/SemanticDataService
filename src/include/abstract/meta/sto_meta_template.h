/* Author: Snivyer
*  Create on: 2024/3/26
*  Description:
    define various content metadata template
*/

#pragma once
#include <vector>
#include <string>

namespace SDS {

    // 存储系统类型
    enum StoreSpaceKind 
    {
        Ceph,
        Lustre,
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
    };

    // 存储元数据
    struct StoreDesc
    {
        bool writable;
        size_t size;
        size_t capacity;
        StoreSpaceKind  kind;
        struct SystemDesc sysDesc;

        StoreDesc():size(0),capacity(2) {};

    };

    // 存储模板
    struct StoreTemplate
    {
        bool writable;
        StoreSpaceKind kind;
        struct ConnectConfig connConf;
        size_t spaceSize;
    };

}