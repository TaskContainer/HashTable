#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <algorithm>
#include <locale>
#include <iterator>


// Класс представляет собой модифицируемый динамический массив любого типа с изменяемыми размерами
template<typename T> class List {
private:
    // Приватная структура отвечает за ячейку массива
    struct Each {
        Each *Next = nullptr;
        T Value;
        Each(Each *next, const T &val) : Next(next), Value(val) {}
        // Действую согласно идиоме RAII:
        // Деструктор очистит память конкретной ячейки при её выходе из области видимости
        // Удаление памяти выделенной под следующую ячейку вызовет в свою очередь её деструктор
        // Таким образом вся память будет рекурсивно очищена
        virtual ~Each() { delete Next; }
    } *First = nullptr;

    size_t Length_ = 0;

    // Получить ячейку под номером index
    Each *const GetEach(const size_t index) {
        auto res = First;
        for (int i = 0; i < (Length_ - index - 1); ++i) res = res->Next;
        return res;
    }

    // Удалить ячейку toDelete и прицепить остаток цепочки к ячейке перед удаленной
    void DeleteEach(Each *&before, Each *toDelete) {

        before = toDelete->Next;
        // Чтобы деструктор ячейки при очистке памяти рекурсивно не удалил всю цепочку
        // обнуляем поле указателя на следующую ячейку
        toDelete->Next = nullptr;
        // И удаляем память ячейки
        delete toDelete;
    }

public:
    // Добавить элемент в конец списка
    void Append(const T &val) {
        First = new Each(First, val);
        ++Length_;
    }

    // Удалить элемент по индексу
    void RemoveAt(const size_t index) {
        if (index >= Length_) return;
        if (index == 0) DeleteEach(First, First);
        else {
            auto before = GetEach(index - 1);
            DeleteEach(before->Next, before->Next);
        }
        --Length_;
    }

    // Поменять размер
    void Resize(const size_t newLength, const T &init = T()) {
        if (newLength > Length_) for (auto i = Length_; i <= newLength; ++i) Append(init);
        if (newLength < Length_) delete GetEach(newLength);
        Length_ = newLength;
    }

    // Получить размер
    size_t Length() const { return Length_; }

    // Перегружаем оператор [int] для доступа по индексу
    T &operator[](const size_t index) { return GetEach(index)->Value; }

    // Конструктор пустого списка
    List() = default;

    // Конструктор с прединициализацией элементов
    explicit List(const size_t length, const T &init = T()) {
        for (int i = 0; i < length; ++i) Append(init);
    }

    // При выходе из области видимости 
    // будет автоматически вызвана очистка всех элементов массива через рекурсивные деструкторы ячеек
    virtual ~List() { delete First; }
};



// Класс представляет собой словарь ключ-значение на основе хеш-таблицы
template<typename Key, typename Value> class Map {
private:
    struct Item {
        Value Val_;
        const Key Key_;
    };
    // Заранее зарезервируем память на 256 ячеек
    List<List<Item>> Values{ 256 };
    // Функтор для хеширования
    const std::function<size_t(const Key&)> HashFunction;

public:
    // Добавляет пару ключ-значение
    void Append(const Key &key, const Value &val) {
        const size_t hash = HashFunction(key);
        if (hash > Values.Length()) Values.Resize(hash);
        Values[hash].Append({val, key});
    }

    // Удаляет пару ключ-значение
    void Remove(const Key &key) {
        const size_t hash = HashFunction(key);
        if (hash >= Values.Length()) return;
        auto &list = Values[hash];
        if (list.Length() == 0) return;
        for (size_t i = 0; i < list.Length(); ++i) {
            if (list[i].Key_ == key) {
                list.RemoveAt(i);
                return;
            }
        }
    }

    std::wostream &operator<<(std::wostream &out) {
        for (size_t i = 0; i < Values.Length(); ++i) {
            if (const auto &list = Values[i];  list.Length() > 0) {
                for (size_t j = 0; j < list.Length(); ++j) {
                    const auto &item = list[j];
                    std::cout << item.Key_ << ": " << item.Val_ << std::endl;
                }
            }
        }
        return out;
    }

    // Проверяет наличие ключа
    bool HasKey(const Key &key) {
        const auto hash = HashFunction(key);
        return hash <= Values.Length() && List[hash].Length() > 0;
    }

    // Перегружаем оператор [Key] для доступа по ключу
    Value &operator[](const Key &key) { 
        const size_t hash = HashFunction(key);
        if (hash >= Values.Length()) throw std::runtime_error("Key not found!");
        auto &list = Values[hash];
        if (list.Length() == 0) throw std::runtime_error("Key not found!");
        for (size_t i= 0; i < list.Length(); ++i)
            if (auto &item = list[i]; item.Key_ == key) 
                return item.Val_;
        throw std::runtime_error("Key not found!");
    }

    // Конструируем хеш-таблицу на основе хеш-функции
    Map(const std::function<size_t(const Key&)> &hashFunc) : HashFunction(hashFunc) {}

};


namespace Number {
    // Функция ощищает номер от пробелов и дефисов
    std::wstring Clean(std::wstring number) {
        return  { number.begin(), std::remove_if(number.begin(), number.end(),[loc = std::locale("")](const auto i) {
            return std::isspace(i, loc) || i == '-';
        }) };
    }

    // Функция проверяет номер на корректность
    bool IsCorrect(const std::wstring_view &number) {
        return !std::any_of(number.cbegin(), number.cend(), [loc = std::locale("")](const auto i) {
            return !(std::isdigit(i, loc) || std::isspace(i, loc) || i == '-');
        });
    }
}

int main() {
    // Устанавливаем в консоли поддержку юникодных строк
    std::locale::global(std::locale(""));
    // Каталог имя-номер
    Map<std::string, std::string> catalog([](const std::string &name) {
        return std::abs(std::accumulate(name.cbegin(), name.cend(), 0) % 255);
    });

    // Пример работы:
    catalog.Append("Евгений Олегович", "8-999-777-77-77");
    catalog.Append("Андрей Андреевич", "8-999-888-88-55");
    catalog.Append("abc", "9-666-854-59-88");
    catalog.Append("cba", "9-666-854-59-69");
    std::cout << catalog["abc"] << std::endl;
    std::cout << catalog["cba"] << std::endl;
}