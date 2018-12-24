//
//  main.m
//  AsyncTaskPool
//
//  Created by ilong on 2018/2/7.
//  Copyright © 2018年 ilong. All rights reserved.
//

#include <stdio.h>
#include "async_task_pool.h"
#include "async_task_pool_test.h"
#include "timer.h"
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
   
   // timer test
   Timer t;
   //周期性执行定时任务
   t.StartTimer(1000, std::bind(EchoFunc,"hello world!"));
   std::this_thread::sleep_for(std::chrono::seconds(4));
   std::cout << "try to expire timer!" << std::endl;
   t.Expire();
   
   //周期性执行定时任务
   t.StartTimer(1000, std::bind(EchoFunc,  "hello c++11!"));
   std::this_thread::sleep_for(std::chrono::seconds(4));
   std::cout << "try to expire timer!" << std::endl;
   t.Expire();
   
   std::this_thread::sleep_for(std::chrono::seconds(2));
   
   //只执行一次定时任务
   //同步
   t.SyncWait(1000, std::bind(EchoFunc,  "hello c++11!"));
   //异步
   t.AsyncWait(1000, std::bind(EchoFunc,  "hello c++11!"));
   
   std::this_thread::sleep_for(std::chrono::seconds(2));
   
   //AsyncTaskPool test
   AsyncTaskPool::getInstance()->startTasks(AsyncTaskPool::TaskType::TASK_NETWORK);
   
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
   
   AsyncTaskPool::getInstance()->stopTasks(AsyncTaskPool::TaskType::TASK_NETWORK);
   AsyncTaskPool::getInstance()->startTasks(AsyncTaskPool::TaskType::TASK_NETWORK);
   //Lambda
   value = 22;
   AsyncTaskPool::getInstance()->enqueue(AsyncTaskPool::TaskType::TASK_NETWORK,[value]()->void{
      printf("Lambda new value:%d\n",value);
   });
   
   AsyncTaskPool::getInstance()->stopTasks(AsyncTaskPool::TaskType::TASK_NETWORK);
   
   return 0;
}
