#include "pch.h"
#include "PrivateStorage.h"

using namespace StorageModule;

explicit PrivateStorage::PrivateStorage(const std::string& folderPath) : appFolder(folderPath) {
    config.level = LoggerModule::LogLevel::INFO;
    config.flushInterval = 1000;
}

void PrivateStorage::createStorage(const std::string& name)
{
    std::string filePath = appFolder + "/" + name + ".dat";

    std::ofstream file(filePath);
    if (!file) {
        throw std::runtime_error("Cannot create storage file: " + filePath);
        auto logger = LoggerModule::Logger(config);
        LOG_MESSAGE(LoggerModule::ERROR, "Cannot create storage file: " + filePath, logger)
    }

    file.close();

    IStorage newStorage(filePath);

    storages.emplace(name, std::move(newStorage)); 

    auto logger = LoggerModule::Logger(config);
    LOG_MESSAGE(LoggerModule::ERROR, "Storage file: " + filePath + " created!", logger)

}

void PrivateStorage::removeStorage(const std::string& name)
{
    std::string filePath = appFolder + "/" + name + ".dat";
	storages.erase(name);

    std::remove(filePath.c_str());

    auto logger = LoggerModule::Logger(config);
    LOG_MESSAGE(LoggerModule::ERROR, "Storage file: " + filePath + " has been removed!", logger)
}   

IStorage& PrivateStorage::getStorage(const std::string& name)
{
	return storages.at(name);
}
