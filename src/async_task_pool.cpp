#include "async_task_pool.h"

AsyncTaskPool* AsyncTaskPool::s_asyncTaskPool = nullptr;

std::mutex AsyncTaskPool::_instanceMutex;

AsyncTaskPool* AsyncTaskPool::getInstance(){
    std::unique_lock<std::mutex> lock(_instanceMutex);
    if (s_asyncTaskPool == nullptr){
        s_asyncTaskPool = new (std::nothrow) AsyncTaskPool();
    }
    return s_asyncTaskPool;
}

void AsyncTaskPool::destroyInstance(){
   if (s_asyncTaskPool) {
      delete s_asyncTaskPool;
      s_asyncTaskPool = nullptr;
   }
}

AsyncTaskPool::AsyncTaskPool(){
   
}

AsyncTaskPool::~AsyncTaskPool(){
   
}

