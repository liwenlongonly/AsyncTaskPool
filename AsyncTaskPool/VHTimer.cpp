//
//  VHTimer.cpp
//
//  Created by ilong on 2017/12/19.
//  Copyright © 2017年 ilong. All rights reserved.
//

#include "VHTimer.h"
#include<thread>
#include<chrono>
#include<memory>

VHTimer::VHTimer():
mExpired(true){
   
}

VHTimer::~VHTimer(){
   Expire();
}

void VHTimer::StartTimer(int interval, std::function<void()> task){
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

void VHTimer::Expire(){
   if (mExpired) {
      return;
   }
   mExpired = true;
   mExpiredCond.notify_one();
}

template<typename callable, class... arguments>
void VHTimer::SyncWait(int after, callable&& f, arguments&&... args){
   mExpired = false;
   std::function<typename std::result_of<callable(arguments...)>::type()> task
   (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
   std::unique_lock<std::mutex> lck(mMutex);
   std::cv_status ret = mExpiredCond.wait_for(lck, std::chrono::milliseconds(after));
   if (ret == std::cv_status::timeout) {
      task();
   }
}

template<typename callable, class... arguments>
void VHTimer::AsyncWait(int after, callable&& f, arguments&&... args){
   mExpired = false;
   std::function<typename std::result_of<callable(arguments...)>::type()> task
   (std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
   std::thread([this,after, task](){
      std::unique_lock<std::mutex> lck(mMutex);
      std::cv_status ret = mExpiredCond.wait_for(lck, std::chrono::milliseconds(after));
      if (ret == std::cv_status::timeout) {
         task();
      }
   }).detach();
}
