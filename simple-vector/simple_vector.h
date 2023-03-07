#pragma once
 
#include <initializer_list>
#include <string>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <utility>
#include "array_ptr.h"
 
using namespace std;


class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity) {
        capacity_ = capacity;
    }
    size_t Get_Capacity() {
        return capacity_;
    }
private:
    size_t capacity_{};
};
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
 
template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default;
    
    SimpleVector(ReserveProxyObj capacity) : items_(capacity.Get_Capacity()) {
 
        capacity_ = capacity.Get_Capacity();
    }
        // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : items_(size), size_(size), capacity_(size) {
    fill(items_.Get(), items_.Get() + size, Type());
    }
    
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : items_(size), size_(size), capacity_(size) {
       fill(items_.Get(), items_.Get() + size, value);
    }
    
     // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, Type&& value) : items_(size), size_(size), capacity_(size) {
        for (size_t i = 0; i < size_; ++i){
        items_[i] = move(value);
        }
    }
 
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        copy(init.begin(), init.end(), begin());
        }
 
 
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
 
    }
 
    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
 
    }
 
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
 
    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index <= size_);        
        return items_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_){
            throw out_of_range ("out_of_range"s);
        }
        return items_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_){
            throw out_of_range ("out_of_range"s);
        }
        return items_[index];
    }
 
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0u;
    }
 
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size > size_){
            if (new_size <= capacity_){
                generate(end(),items_.Get() + new_size,
                        []{return Type();});
            }else{
                Reserve(new_size);
                generate(end(), items_.Get()+new_size,
                        []{return Type();});
            }
        }
        size_ = new_size;
    }
 
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return Iterator{items_.Get()};
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
       return Iterator{items_.Get() + size_};
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return cbegin();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return cend();
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return ConstIterator{items_.Get()};
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return ConstIterator{items_.Get() + size_};
    }
 
    SimpleVector(const SimpleVector& other) {
        for (auto it = other.begin();
            it != other.end(); ++it){
                PushBack(*it);
        }
    }
    
    SimpleVector(SimpleVector&& other) {
        for (auto it = other.begin();
            it != other.end(); ++it){
                PushBack(move(*it));
        }
        exchange(other.size_,0);
        exchange(other.capacity_,0);
    }
 
void Reserve(size_t new_capacity) {
		if (new_capacity > capacity_) {
			ArrayPtr<Type> tmp(new_capacity);
			std::move(begin(), end(), tmp.Get());
			items_.swap(tmp);
			capacity_ = new_capacity;
		}
	}
 
    SimpleVector& operator=(const SimpleVector& rhs) {
        SimpleVector<Type> tmp (rhs);
        swap(tmp);
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        SimpleVector<Type> tmp (rhs);
        swap(tmp);
        return *this;
    }
    
 
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    // выделите новый массив с удвоенной вместимостью, скопируйте в него элементы исходного массива, а в конец поместите вставляемый элемент. После этого можно обновить размер и вместимость вектора, переключиться на новый массив, а старый массив — удалить.
    void PushBack(const Type& item) {
        if (IsEmpty()){
            if(!capacity_){
            Reserve(1);
            }
            items_[0] = item;
        }
        if (size_ < capacity_){
            items_[size_] = item;
        }else{
            ArrayPtr<Type> tmp (capacity_ * 2);
            copy(begin(), end(), tmp.Get());
            tmp[size_] = item;
            items_.swap(tmp);
            capacity_ *= 2;
        }
        ++size_;
    }
    
    void PushBack(Type&& item) {
        if (IsEmpty()){
            if(!capacity_){
            Reserve(1);
            }
            items_[0] = move(item);
        }
        if (size_ < capacity_){
            items_[size_] = move(item);
        }else{
            ArrayPtr<Type> tmp (capacity_ * 2);
            for (auto it = items_.Get();
                it < items_.Get() + size_; ++it){
                    auto it_ = distance(items_.Get(), it);
                    tmp[it_] = move(*it);
            }
            tmp[size_] = move(item);
            items_.swap(tmp);
            capacity_ *= 2;
        }
        ++size_;
    }
 
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert (begin() <= pos && pos <= end());
        auto dist = distance(cbegin(), pos);
        if(size_ == capacity_){
            if(capacity_ == 0){
                ArrayPtr<Type> tmp (1);
                tmp[0] = value;
                items_.swap(tmp);
                ++size_;
                ++capacity_;
                return begin();
            }else{
                ArrayPtr<Type> tmp (++size_);
                copy (begin(), Iterator(pos), tmp.Get());
                tmp[dist] = value;
                copy(Iterator(pos), end(),
                    tmp.Get() + dist + 1);
                items_.swap(tmp);
                capacity_ = 2* capacity_;
            }
        }else if (size_ < capacity_){
            copy_backward(pos, cend(), end()+ 1);
            items_[dist] = value;
            ++size_;
        }
        return begin()+dist;     
    }
    
    Iterator Insert(Iterator pos, Type&& value) {
        assert (begin() <= pos && pos <= end());
        auto dist = distance(begin(), pos);
        if(size_ < capacity_){
            move_backward(Iterator(pos),end(),end()+1);
        }else{           
            if(capacity_ == 0){
               capacity_ = 1;
            }else{
                capacity_ *= 2;
            }
        ArrayPtr<Type> tmp (capacity_);
        move(begin(), pos, tmp.Get());
        move(pos, end(), tmp.Get() +dist +1);
        items_.swap(tmp);
        } 
        items_[dist] = move(value);
        ++size_;
        return begin()+dist;     
    }
    
 
 
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
       assert(!IsEmpty());
       --size_;
    }
 
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(!IsEmpty() && begin() <= pos && pos <= end());
        copy(make_move_iterator(Iterator(pos+1)),
            make_move_iterator(end()), Iterator(pos));
        --size_;
        return Iterator(pos);
    }
 
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        items_.swap(other.items_);
    }
 
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};


 
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
   return (operator==(lhs, rhs)) || (operator<(lhs, rhs));
}
 
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}
 
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (operator==(lhs, rhs)) || (operator>(lhs, rhs));
}