#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
public:
  BlockingQueue(size_t capacity): m_capacity(capacity) {}

  void push(T &&item) {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_not_full.wait(lk, [this]() { return content.size() < m_capacity; });
      m_content.push_back(std::move(item));
    }
    m_not_empty.notify_one();
  }

  bool try_push(T &&item) {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      if (m_content.size() == m_capacity)
        return false;
      m_content.push_back(std::move(item));
    }
    m_not_empty.notify_one();
    return true;
  }

  void pop(T &item) {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      m_not_empty.wait(lk, [this]() { return !m_content.empty(); });
      item = std::move(m_content.front());
      m_content.pop_front();
    }
    m_not_full.notify_one();
  }

  bool try_pop(T &item) {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      if (m_content.empty())
        return false;
      item = std::move(m_content.front());
      m_content.pop_front();
    }
    m_not_full.notify_one();
    return true;
  }
private:
  std::deque<T> m_content;
  size_t m_capacity;

  std::mutex m_mutex;
  std::condition_variable m_not_empty;
  std::condition_variable m_not_full;

  BlockingQueue(const BlockingQueue &) = delete;
  BlockingQueue(BlockingQueue &&) = delete;
  BlockingQueue &operator = (const BlockingQueue &) = delete;
  BlockingQueue &operator = (BlockingQueue &&) = delete;

};
