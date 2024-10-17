#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <string_view>
#include <map>
#include <array>
#include <optional>

#include <cryptopp/aes.h>

struct Record
{
	std::string name, login, password, description;
};


// Эти перегрузки необходимы для корректной работы 
// юнит тестов.
bool operator==(const Record& l, const Record& r);
bool operator!=(const Record& l, const Record& r);

// Перегрузка оператора вывода для структуры Record.
// Выводит только имя и описание. Описание не шире 
// WIDTH_OUT_STRING (из файла parser.hpp).
std::ostream& operator<<(std::ostream& out, const Record& rec);

// Ширина вывода описания
const size_t WIDTH_OUT_STRING = 75;

// Выводит строку des в ширину не длиннее WIDTH_OUT_STRING (75)
void outDescription(std::ostream& out, std::string_view des);

class PasswordManager
{
private:
	using DataStorage = std::map<std::string, Record>;
	using DataIter = DataStorage::iterator;
	
	// ПУть до файла-хранилище. В данный файл запишутся все данные после окончания
	// работы с менеджером. Нужна для удобства и дальнейшего расширения программы.
	std::filesystem::path fileStorage;

	// Хранилище записей. Ключом выступает копия имени записи.
	DataStorage data;
	
	// Текущая запись. Последняя запись, которую искали методами 
	// getRecordByName, getRecordByNumber. Используется для быстрой 
	// работы с интересующей нас записью.
	DataIter currRec;

	// Строка, в которую записываются данные из файла и из которой
	// данные записываются в файл.
	std::string fileData;

	// ключ шифрования и вектор инициализации
	std::array<unsigned char, CryptoPP::AES::MAX_KEYLENGTH> aesKey;
	std::array<unsigned char, CryptoPP::AES::BLOCKSIZE> cbcIv;

	// Хэш мастер ключа посчитанный с помощью SHA256
	std::string hashMasterKey;

	// Флаг, говорящий о том, что правильный мастер ключ был введён.
	bool rightMasterKey;

	// Получение итератора по номеру.
	// Проверка номера на выход за пределы размера возлагаются на вызывающую функцию.
	DataIter getIterByNumber(size_t number);

	// Вычисляет из мастер ключа хэш значение с помощью SHA384 и из него получает
	// ключ шифрования (первые 256 бит хэша) и вектор инициализации (оставшиеся 128 бит хэша).
	void createKeyAndIv(std::string masterKey);

	// Из строки fileData заполняет словарь записей data.
	// В строке записи разделены символов ';', а поля одной записи разделены пробелами.
	void createStringFromData();

	// Из словаря data создаёт строку и сохраняет результат в fileData.
	// В строке записи разделены символов ';', а поля одной записи разделены пробелами.
	void createDataFromString();

	// Записывает строку fileData в файл fileStorage, открытый в бинарном режиме.
	void writeDataInFile();


	// Расшифровывает/зашифровывает данные из строки fileData с помощью AES256
	// и записывает результат в неё же.
	void decrypt();
	void encrypt();

	// Вычисляет хэш от строки с помощью SHA256 и возвращает результат
	std::string getHashSHA256(const std::string& str);

	// Проверяет, на правильно ли ключе расшифровали. Основана проверка на том,
	// что в начало зашифрованной строки записан вектор инициализации в обратном порядке
	bool checkMasterKey(const std::string& masterKey);

public:
	PasswordManager() = default;

	// В деструкторе зашифровываем данные и записываем их в файл fileStorage.
	// Если data пусто из-за того, что пользователь удалил все данные, очищает файл
	// от старых записей.
	// Если data пусто из-за того, что не был введён правильный мастер ключ, то ничего
	// не делает.
	~PasswordManager();

	// Считывание данных из файла. Отдельно и в публичной секции 
	// из-за того, что считывание файла должно происходить параллельно
	// тому, как пользователь вводит мастер пароль.
	// Считывает данные в строку fileData.
	// Сохраняет путь до файла в storageFile.
	// Если файл пуст, то только сохраняет путь до файла в storageFile.
	void readDataFromFile(std::filesystem::path file);

	// Проверяет правильность полученного ключа. Если ключ не правильный возвращает 1.
	// Если файл пуст и, следовательно, создаётся новая база записей, то вычисляет из 
	// ключа хэш c помощью SHA256 и сохраняет в hashMasterKey.
	// Вычисляет ключ шифрования и вектор инициализации. Расшифровывает данные, 
	// полученные из файла, и формирует из них словарь записей data. Присваивает 
	// currRec значение data.end()
	// Если строка fileData пуста, то только вычисляет ключ шифрования и вектор инициализации.
	// При корректной отработке возвращает 0.
	int decryptData(std::string masterKey);


	 /*
		Методы добавления записи.
	 */
	
	// Исторический метод, с ним писались тесты, в самой программе не
	// используется. Создаёт запись из отдельных строк и вызывает вторую
	// функцию добавления записи.
	void addRecord(
		std::string name,
		std::string login,
		std::string password,
		std::string description = "");

	// Добавление записи в словарь data.
	bool addRecord(Record rec);


	/*
		Методы удаления записей.	
	*/
	
	// Удаляет запись по имени.
	// Если записи с таким имени нет, ничего не делает.
	void deleteRecordByName(std::string name);

	// Удаляет запись по номеру.
	// Если записи с таким номер нет, ничего не делает
	void deleteRecordByNumber(size_t number);

	// Удаляет запись из currRec.
	// После удаления currRec = data.end()
	void deleteCurrent();


	/*
		Методы изменения полей текущей записи.
	*/

	// При изменении имени запись со старым именем заменяется
	// записью с новым именем
	void changeNameRecord(std::string newName);
	void changeLoginRecord(std::string newLogin);
	void changePasswordRecord(std::string newPass);
	void changeDescriptionRecord(std::string newDes);


	/*
		Методы получения информации.
	*/

	// Возвращает все имена записей из data.
	std::vector<std::string_view> getNames() const;

	// Получение записи по имени. Если записи с таким нет, возвращает
	// пустую запись.
	std::optional<Record> getRecordByName(std::string name);

	// Получение записи по номеру. Если записи под таким номером нет, 
	// возвращает пустую запись.
	std::optional<Record> getRecordByNumber(size_t number);
	
	const std::string& getCurrentRecordName() const;
	const std::string& getCurrentRecordLogin() const;
	const std::string& getCurrentRecordPassword() const;
	const std::string& getCurrentRecordDescription() const;


	// Метод получения текущего количества записей
	size_t numRecords() const;

	// Метод очищает словарь data.
	void clearData();
};