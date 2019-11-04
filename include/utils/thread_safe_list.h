/**
 * Copyright 2019 all rights reserved
 * @brief This file offers a class of thread safe list.
 * @date 02/Nov/2019
 * @author jin.ma
 */

#ifndef UTILS_THREAD_SAFE_LIST_H_
#define UTILS_THREAD_SAFE_LIST_H_

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

  ThreadSafeList(const ThreadSafeList& other);

  void PushBack(const T& new_value);

  void PushFront(const T& new_value);

  void WaitAndPopFront(T& value);

  std::shared_ptr<T> WaitAndPopFront();

  bool TryPopFront(T& value);

  std::shared_ptr<T> TryPopFront();

  void WaitAndPopBack(T& value);

  std::shared_ptr<T> WaitAndPopBack();

  bool TryPopBack(T& value);

  std::shared_ptr<T> TryPopBack();

  std::shared_ptr<T> Front();

  std::shared_ptr<T> Back();

  bool IsEmpty() const;

  void Clear();

  size_t Size() const;

 private:
  mutable std::mutex mut_;
  std::list<std::shared_ptr<T>> data_list_;
  std::condition_variable data_cond_;
};

}  // namespace Utils

#endif  // UTILS_THREAD_SAFE_LIST_H_
