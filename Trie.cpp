#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

using namespace std;

const int ALPHABET_SIZE = 26;

struct TrieNode {
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;

    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
private:
    TrieNode* root;
    size_t nodeCount;

    void clear(TrieNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i] != nullptr) {
                clear(node->children[i]);
            }
        }
        delete node;
        nodeCount--;
    }

    bool hasChildren(TrieNode* node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i] != nullptr) return true;
        }
        return false;
    }

    TrieNode* removeHelper(TrieNode* current, const string& key, size_t depth, bool& success) {
        if (current == nullptr) return nullptr;

        if (depth == key.size()) {
            if (current->isEndOfWord) {
                current->isEndOfWord = false;
                success = true;
            }
            if (!hasChildren(current)) {
                delete current;
                nodeCount--;
                current = nullptr;
            }
            return current;
        }

        int index = key[depth] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE) {
            success = false;
            return current;
        }

        current->children[index] = removeHelper(current->children[index], key, depth + 1, success);

        if (!hasChildren(current) && !current->isEndOfWord) {
            delete current;
            nodeCount--;
            current = nullptr;
        }
        return current;
    }

    void collectWords(TrieNode* current, string currentPrefix, vector<string>& results) {
        if (current->isEndOfWord) {
            results.push_back(currentPrefix);
        }
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (current->children[i] != nullptr) {
                char nextChar = 'a' + i;
                collectWords(current->children[i], currentPrefix + nextChar, results);
            }
        }
    }

    string toLowerCaseAndClean(const string& str) {
        string result = "";
        for (char ch : str) {
            if (isalpha(ch)) {
                result += tolower(ch);
            }
        }
        return result;
    }

public:
    Trie() {
        root = new TrieNode();
        nodeCount = 1;
    }

    ~Trie() {
        clear(root);
    }

    bool insert(string key) {
        key = toLowerCaseAndClean(key);
        if (key.empty()) return false;

        TrieNode* current = root;
        for (char ch : key) {
            int index = ch - 'a';
            if (index < 0 || index >= ALPHABET_SIZE) return false;

            if (current->children[index] == nullptr) {
                current->children[index] = new TrieNode();
                nodeCount++;
            }
            current = current->children[index];
        }
        if (current->isEndOfWord) {
            return false;
        }
        current->isEndOfWord = true;
        return true;
    }

    bool search(string key) {
        key = toLowerCaseAndClean(key);
        if (key.empty()) return false;

        TrieNode* current = root;
        for (char ch : key) {
            int index = ch - 'a';
            if (index < 0 || index >= ALPHABET_SIZE) return false;
            if (current->children[index] == nullptr) return false;
            current = current->children[index];
        }
        return current->isEndOfWord;
    }

    bool remove(string key) {
        key = toLowerCaseAndClean(key);
        if (key.empty()) return false;
        bool success = false;
        root = removeHelper(root, key, 0, success);
        if (root == nullptr) {
            root = new TrieNode();
            nodeCount = 1;
        }
        return success;
    }

    vector<string> getAutoSuggestions(string prefix) {
        prefix = toLowerCaseAndClean(prefix);
        vector<string> results;
        TrieNode* current = root;

        for (char ch : prefix) {
            int index = ch - 'a';
            if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == nullptr) {
                return results;
            }
            current = current->children[index];
        }

        collectWords(current, prefix, results);
        return results;
    }

    vector<string> getAllWords() {
        vector<string> results;
        collectWords(root, "", results);
        return results;
    }

    void loadFromTextFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка открытия файла: " << filename << endl;
            return;
        }

        string word;
        int count = 0;
        while (file >> word) {
            if (insert(word)) {
                count++;
            }
        }
        file.close();
        cout << "Успешно импортировано новых слов из файла: " << count << endl;
    }

    size_t getNodeCount() const {
        return nodeCount;
    }

    size_t getMemoryUsage() const {
        return nodeCount * sizeof(TrieNode);
    }
};

void manageDictionary(string dictName, Trie& dictionary) {
    int choice;
    string word, prefix, filename;

    while (true) {
        cout << "\nУПРАВЛЕНИЕ СЛОВАРЕМ: " << dictName << "" << endl;
        cout << "1. Добавить слово в словарь" << endl;
        cout << "2. Найти слово в словаре" << endl;
        cout << "3. Удалить слово из словаря" << endl;
        cout << "4. Поиск по префиксу (Автодополнение)" << endl;
        cout << "5. Просмотреть весь словарь" << endl;
        cout << "6. Импортировать слова из текстового файла" << endl;
        cout << "7. Сравнить расход памяти со стандартными контейнерами" << endl;
        cout << "0. Вернуться в главное меню" << endl;
        cout << "Выберите действие: ";

        if (!(cin >> choice)) {
            cout << "Некорректный ввод!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите слово для добавления (английские буквы): ";
            cin >> word;
            if (dictionary.insert(word)) {
                cout << "Слово \"" << word << "\" добавлено." << endl;
            }
            else {
                cout << "Не удалось добавить слово (возможно, оно уже есть или содержит некорректные символы)." << endl;
            }
            break;

        case 2:
            cout << "Введите слово для поиска: ";
            cin >> word;
            if (dictionary.search(word)) {
                cout << "Результат: Слово \"" << word << "\" НАЙДЕНО." << endl;
            }
            else {
                cout << "Результат: Слово \"" << word << "\" НЕ НАЙДЕНО." << endl;
            }
            break;

        case 3:
            cout << "Введите слово для удаления: ";
            cin >> word;
            if (dictionary.remove(word)) {
                cout << "Слово \"" << word << "\" удалено." << endl;
            }
            else {
                cout << "Слово не найдено в словаре." << endl;
            }
            break;

        case 4:
            cout << "Введите префикс для автодополнения: ";
            cin >> prefix;
            {
                vector<string> suggestions = dictionary.getAutoSuggestions(prefix);
                if (suggestions.empty()) {
                    cout << "Вариантов автодополнения не найдено." << endl;
                }
                else {
                    cout << "Найденные варианты (" << suggestions.size() << "):" << endl;
                    for (const auto& s : suggestions) {
                        cout << "  - " << s << endl;
                    }
                }
            }
            break;

        case 5:
        {
            vector<string> allWords = dictionary.getAllWords();
            if (allWords.empty()) {
                cout << "Словарь пуст." << endl;
            }
            else {
                cout << "Содержимое словаря (" << allWords.size() << " слов):" << endl;
                for (const auto& w : allWords) {
                    cout << "  " << w << endl;
                }
            }
        }
        break;

        case 6:
            cout << "Введите имя файла (например, words.txt): ";
            cin >> filename;
            dictionary.loadFromTextFile(filename);
            break;

        case 7:
        {
            vector<string> allWords = dictionary.getAllWords();
            size_t wordCount = allWords.size();
            size_t trieMemory = dictionary.getMemoryUsage();

            size_t averageWordLen = 0;
            for (const auto& w : allWords) {
                averageWordLen += w.size();
            }
            if (wordCount > 0) averageWordLen /= wordCount;

            size_t stdStringOverhead = sizeof(string) + (averageWordLen > 15 ? averageWordLen : 0);
            size_t setNodeSize = 3 * sizeof(void*) + sizeof(int) + stdStringOverhead;
            size_t setMemory = wordCount * setNodeSize;

            size_t unorderedSetNodeSize = sizeof(void*) + stdStringOverhead;
            size_t unorderedSetMemory = (wordCount * unorderedSetNodeSize) + (wordCount * sizeof(void*));

            cout << "\nАНАЛИЗ РАСХОДА ПАМЯТИ ДЛЯ ТЕКУЩЕГО СЛОВАРЯ" << endl;
            cout << "Текущее количество слов в словаре: " << wordCount << endl;
            cout << "Количество созданных узлов префиксного дерева: " << dictionary.getNodeCount() << endl;
            cout << "1. Префиксное дерево (Trie):       " << trieMemory << " байт (~" << trieMemory / 1024.0 << " Кб)" << endl;
            cout << "2. Контейнер std::set (РБ-дерево): " << setMemory << " байт (~" << setMemory / 1024.0 << " Кб)" << endl;
            cout << "3. Контейнер std::unordered_set:   " << unorderedSetMemory << " байт (~" << unorderedSetMemory / 1024.0 << " Кб)" << endl;
        }
        break;

        case 0:
            return;

        default:
            cout << "Неверный пункт меню." << endl;
        }
    }
}

int main() {
    setlocale(LC_ALL, "");

    map<string, Trie*> dictionaryManager;
    int mainChoice;
    string dictName;

    while (true) {
        cout << endl;
        cout << "МЕНЕДЖЕР СЛОВАРЕЙ" << endl;
        cout << "1. Создать новый словарь" << endl;
        cout << "2. Удалить существующий словарь" << endl;
        cout << "3. Выбрать словарь" << endl;
        cout << "4. Показать список всех словарей" << endl;
        cout << "0. Выход из программы" << endl;
        cout << "Выберите действие: ";

        if (!(cin >> mainChoice)) {
            cout << "Некорректный ввод! Попробуйте снова." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (mainChoice) {
        case 1:
            cout << "Введите название для нового словаря: ";
            cin >> dictName;
            if (dictionaryManager.find(dictName) != dictionaryManager.end()) {
                cout << "Словарь с таким названием уже существует!" << endl;
            }
            else {
                dictionaryManager[dictName] = new Trie();
                cout << "Словарь \"" << dictName << "\" успешно создан." << endl;
            }
            break;

        case 2:
            cout << "Введите название словаря для удаления: ";
            cin >> dictName;
            {
                auto it = dictionaryManager.find(dictName);
                if (it != dictionaryManager.end()) {
                    delete it->second;
                    dictionaryManager.erase(it);
                    cout << "Словарь \"" << dictName << "\" полностью удален." << endl;
                }
                else {
                    cout << "Словарь с таким названием не найден." << endl;
                }
            }
            break;

        case 3:
            cout << "Введите название словаря для работы: ";
            cin >> dictName;
            {
                auto it = dictionaryManager.find(dictName);
                if (it != dictionaryManager.end()) {
                    manageDictionary(dictName, *(it->second));
                }
                else {
                    cout << "Словарь с таким названием не найден." << endl;
                }
            }
            break;

        case 4:
            if (dictionaryManager.empty()) {
                cout << "На данный момент не создано ни одного словаря." << endl;
            }
            else {
                cout << "Список доступных словарей:" << endl;
                for (const auto& pair : dictionaryManager) {
                    cout << "  - " << pair.first << endl;
                }
            }
            break;

        case 0:
            for (auto& pair : dictionaryManager) {
                delete pair.second;
            }
            return 0;

        default:
            cout << "Неверный пункт меню! Пожалуйста, выберите от 0 до 4." << endl;
        }
    }
}