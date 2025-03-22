#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>

template <typename T>
class Singleton {
private:
    inline static std::shared_ptr<T> instance_{nullptr};
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
public:
    static std::shared_ptr<T> Instance() {
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            instance_ = std::shared_ptr<T>(new T);
        });
        return instance_;
    }

    ~Singleton() = default;
};

#endif // SINGLETON_H
