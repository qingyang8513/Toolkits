/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe queen.
 * @date 20/Aug/2019
 * @author jin.ma
 */

#ifndef UTILS_THREAD_SAFE_QUEUE_H_
#define UTILS_THREAD_SAFE_QUEUE_H_

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

  ThreadSafeQueue(const ThreadSafeQueue& other);

  void Push(const T& new_value);

  void WaitAndPop(T& value);

  std::shared_ptr<T> WaitAndPop();

  bool TryPop(T& value);

  std::shared_ptr<T> TryPop();

  std::shared_ptr<T> Front();

  std::shared_ptr<T> Back();

  bool IsEmpty() const;

  void Clear();

  size_t Size() const;

 private:
  mutable std::mutex mut_;
  std::queue<std::shared_ptr<T>> data_queue_;
  std::condition_variable data_cond_;
};

}  // namespace Utils

#endif  // UTILS_THREAD_SAFE_QUEUE_H_
