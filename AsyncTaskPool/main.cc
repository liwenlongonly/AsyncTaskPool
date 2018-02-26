//
//  main.m
//  AsyncTaskPool
//
//  Created by ilong on 2018/2/7.
//  Copyright © 2018年 ilong. All rights reserved.
//

#include <stdio.h>
#include "VHAsyncTaskPool.h"
#include "AsyncTaskPoolTest.hpp"
#include <thread>
#include <string>
#include <chrono>

uint64_t GetTimestampMs() {
   auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
   return ms.count();
}

const std::string getCurrentSystemTime()
{
   auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   struct tm* ptm = localtime(&tt);
   char date[60] = {0};
   sprintf(date, "%d-%02d-%02d      %02d:%02d:%02d",
           (int)ptm->tm_year + 1900,(int)ptm->tm_mon + 1,(int)ptm->tm_mday,
           (int)ptm->tm_hour,(int)ptm->tm_min,(int)ptm->tm_sec);
   return std::string(date);
}

void funcTest(int value){
   printf("funcTest value:%d \n",value);
   std::chrono::milliseconds dura(2000);
   std::this_thread::sleep_for(dura);
   printf("Current ts: %lld \n",GetTimestampMs());
   printf("funcTest sleep finish.\n");
}

int main(int argc, const char * argv[]) {
   
   auto value = 10;
   //Lambda
   AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_NETWORK,[value]()->void{
      printf("Lambda value:%d\n",value);
   });
   
   // func
   AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_NETWORK,std::bind(funcTest, value));
   
   //class func
   AsyncTaskPoolTest test;
   AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_NETWORK,std::bind(&AsyncTaskPoolTest::Func,&test,value));
   
   AsyncTaskPool *thread = new AsyncTaskPool();
   thread->enqueue(AsyncTaskPool::TaskType::TASK_NETWORK, [value]()->void{
      printf("thread Lambda value:%d\n",value);
   });
   
   AsyncTaskPool::getInstance()->stopTasks(AsyncTaskPool::TaskType::TASK_NETWORK);
   
   return 0;
}
