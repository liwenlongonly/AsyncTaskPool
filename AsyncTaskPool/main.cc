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
#include "VHTimer.h"
#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <ctime> //std::localtime
#include <iomanip> //std::put_time
#include <sstream> //std::stringstream

uint64_t GetTimestampMs() {
   auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
   return ms.count();
}

const std::string getCurrentSystemDate()
{
   auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   std::stringstream ss;
   ss<<std::put_time(std::localtime(&tt), "%Y-%m-%d %H.%M.%S");
   return ss.str();
}

void funcTest(int value){
   printf("funcTest value:%d \n",value);
   std::chrono::milliseconds dura(2000);
   std::this_thread::sleep_for(dura);
   printf("current ts: %lld \n",GetTimestampMs());
   printf("current date: %s \n",getCurrentSystemDate().c_str());
   printf("funcTest sleep finish.\n");
}

void EchoFunc(std::string&& s){
   std::cout << "test : " << s << std::endl;
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
   
   VHTimer timer;
   timer.SyncWait(1000, EchoFunc, "hello world!");
   
   return 0;
}
