//
//  VHTimer.hpp
//
//  Created by ilong on 2017/12/19.
//  Copyright © 2017年 ilong. All rights reserved.
//

#ifndef VHTimer_h
#define VHTimer_h

#include<functional>
#include<atomic>
#include<mutex>
#include<condition_variable>

//TEST

//#include<iostream>
//#include<string>
//#include"Timer.hpp"
//using namespace std;
//void EchoFunc(std::string&& s){
//   std::cout << "test : " << s << endl;
//}
//int main(){
//   VHTimer t;
//   //周期性执行定时任务
//   t.StartTimer(1000, std::bind(EchoFunc,"hello world!"));
//   std::this_thread::sleep_for(std::chrono::seconds(4));
//   std::cout << "try to expire timer!" << std::endl;
//   t.Expire();
//
//   //周期性执行定时任务
//   t.StartTimer(1000, std::bind(EchoFunc,  "hello c++11!"));
//   std::this_thread::sleep_for(std::chrono::seconds(4));
//   std::cout << "try to expire timer!" << std::endl;
//   t.Expire();
//
//   std::this_thread::sleep_for(std::chrono::seconds(2));
//
//   //只执行一次定时任务
//   //同步
//   t.SyncWait(1000, EchoFunc, "hello world!");
//   //异步
//   t.AsyncWait(1000, EchoFunc, "hello c++11!");
//
//   std::this_thread::sleep_for(std::chrono::seconds(2));
//
//   return 0;
//}

class VHTimer {
   
public:
   VHTimer();
   ~VHTimer();

   /**
    定时器启动

    @param interval 定时器的时间间隔 ms
    */
   void StartTimer(int interval, std::function<void()> task);
   
   /**
    结束定时器
    */
   void Expire();
   template<typename callable, class... arguments>
   void SyncWait(int after, callable&& f, arguments&&... args);
   template<typename callable, class... arguments>
   void AsyncWait(int after, callable&& f, arguments&&... args);
private:
   VHTimer(const VHTimer& )=delete;//禁用copy方法
   const VHTimer& operator=( const VHTimer& )=delete;//禁用赋值方法
   std::atomic<bool> mExpired;
   std::mutex mMutex;
   std::condition_variable mExpiredCond;
};

#endif /* VHTimer_h */
