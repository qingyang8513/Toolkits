/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe queen.
 * @date 20/Aug/2019
 * @author jin.ma
 */

#include "thread_safe_queue.h"

namespace Utils {

template <typename T>
ThreadSafeQueue<T>::ThreadSafeQueue(const ThreadSafeQueue& other) {
  std::lock_guard<std::mutex> lk(other.mut_);
  data_queue_ = other.data_queue_;
}

template <typename T>
void ThreadSafeQueue<T>::Push(const T& new_value) {
  auto data = std::make_shared<T>(std::move(new_value));
  std::lock_guard<std::mutex> lk(mut_);
  data_queue_.push(data);
  data_cond_.notify_one();
}

template <typename T>
void ThreadSafeQueue<T>::WaitAndPop(T& value) {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_queue_.empty(); });
  value = std::move(*data_queue_.front());
  data_queue_.pop();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::WaitAndPop() {
  std::unique_lock<std::mutex> lk(mut_);
  data_cond_.wait(lk, [this] { return !data_queue_.empty(); });
  auto res = data_queue_.front();
  data_queue_.pop();
  return res;
}

template <typename T>
bool ThreadSafeQueue<T>::TryPop(T& value) {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return false;
  }
  value = std::move(*data_queue_.front());
  data_queue_.pop();
  return true;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::TryPop() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return std::shared_ptr<T>();
  }
  auto res = data_queue_.front();
  data_queue_.pop();
  return res;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::Front() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return std::shared_ptr<T>();
  }
  return data_queue_.front();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::Back() {
  std::lock_guard<std::mutex> lk(mut_);
  if (data_queue_.empty()) {
    return std::shared_ptr<T>();
  }
  return data_queue_.back();
}

template <typename T>
bool ThreadSafeQueue<T>::IsEmpty() const {
  std::lock_guard<std::mutex> lk(mut_);
  return data_queue_.empty();
}

template <typename T>
void ThreadSafeQueue<T>::Clear() {
  std::lock_guard<std::mutex> lk(mut_);
  data_queue_ = std::queue<std::shared_ptr<T>>();
}

template <typename T>
size_t ThreadSafeQueue<T>::Size() const {
  std::lock_guard<std::mutex> lk(mut_);
  return data_queue_.size();
}

}  // namespace Utils
