#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 
template <typename T>
class Queue
{
 public:
 
  T pop()
  {
    unique_lock<mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    T item = queue_.front();
    queue_.pop();
    return item;
  }
 
  void push(const T& item)
  {
    unique_lock<mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
 
 private:
  queue<T> queue_;
  mutex mutex_;
  condition_variable cond_;
};