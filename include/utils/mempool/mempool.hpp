/**
 * Copyright 2019 all rights reserved
 * @brief Simple memory pool
 * @date 22/Aug/2019
 * @author jin.ma
 */

#ifndef UTILS_MEMPOOL_MEMPOOL_HPP_
#define UTILS_MEMPOOL_MEMPOOL_HPP_

#include <list>
#include <memory>
#include <utility>
#include <vector>

namespace Utils {

template <typename T>
class MemPool {
 public:
  template <typename... Args>
  static bool Create(int pre_alloc, int max_alloc, Args &&... args) {
    std::lock_guard<std::mutex> lck(singleton_mutex_);
    if (pool_ptr_.get() == nullptr) {
      pool_ptr_.reset(new MemPool<T>);
      pool_ptr_.get()->Init(pre_alloc, max_alloc, std::forward<Args>(args)...);
    } else {
      std::cout << "Mempool already initialized" << std::endl;
      return false;
    }
    return true;
  }

  template <typename... Args>
  int Init(int pre_alloc, int max_alloc, Args &&... args);

  static MemPool<T> *GetInstance() {
    std::lock_guard<std::mutex> lck(singleton_mutex_);

    if (pool_ptr_.get() == nullptr) {
      std::cout << "Please initialize MemPool<" << typeid(T).name() << "> first"
                << std::endl;
      return nullptr;
    }
    return pool_ptr_.get();
  }

  static T *Get() {
    MemPool<T> *pool = GetInstance();
    if (pool) {
      std::lock_guard<std::mutex> lck(pool->pool_mutex_);
      if (pool->free_list_.empty()) {
        return nullptr;
      }

      T *item = pool->free_list_.front();
      pool->free_list_.pop_front();
      pool->used_list_.push_back(item);
      return item;
    } else {
      return nullptr;
    }
  }

  template <typename... Args>
  static T *GetEx(Args &&... args) {
    MemPool<T> *pool = GetInstance();
    if (pool) {
      std::lock_guard<std::mutex> lck(pool->pool_mutex_);
      if (pool->free_list_.empty()) {
        if (pool->max_alloc_ != 0 && pool->max_alloc_ == pool->allocated_) {
          return nullptr;
        } else if (pool->max_alloc_ > pool->allocated_ ||
                   pool->max_alloc_ == 0) {
          pool->AllocItem(std::forward<Args>(args)...);
        }
      }

      T *item = pool->free_list_.front();
      pool->free_list_.pop_front();
      pool->used_list_.push_back(item);
      return item;
    } else {
      return nullptr;
    }
  }

  static std::shared_ptr<T> GetSharedPtr(bool auto_release = true) {
    T *item = Get();
    if (item) {
      if (auto_release) {
        std::shared_ptr<T> sp_item(item, ItemDeleter);
        return sp_item;
      } else {
        std::shared_ptr<T> sp_item(item, ItemDeleterNull);
        return sp_item;
      }
    } else {
      return nullptr;
    }
  }

  template <typename... Args>
  static std::shared_ptr<T> GetSharedPtrEx(bool auto_release, Args &&... args) {
    T *item = nullptr;
    item = GetEx(std::forward<Args>(args)...);
    if (item) {
      if (auto_release) {
        std::shared_ptr<T> sp_item(item, ItemDeleter);
        return sp_item;
      } else {
        std::shared_ptr<T> sp_item(item, ItemDeleterNull);
        return sp_item;
      }
    } else {
      return nullptr;
    }
  }

  static int Release(T *item) {
    MemPool<T> *pool = GetInstance();
    if (pool) {
      ItemDeleter(item);
    }
    return 0;
  }

  int GetAllocatedCnt() { return allocated_; }

  int GetMaxAllocCnt() { return max_alloc_; }

  int GetUsedCnt() {
    std::lock_guard<std::mutex> lck(pool_mutex_);
    return used_list_.size();
  }

  int GetFreeCnt() {
    std::lock_guard<std::mutex> lck(pool_mutex_);
    return free_list_.size();
  }

 private:
  MemPool() {}
  ~MemPool() {
    std::lock_guard<std::mutex> lck(pool_mutex_);
    for (size_t i = 0; i < items_.size(); i++) {
      delete items_[i];
    }
    items_.clear();
    free_list_.clear();
    used_list_.clear();
  }

  MemPool(const MemPool &other);
  MemPool &operator=(const MemPool &other);

  /* the invoker must hold pool_mutex before call AllocItem*/
  template <typename... Args>
  int AllocItem(Args &&... args) {
    T *item = new T(std::forward<Args>(args)...);
    items_.push_back(item);
    free_list_.push_front(item);
    allocated_++;
    return 0;
  }

  static void ItemDeleter(T *item) {
    if (!item) {
      return;
    }
    MemPool<T> *pool = GetInstance();
    if (pool) {
      std::lock_guard<std::mutex> lck(pool->pool_mutex_);
      auto iter = find(pool->used_list_.begin(), pool->used_list_.end(), item);
      if (iter == pool->used_list_.end()) {
        return;
      }
      item->Reset();
      pool->free_list_.push_back(item);
      pool->used_list_.erase(iter);
    }
    return;
  }

  static void ItemDeleterNull(T *item) { return; }

 private:
  std::vector<T *> items_;
  std::list<T *> free_list_;
  std::list<T *> used_list_;
  int max_alloc_;
  int allocated_;
  std::mutex pool_mutex_;

  static std::mutex singleton_mutex_;
  static std::unique_ptr<MemPool<T> > pool_ptr_;
};

template <typename T>
std::mutex MemPool<T>::singleton_mutex_;

template <typename T>
std::unique_ptr<MemPool<T> > MemPool<T>::pool_ptr_;

template <typename T>
template <typename... Args>
int MemPool<T>::Init(int pre_alloc, int max_alloc, Args &&... args) {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  allocated_ = 0;
  for (int i = 0; i < pre_alloc; i++) {
    AllocItem(std::forward<Args>(args)...);
  }
  used_list_.clear();
  max_alloc_ = max_alloc;
  return 0;
}



}  // namespace Utils

#endif  // UTILS_MEMPOOL_MEMPOOL_HPP_