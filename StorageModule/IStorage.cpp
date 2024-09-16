#include "pch.h"
#include "IStorage.h"

using namespace StorageModule;

IStorage::IStorage(const std::string& path) : filePath(path)
{
	if (fileStorage.is_open()) {
		throw std::runtime_error("Failed to open storage file!");
	}

	fileStorage = std::fstream(path, std::fstream::in | std::fstream::out);
	readStorageFile();
}

IStorage::~IStorage()
{
	if (fileStorage.is_open()) {
		fileStorage.close();
	}
	else {}
}

std::string IStorage::getStringValue(const std::string& key)
{
	auto it = storage.find(key);
	if (it == storage.end()) {
		throw std::invalid_argument("Key doesn't exist!");
	}
	return it->second;
}

bool IStorage::getBooleanValue(const std::string& key)
{
	return getStringValue(key) == "true" or getStringValue(key) == "0";
}

bool IStorage::addValue(const std::string& key, const std::string& value)
{
	auto result = storage.insert({ key, value });
	if (!result.second) {
		throw std::invalid_argument("Key already exists!");
	}
	return true;
}

bool IStorage::removeValue(const std::string& key, bool isKey = true)
{
	return isKey ? eraseByKey(key) : eraseByValue(key);
}

bool IStorage::eraseByKey(const std::string& key)
{
	if (!storage.erase(key)) throw std::invalid_argument("Key doesn't exist!");

	return true;
}

bool IStorage::eraseByValue(const std::string& key)
{
	auto it = FIND_VALUE(storage, key);
	if (it == std::end(storage)) throw std::invalid_argument("Value doesn't exist!");
	storage.erase(it);

	return true;
}

/*	Простенькое чтение зашифрованного файла с данными. 
	В перспективе - можно где нужно - переопределить алгоритм. 
*/
void IStorage::readStorageFile()
{
	if (!fileStorage.is_open()) {
		throw std::runtime_error("Storage file not opened!");
	}

	std::string encryptedLine;

	while (std::getline(fileStorage, encryptedLine)) {
		std::hash<std::string> hash_fn;
		std::size_t dynamic_key = hash_fn(encryptedLine);

		std::string decryptedLine = encryptedLine;
		for (std::size_t i = 0; i < decryptedLine.size(); ++i) {
			decryptedLine[i] ^= (dynamic_key >> (i % 8));
		}

		auto delimiterPos = decryptedLine.find('=');
		if (delimiterPos == std::string::npos) {
			throw std::runtime_error("Invalid line format in file!");
		}

		std::string key = decryptedLine.substr(0, delimiterPos);
		std::string value = decryptedLine.substr(delimiterPos + 1);

		storage[key] = value;
	}
}

void IStorage::saveStorageFile()
{
	if (!fileStorage.is_open()) {
		throw std::runtime_error("Storage file not opened!");
	}

	fileStorage.seekp(0, std::ios::beg);
	fileStorage.clear();
	fileStorage.close();

	fileStorage.open(filePath, std::fstream::out | std::fstream::trunc);

	if (!fileStorage.is_open()) {
		throw std::runtime_error("Failed to open storage file for writing!");
	}

	for (const auto& pair : storage) {
		std::string line = pair.first + "=" + pair.second;

		std::hash<std::string> hash_fn;
		std::size_t dynamic_key = hash_fn(line);

		std::string encryptedLine = line;
		for (std::size_t i = 0; i < encryptedLine.size(); ++i) {
			encryptedLine[i] ^= (dynamic_key >> (i % 8));
		}

		fileStorage << encryptedLine << std::endl;
	}

	fileStorage.flush();
	fileStorage.close();

	fileStorage = std::fstream(filePath, std::fstream::in | std::fstream::out);
}
