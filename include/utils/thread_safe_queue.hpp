/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe queen.
 * @date 20/Aug/2019
 * @author jin.ma
 */

#ifndef UTILS_THREAD_SAFE_QUEUE_HPP_
#define UTILS_THREAD_SAFE_QUEUE_HPP_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace Utils {
template <typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() = default;

  ThreadSafeQueue(const ThreadSafeQueue& other) {
    std::lock_guard<std::mutex> lk(other.mut_);
    data_queue_ = other.data_queue_;
  }

  void Push(const T& new_value) {
    auto data = std::make_shared<T>(std::move(new_value));
    std::lock_guard<std::mutex> lk(mut_);
    data_queue_.push(data);
    data_cond_.notify_one();
  }

  void WaitAndPop(T& value) {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_queue_.empty(); });
    value = std::move(*data_queue_.front());
    data_queue_.pop();
  }

  std::shared_ptr<T> WaitAndPop() {
    std::unique_lock<std::mutex> lk(mut_);
    data_cond_.wait(lk, [this] { return !data_queue_.empty(); });
    auto res = data_queue_.front();
    data_queue_.pop();
    return res;
  }

  bool TryPop(T& value) {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_queue_.empty()) {
      return false;
    }
    value = std::move(*data_queue_.front());
    data_queue_.pop();
    return true;
  }

  std::shared_ptr<T> TryPop() {
    std::lock_guard<std::mutex> lk(mut_);
    if (data_queue_.empty()) {
      return std::shared_ptr<T>();
    }
    auto res = data_queue_.front();
    data_queue_.pop();
    return res;
  }

  bool IsEmpty() const {
    std::lock_guard<std::mutex> lk(mut_);
    return data_queue_.empty();
  }

  void Clear() {
    std::lock_guard<std::mutex> lk(mut_);
    data_queue_ = std::queue<std::shared_ptr<T>>();
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lk(mut_);
    return data_queue_.size();
  }

 private:
  mutable std::mutex mut_;
  std::queue<std::shared_ptr<T>> data_queue_;
  std::condition_variable data_cond_;
};

}  // namespace Utils

#endif  // UTILS_THREAD_SAFE_QUEUE_HPP_
