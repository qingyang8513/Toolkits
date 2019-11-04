/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe list.
 * @date 02/Nov/2019
 * @author jin.ma
 */
#include "thread_safe_list.h"

namespace Utils {

template <typename T>
ThreadSafeList<T>::ThreadSafeList(const ThreadSafeList& other) {
  std::lock_guard<std::mutex> lk(other.mut_);
  data_list_ = other.data_list_;
}

template <typename T>
void ThreadSafeList<T>::PushBack(const T& new_value) {
  auto data = std::make_shared<T>(std::move(new_value));
  std::lock_guard<std::mutex> lk(mut_);
  data_list_.push_back(data);
  data_cond_.notify_one();
}

template <typename T>
void ThreadSafeList<T>::PushFront(const T& new_value) {
  auto data = std::make_shared<T>(std::move(new_value));
  std::lock_guard<std::mutex> lk(mut_);
  data_list_.push_front(data);
  data_cond_.notify_one();
}

template <typename T>
void ThreadSafeList<T>::WaitAndPopFront(T& value) {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_list_.empty(); });
  value = std::move(*data_list_.front());
  data_list_.pop_front();
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::WaitAndPopFront() {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_list_.empty(); });
  auto res = data_list_.front();
  data_list_.pop_front();
  return res;
}

template <typename T>
bool ThreadSafeList<T>::TryPopFront(T& value) {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return false;
  }
  value = std::move(*data_list_.front());
  data_list_.pop_front();
  return true;
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::TryPopFront() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return std::shared_ptr<T>();
  }
  auto res = data_list_.front();
  data_list_.pop_front();
  return res;
}

template <typename T>
void ThreadSafeList<T>::WaitAndPopBack(T& value) {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_list_.empty(); });
  value = std::move(*data_list_.back());
  data_list_.pop_back();
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::WaitAndPopBack() {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_list_.empty(); });
  auto res = data_list_.back();
  data_list_.pop_back();
  return res;
}

template <typename T>
bool ThreadSafeList<T>::TryPopBack(T& value) {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return false;
  }
  value = std::move(*data_list_.back());
  data_list_.pop_back();
  return true;
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::TryPopBack() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return std::shared_ptr<T>();
  }
  auto res = data_list_.back();
  data_list_.pop_back();
  return res;
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::Front() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return std::shared_ptr<T>();
  }
  return data_list_.front();
}

template <typename T>
std::shared_ptr<T> ThreadSafeList<T>::Back() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_list_.empty()) {
    return std::shared_ptr<T>();
  }
  return data_list_.back();
}

template <typename T>
bool ThreadSafeList<T>::IsEmpty() const {
  std::lock_guard<std::mutex> lk(mut_);
  return data_list_.empty();
}

template <typename T>
void ThreadSafeList<T>::Clear() {
  std::lock_guard<std::mutex> lk(mut_);
  data_list_ = std::list<std::shared_ptr<T>>();
}

template <typename T>
size_t ThreadSafeList<T>::Size() const {
  std::lock_guard<std::mutex> lk(mut_);
  return data_list_.size();
}

}  // namespace Utils
