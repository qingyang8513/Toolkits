/**
 * Copyright 2019 all rights reserved
 * @brief Simple memory pool
 * @date 22/Aug/2019
 * @author jin.ma
 */

#ifndef UTILS_MEMPOOL_H_
#define UTILS_MEMPOOL_H_

#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Utils {

template <typename T>
class MemPool {
 public:
  template <typename... Args>
  static bool Create(int pre_alloc, int max_alloc, Args &&... args);

  static MemPool<T> *GetInstance();

  static T *Get();

  template <typename... Args>
  static T *GetEx(Args &&... args);

  static std::shared_ptr<T> GetSharedPtr(bool auto_release = true);

  template <typename... Args>
  static std::shared_ptr<T> GetSharedPtrEx(bool auto_release, Args &&... args);

  static int Release(T *item);

  int GetAllocatedCount() { return allocated_; }

  int GetMaxAllocCount() { return max_alloc_; }

  int GetUsedCount();

  int GetFreeCount();

 private:
  MemPool() = default;
  ~MemPool();

  MemPool(const MemPool &other) = delete;
  MemPool &operator=(const MemPool &other) = delete;

  template <typename... Args>
  int Init(int pre_alloc, int max_alloc, Args &&... args);

  template <typename... Args>
  int AllocItem(Args &&... args);

  static void ItemDeleter(T *item);

  static void ItemDeleterNull(T *item);

 private:
  std::vector<T *> items_;
  std::list<T *> free_list_;
  std::list<T *> used_list_;
  int max_alloc_{0};
  int allocated_{0};
  std::mutex pool_mutex_;

  static std::mutex singleton_mutex_;
  static std::unique_ptr<MemPool<T> > pool_ptr_;
};  // class MemPool

///////////////////////////////////////////////////////////////////////////////
template <typename T>
class MemPoolEx {
 public:
  template <typename... Args>
  static std::shared_ptr<MemPoolEx<T> > Create(int pre_alloc, int max_alloc,
                                               Args &&... args);
  T *Get();

  template <typename... Args>
  T *GetEx(Args &&... args);

  std::shared_ptr<T> GetSharedPtr(bool auto_release = true);

  template <typename... Args>
  std::shared_ptr<T> GetSharedPtrEx(bool auto_release, Args &&... args);

  int Release(T *item);

  int GetAllocatedCount() { return allocated_; }

  int GetMaxAllocCount() { return max_alloc_; }

  int GetUsedCount();

  int GetFreeCount();

 private:
  template <typename... Args>
  int Init(int pre_alloc, int max_alloc, Args &&... args);

  template <typename... Args>
  int AllocItem(Args &&... args);

  struct ItemDeleter {  // a verbose array deleter:
    explicit ItemDeleter(std::shared_ptr<MemPoolEx<T> > pool);
    std::shared_ptr<MemPoolEx<T> > pool_;

    void operator()(T *item);
  };

  static void ItemDeleterNull(T *item);

 private:
  MemPoolEx() = default;

  ~MemPoolEx();

  MemPoolEx(const MemPoolEx &other) = delete;

  MemPoolEx &operator=(const MemPoolEx &other) = delete;

  std::vector<T *> items_;
  std::list<T *> free_list_;
  std::list<T *> used_list_;
  int max_alloc_;
  int allocated_;
  std::mutex pool_mutex_;

  typedef std::unordered_map<MemPoolEx<T> *, std::shared_ptr<MemPoolEx<T> > >
      PoolMap;
  static std::mutex static_pool_mutex_;
  static PoolMap static_pool_map_;
};  // class MemPoolEx

}  // namespace Utils

#endif  // UTILS_MEMPOOL_H_
