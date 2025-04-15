#include <iostream>
#include <mutex>
#include <atomic>

// Базовый класс Singleton
template <typename T, template <typename> class CreationPolicy, template <typename> class ThreadSafetyPolicy>
class Singleton {
public:
    // Получение единственного экземпляра
    static T& getInstance() {
        return ThreadSafetyPolicy<T>::getInstance();
    }

protected:
    Singleton() = default; // Защищенный конструктор
    ~Singleton() = default; // Защищенный деструктор

private:
    Singleton(const Singleton&) = delete; // Запрет копирования
    Singleton& operator=(const Singleton&) = delete; // Запрет присваивания
};

// Стратегия создания объекта: Lazy Initialization
template <typename T>
class LazyCreation {
protected:
    static T* instance;
    static T* createInstance() {
        if (!instance) {
            instance = new T();
        }
        return instance;
    }
};
template <typename T>
T* LazyCreation<T>::instance = nullptr;

// Стратегия создания объекта: Eager Initialization
template <typename T>
class EagerCreation {
protected:
    static T instance;
    static T* createInstance() {
        return &instance;
    }
};
template <typename T>
T EagerCreation<T>::instance;

// Стратегия синхронизации: Без синхронизации
template <typename T>
class NoThreadSafety {
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }
};

// Стратегия синхронизации: Mutex
template <typename T>
class MutexThreadSafety {
private:
    static std::mutex mutex;
    static T* instance;

public:
    static T& getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            instance = new T();
        }
        return *instance;
    }
};
template <typename T>
std::mutex MutexThreadSafety<T>::mutex;
template <typename T>
T* MutexThreadSafety<T>::instance = nullptr;

// Стратегия синхронизации: std::call_once
template <typename T>
class CallOnceThreadSafety {
private:
    static std::once_flag onceFlag;
    static T* instance;

public:
    static T& getInstance() {
        std::call_once(onceFlag, []() { instance = new T(); });
        return *instance;
    }
};
template <typename T>
std::once_flag CallOnceThreadSafety<T>::onceFlag;
template <typename T>
T* CallOnceThreadSafety<T>::instance = nullptr;

// Пример использования Singleton
class MySingleton : public Singleton<MySingleton, LazyCreation, MutexThreadSafety> {
    friend class Singleton<MySingleton, LazyCreation, MutexThreadSafety>;

private:
    MySingleton() { std::cout << "MySingleton created\n"; }
    ~MySingleton() { std::cout << "MySingleton destroyed\n"; }

public:
    void sayHello() {
        std::cout << "Hello from MySingleton!\n";
    }
};

int main() {
    // Получение экземпляра Singleton
    MySingleton& instance1 = MySingleton::getInstance();
    instance1.sayHello();

    // Проверка, что экземпляр единственный
    MySingleton& instance2 = MySingleton::getInstance();
    instance2.sayHello();

    return 0;
}
