#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

#define FIND_VALUE(storage, key) \
	std::find_if(std::begin(storage), std::end(storage), \
		[&key](auto&& p) { return p.second == key; })

namespace StorageModule {
	/*
	* Базовый класс для хранилища.
	*/
	class IStorage
	{
	public:
		explicit IStorage(const std::string& path);
		virtual ~IStorage();

		virtual std::string getStringValue(const std::string& key);
		virtual bool getBooleanValue(const std::string& key);

		virtual bool addValue(const std::string& key, const std::string& value);
		virtual bool removeValue(const std::string& key, bool isKey = true);

	protected:
		bool eraseByKey(const std::string& key);
		bool eraseByValue(const std::string& key);

		void readStorageFile();
		void saveStorageFile();

		std::unordered_map<std::string, std::string> storage;
	private:
		std::fstream fileStorage;
		std::string filePath;
	};
}
