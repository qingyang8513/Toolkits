/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe list.
 * @date 02/Nov/2019
 * @author jin.ma
 */

#ifndef UTILS_THREAD_SAFE_LIST_HPP_
#define UTILS_THREAD_SAFE_LIST_HPP_

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace Utils {
template <typename T>
class ThreadSafeList {
 public:
  ThreadSafeList() = default;

  ThreadSafeList(const ThreadSafeList& other) {
    std::lock_guard<std::mutex> lk(other.mut_);
    data_list_ = other.data_list_;
  }

  void PushBack(const T& new_value) {
    auto data = std::make_shared<T>(std::move(new_value));
    std::lock_guard<std::mutex> lk(mut_);
    data_list_.push_back(data);
    data_cond_.notify_one();
  }

  void PushFront(const T& new_value) {
    auto data = std::make_shared<T>(std::move(new_value));
    std::lock_guard<std::mutex> lk(mut_);
    data_list_.push_front(data);
    data_cond_.notify_one();
  }

  void WaitAndPopFront(T& value) {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_list_.empty(); });
    value = std::move(*data_list_.front());
    data_list_.pop_front();
  }

  std::shared_ptr<T> WaitAndPopFront() {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_list_.empty(); });
    auto res = data_list_.front();
    data_list_.pop_front();
    return res;
  }

  bool TryPopFront(T& value) {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return false;
    }
    value = std::move(*data_list_.front());
    data_list_.pop_front();
    return true;
  }

  std::shared_ptr<T> TryPopFront() {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return std::shared_ptr<T>();
    }
    auto res = data_list_.front();
    data_list_.pop_front();
    return res;
  }

  void WaitAndPopBack(T& value) {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_list_.empty(); });
    value = std::move(*data_list_.back());
    data_list_.pop_back();
  }

  std::shared_ptr<T> WaitAndPopBack() {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_list_.empty(); });
    auto res = data_list_.back();
    data_list_.pop_back();
    return res;
  }

  bool TryPopBack(T& value) {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return false;
    }
    value = std::move(*data_list_.back());
    data_list_.pop_back();
    return true;
  }

  std::shared_ptr<T> TryPopBack() {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return std::shared_ptr<T>();
    }
    auto res = data_list_.back();
    data_list_.pop_back();
    return res;
  }

  std::shared_ptr<T> Front() {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return std::shared_ptr<T>();
    }
    return data_list_.front();
  }

  std::shared_ptr<T> Back() {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_list_.empty()) {
      return std::shared_ptr<T>();
    }
    return data_list_.back();
  }

  bool IsEmpty() const {
    std::lock_guard<std::mutex> lk(mut_);
    return data_list_.empty();
  }

  void Clear() {
    std::lock_guard<std::mutex> lk(mut_);
    data_list_ = std::list<std::shared_ptr<T>>();
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lk(mut_);
    return data_list_.size();
  }

 private:
  mutable std::mutex mut_;
  std::list<std::shared_ptr<T>> data_list_;
  std::condition_variable data_cond_;
};

}  // namespace Utils

#endif  // UTILS_THREAD_SAFE_LIST_HPP_
