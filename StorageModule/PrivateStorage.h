#pragma once

#include <string>
#include <unordered_map>

#include "IStorage.h"
#include "../LoggerModule/Logger.h"

namespace StorageModule {
	class PrivateStorage
	{
	public:
		PrivateStorage(const std::string& folderPath);
		~PrivateStorage() = default;
		
		void createStorage(const std::string& name);
		void removeStorage(const std::string& name);

		IStorage& getStorage(const std::string& name);
	private:
		LoggerModule::LoggerConfig config;
		std::string appFolder;
		std::unordered_map<std::string, IStorage> storages;
	};
}
