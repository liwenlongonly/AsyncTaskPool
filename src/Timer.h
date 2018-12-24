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

class Timer {
   
public:
   Timer();
   ~Timer();

   /**
    定时器启动

    @param interval 定时器的时间间隔 ms
    */
   void startTimer(int interval, std::function<void()> task);
   
   /**
    结束定时器
    */
   void expire();
   
   void syncWait(int after, std::function<void()> task);
  
   void asyncWait(int after, std::function<void()> task);
private:
   Timer(const Timer& )=delete;//禁用copy方法
   const Timer& operator=( const Timer& )=delete;//禁用赋值方法
   std::atomic<bool> mExpired;
   std::mutex mMutex;
   std::condition_variable mExpiredCond;
};

#endif /* VHTimer_h */
