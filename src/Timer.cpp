//
//  VHTimer.cpp
//
//  Created by ilong on 2017/12/19.
//  Copyright © 2017年 ilong. All rights reserved.
//

#include "timer.h"
#include<thread>
#include<chrono>
#include<memory>

Timer::Timer():
mExpired(true){
   
}

Timer::~Timer(){
   Expire();
}

void Timer::StartTimer(int interval, std::function<void()> task){
   if (mExpired == false){
      return;
   }
   mExpired = false;
   std::thread([this, interval, task](){
      while (!mExpired){
         std::unique_lock<std::mutex> lck(mMutex);
         std::cv_status ret = mExpiredCond.wait_for(lck, std::chrono::milliseconds(interval));
         if (ret == std::cv_status::timeout) {
            task();
         }
      }
   }).detach();
}

void Timer::Expire(){
   if (mExpired) {
      return;
   }
   mExpired = true;
   mExpiredCond.notify_one();
}

void Timer::SyncWait(int after, std::function<void()> task){
   mExpired = false;
   std::unique_lock<std::mutex> lck(mMutex);
   std::cv_status ret = mExpiredCond.wait_for(lck, std::chrono::milliseconds(after));
   if (ret == std::cv_status::timeout) {
      task();
   }
}

void Timer::AsyncWait(int after, std::function<void()> task){
   mExpired = false;
   std::thread([this,after, task](){
      std::unique_lock<std::mutex> lck(mMutex);
      std::cv_status ret = mExpiredCond.wait_for(lck, std::chrono::milliseconds(after));
      if (ret == std::cv_status::timeout) {
         task();
      }
   }).detach();
}
