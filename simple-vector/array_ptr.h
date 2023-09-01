#include <cassert>
#include <cstdlib>
#include <algorithm>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if(size != 0){
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr(ArrayPtr&&) = default;
    ArrayPtr& operator=(ArrayPtr&&) = default;
   
    [[nodiscard]] Type* Release() noexcept {
        Type* buf = raw_ptr_;
        raw_ptr_ = nullptr;
        return buf;
    }

    Type& operator[](size_t index) noexcept {
        auto index_ = raw_ptr_ + index;
        return *index_;
    }

    const Type& operator[](size_t index) const noexcept {
        const auto index_ = raw_ptr_ + index;
        return *index_;
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr ? true : false;
    }

   Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};