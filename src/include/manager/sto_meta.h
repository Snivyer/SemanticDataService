/* Author: Snivyer
*  Create on: 2023/11/13
*  Description:
    StorageMetadata is a children class of Metadata class, which is rensponsible for management
    of the storage metadata and construction of address index.
*/

#ifndef STORAGE_METADATA_H_
#define STORAGE_METADATA_H_

#include "meta.h"
#include <string>
#include <vector>

namespace SDS
{

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
    };

    // 文件路径列表
    struct FilePathList
    {
        std::string DirPath;
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
    };

    // 存储模板
    struct StoreTemplate
    {
        bool writable;
        StoreSpaceKind kind;
        struct ConnectConfig connConf;
        size_t spaceSize;
    };


    // 存储元数据管理类
    class StorageMeta: public Metadata
    {
    public:
        StorageMeta();
        ~StorageMeta();

        size_t storageID;
        StoreDesc* desc;

    

        /*------提取元数据------*/
        void extractStoreDesc(struct StoreTemplate &stoT);        // 根据存储模板，提取存储描述符
      

        /*------获取元数据------*/
        StoreDesc getCntMeta(size_t StorageID);



        
    private:


        std::string getMeta(Key key) override;
        void putMeta(Key key, std::string value) override;      



    };


    











}



#endif