#ifndef __SYNC_TASK_POOL_H_
#define __SYNC_TASK_POOL_H_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

/**
 * @class AsyncTaskPool
 * @brief This class allows to perform background operations without having to manipulate threads.
 */
class AsyncTaskPool
{
public:
    typedef std::function<void(void*)> TaskCallBack;
    
    enum class TaskType
    {
        TASK_IO,
        TASK_NETWORK,
        TASK_OTHER,
        TASK_MAX_TYPE,
    };

    /**
     * Returns the shared instance of the async task pool.
     */
    static AsyncTaskPool* getInstance();

    /**
     * Destroys the async task pool.
     */
    static void destroyInstance();
   
    /**
     * Stop tasks.
     *
     * @param type Task type you want to stop.
     */
    void stopTasks(TaskType type);
   
    void clearTasks(TaskType type);
    
    /**
     * Enqueue a asynchronous task.
     *
     * @param type task type is io task, network task or others, each type of task has a thread to deal with it.
     * @param callback callback when the task is finished. The callback is called in the main thread instead of task thread.
     * @param callbackParam parameter used by the callback.
     * @param task can be lambda function to be performed off thread.
     */
    void enqueue(TaskType type, TaskCallBack callback, void* callbackParam, std::function<void()> task);

    /**
    * Enqueue a asynchronous task.
    *
    * @param type task type is io task, network task or others, each type of task has a thread to deal with it.
     * @param task can be lambda function to be performed off thread.
    * @lua NA
    */
    void enqueue(AsyncTaskPool::TaskType type, std::function<void()> task);
    
public:
    AsyncTaskPool();
    ~AsyncTaskPool();
    
protected:
    
    // thread tasks internally used
    class ThreadTasks {
        struct AsyncTaskCallBack
        {
            TaskCallBack          callback;
            void*                 callbackParam;
        };
    public:
        ThreadTasks()
        : _stop(false)
        {
            _thread = std::thread([this]{
                                      for(;;){
                                          std::function<void()> task;
                                          AsyncTaskCallBack callback;
                                          {
                                              std::unique_lock<std::mutex> lock(this->_queueMutex);
                                              this->_condition.wait(lock,
                                                                    [this]{ return this->_stop || !this->_tasks.empty(); });
                                              if(this->_stop && this->_tasks.empty())
                                                  return;
                                              task = std::move(this->_tasks.front());
                                              callback = std::move(this->_taskCallBacks.front());
                                              this->_tasks.pop();
                                              this->_taskCallBacks.pop();
                                          }
                                          task();
                                          std::function<void()> function = std::bind(callback.callback, callback.callbackParam);
                                          function();
                                      }
                                  });
        }
       
        ~ThreadTasks()
        {
           stop();
        }
       
        void clear()
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            while(_tasks.size())
                _tasks.pop();
            while (_taskCallBacks.size())
                _taskCallBacks.pop();
        }
       
        void stop(){
           {
              std::unique_lock<std::mutex> lock(_queueMutex);
              if (_stop) {
                 return;
              }
              _stop = true;
              
              while(_tasks.size())
                 _tasks.pop();
              while (_taskCallBacks.size())
                 _taskCallBacks.pop();
           }
           _condition.notify_all();
           _thread.join();
        }
       
        void enqueue(TaskCallBack callback, void* callbackParam, std::function<void()> task)
        {
            AsyncTaskCallBack taskCallBack;
            taskCallBack.callback = std::move(callback);
            taskCallBack.callbackParam = callbackParam;

            {
                std::unique_lock<std::mutex> lock(_queueMutex);
                
                // don't allow enqueueing after stopping the pool
                if(_stop)
                {
                    return;
                }
                
                _tasks.push(std::move(task));
                _taskCallBacks.push(std::move(taskCallBack));
            }
            _condition.notify_one();
        }
    private:
        
        // need to keep track of thread so we can join them
        std::thread _thread;
        // the task queue
        std::queue< std::function<void()> > _tasks;
        std::queue<AsyncTaskCallBack> _taskCallBacks;
        
        // synchronization
        std::mutex _queueMutex;
        std::condition_variable _condition;
        bool _stop;
    };
    
    //tasks
    ThreadTasks _threadTasks[int(TaskType::TASK_MAX_TYPE)];
    
    static AsyncTaskPool* s_asyncTaskPool;
    static std::mutex _instanceMutex;
};

inline void AsyncTaskPool::stopTasks(TaskType type)
{
    auto& threadTask = _threadTasks[(int)type];
    threadTask.stop();
}

inline void AsyncTaskPool::clearTasks(TaskType type){
   auto& threadTask = _threadTasks[(int)type];
   threadTask.clear();
}

inline void AsyncTaskPool::enqueue(AsyncTaskPool::TaskType type, TaskCallBack callback, void* callbackParam, std::function<void()> task)
{
    auto& threadTask = _threadTasks[(int)type];
    threadTask.enqueue(std::move(callback), callbackParam, std::move(task));
}

inline void AsyncTaskPool::enqueue(AsyncTaskPool::TaskType type, std::function<void()> task)
{
    enqueue(type, [](void*) {}, nullptr, std::move(task));
}

// end group
/// @}
#endif //__CCSYNC_TASK_POOL_H_
