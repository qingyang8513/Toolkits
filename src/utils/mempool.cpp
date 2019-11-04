/**
 * Copyright 2019 all rights reserved
 * @brief Simple memory pool
 * @date 22/Aug/2019
 * @author jin.ma
 */

#include "mempool.h"

namespace Utils {

template <typename T>
std::mutex MemPool<T>::singleton_mutex_{};

template <typename T>
std::unique_ptr<MemPool<T> > MemPool<T>::pool_ptr_{nullptr};

template <typename T>
template <typename... Args>
static bool MemPool<T>::Create(int pre_alloc, int max_alloc, Args &&... args) {
  if (max_alloc <= 0) {
    std::cout << "Parameter error! 'max_alloc' must be a positive integer."
              << std::endl;
    return false;
  }
  if (pre_alloc < 0) {
    pre_alloc = 0;
    std::cout << "Parameter error! 'pre_alloc' must be a positive integer. Use "
                 "zero by default."
              << std::endl;
  }
  if (pre_alloc > max_alloc) {
    pre_alloc = max_alloc;
  }

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

template <typename T>
static MemPool<T> *MemPool<T>::GetInstance() {
  std::lock_guard<std::mutex> lck(singleton_mutex_);
  if (pool_ptr_.get() == nullptr) {
    std::cout << "Please create MemPool<" << typeid(T).name() << "> first"
              << std::endl;
    return nullptr;
  }
  return pool_ptr_.get();
}

template <typename T>
static T *MemPool<T>::Get() {
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

template <typename T>
template <typename... Args>
static T *MemPool<T>::GetEx(Args &&... args) {
  MemPool<T> *pool = GetInstance();
  if (pool) {
    std::lock_guard<std::mutex> lck(pool->pool_mutex_);
    if (pool->free_list_.empty()) {
      if (pool->max_alloc_ <= 0) {
        return nullptr;
      }
      if (pool->allocated_ < pool->max_alloc_) {
        pool->AllocItem(std::forward<Args>(args)...);
      } else {
        return nullptr;
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

template <typename T>
static std::shared_ptr<T> MemPool<T>::GetSharedPtr(bool auto_release = true) {
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

template <typename T>
template <typename... Args>
static std::shared_ptr<T> MemPool<T>::GetSharedPtrEx(bool auto_release,
                                                     Args &&... args) {
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

template <typename T>
static int MemPool<T>::Release(T *item) {
  MemPool<T> *pool = GetInstance();
  if (pool) {
    ItemDeleter(item);
  }
  return 0;
}

template <typename T>
int MemPool<T>::GetUsedCount() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  return used_list_.size();
}

template <typename T>
int MemPool<T>::GetFreeCount() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  return free_list_.size();
}

template <typename T>
MemPool<T>::~MemPool() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  for (size_t i = 0; i < items_.size(); i++) {
    delete items_[i];
  }
  items_.clear();
  free_list_.clear();
  used_list_.clear();
}

template <typename T>
template <typename... Args>
int MemPool<T>::AllocItem(Args &&... args) {
  T *item = new T(std::forward<Args>(args)...);
  items_.push_back(item);
  free_list_.push_front(item);
  allocated_++;
  return 0;
}

template <typename T>
static void MemPool<T>::ItemDeleter(T *item) {
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

template <typename T>
static void MemPool<T>::ItemDeleterNull(T *item) {
  return;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::mutex MemPoolEx<T>::static_pool_mutex_;

template <typename T>
std::unordered_map<MemPoolEx<T> *, std::shared_ptr<MemPoolEx<T> > >
    MemPoolEx<T>::static_pool_map_;

template <typename T>
template <typename... Args>
static std::shared_ptr<MemPoolEx<T> > MemPoolEx<T>::Create(int pre_alloc,
                                                           int max_alloc,
                                                           Args &&... args) {
  MemPoolEx<T> *pool = new MemPoolEx<T>();
  pool->Init(pre_alloc, max_alloc, std::forward<Args>(args)...);

  auto deleter = [](MemPoolEx<T> *p) { delete p; };
  std::shared_ptr<MemPoolEx<T> > sp_pool(pool, deleter);
  {
    std::lock_guard<std::mutex> lck(static_pool_mutex_);
    // Remove none referenced pools, it means there is no other user using the
    // pool
    auto iter = static_pool_map_.begin();
    while (iter != static_pool_map_.end()) {
      if (iter->second.use_count() == 1) {
        iter = static_pool_map_.erase(iter);
      } else {
        ++iter;
      }
    }

    // Cache new allocated pool's shared_ptr to the map
    static_pool_map_[pool] = sp_pool;
  }
  return sp_pool;
}

template <typename T>
T *MemPoolEx<T>::Get() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  if (free_list_.empty()) {
    return nullptr;
  }

  T *item = free_list_.front();
  free_list_.pop_front();
  used_list_.push_back(item);
  return item;
}

template <typename T>
template <typename... Args>
T *MemPoolEx<T>::GetEx(Args &&... args) {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  if (free_list_.empty()) {
    if (max_alloc_ != 0 && max_alloc_ == allocated_) {
      return nullptr;
    } else if (max_alloc_ > allocated_ || max_alloc_ == 0) {
      AllocItem(std::forward<Args>(args)...);
    }
  }
  if (free_list_.empty()) {
    return nullptr;
  }
  T *item = free_list_.front();
  free_list_.pop_front();
  used_list_.push_back(item);
  return item;
}

template <typename T>
std::shared_ptr<T> MemPoolEx<T>::GetSharedPtr(bool auto_release = true) {
  T *item = Get();
  if (item) {
    if (auto_release) {
      ItemDeleter deleter(static_pool_map_[this]);
      std::shared_ptr<T> sp_item(item, deleter);
      return sp_item;
    } else {
      std::shared_ptr<T> sp_item(item, ItemDeleterNull);
      return sp_item;
    }
  } else {
    return nullptr;
  }
}

template <typename T>
template <typename... Args>
std::shared_ptr<T> MemPoolEx<T>::GetSharedPtrEx(bool auto_release,
                                                Args &&... args) {
  T *item = nullptr;
  item = GetEx(std::forward<Args>(args)...);
  if (item) {
    if (auto_release) {
      ItemDeleter deleter(static_pool_map_[this]);
      std::shared_ptr<T> sp_item(item, deleter);
      return sp_item;
    } else {
      std::shared_ptr<T> sp_item(item, ItemDeleterNull);
      return sp_item;
    }
  } else {
    return nullptr;
  }
}

template <typename T>
int MemPoolEx<T>::Release(T *item) {
  ItemDeleter deleter(static_pool_map_[this]);
  deleter(item);
  return 0;
}

template <typename T>
int MemPoolEx<T>::GetUsedCount() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  return used_list_.size();
}

template <typename T>
int MemPoolEx<T>::GetFreeCount() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  return free_list_.size();
}

template <typename T>
template <typename... Args>
int MemPoolEx<T>::Init(int pre_alloc, int max_alloc, Args &&... args) {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  allocated_ = 0;
  for (int i = 0; i < pre_alloc; i++) {
    AllocItem(std::forward<Args>(args)...);
  }
  used_list_.clear();
  max_alloc_ = max_alloc;
  return 0;
}

template <typename T>
template <typename... Args>
int MemPoolEx<T>::AllocItem(Args &&... args) {
  T *item = new T(std::forward<Args>(args)...);
  items_.push_back(item);
  free_list_.push_front(item);
  allocated_++;
  return 0;
}

template <typename T>
static void MemPoolEx<T>::ItemDeleterNull(T *item) {
  return;
}

template <typename T>
MemPoolEx<T>::ItemDeleter::ItemDeleter(std::shared_ptr<MemPoolEx<T> > pool)
    : pool_(pool) {}

template <typename T>
void MemPoolEx<T>::ItemDeleter::operator()(T *item) {
  if (!item) {
    return;
  }
  std::lock_guard<std::mutex> lck(pool_->pool_mutex_);
  auto iter =
      std::find(pool_->used_list_.begin(), pool_->used_list_.end(), item);
  if (iter == pool_->used_list_.end()) {
    return;
  }

  item->Reset();

  pool_->free_list_.push_back(item);
  pool_->used_list_.erase(iter);
  return;
}

template <typename T>
MemPoolEx<T>::~MemPoolEx() {
  std::lock_guard<std::mutex> lck(pool_mutex_);
  for (auto item : items_) {
    delete item;
  }
  items_.clear();
  free_list_.clear();
  used_list_.clear();
}

}  // namespace Utils
