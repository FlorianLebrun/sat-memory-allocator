#pragma once

#include <sat/memory/system-object.hpp>
#include <sat/stack-analysis/stack_analysis.hpp>
#include <string>
#include <functional>

namespace sat {

   struct ThreadStackContext;

   struct NativeStackMarker : IStackMarker {
      uintptr_t base;
      uintptr_t callsite;
      virtual void getSymbol(char* buffer, int size) override;
   };

   struct StackFrame {

      enum tKind {
         NoFrame = 0,
         RootFrame,
         FunctionFrame,
         BeaconSpanFrame,
         BeaconTagFrame,
      };

      // Basic
      StackFrame* next;
      uintptr_t InsAddress;
      uintptr_t StackAddress;
      uint64_t BeaconId;

      // Specific
      tKind Kind;
      union {
         uintptr_t FunctionAddress;
         sat::StackBeacon* BeaconAddress;
      } Data;

      // For call tree matching
      void* node;

      StackFrame* setRoot();
      std::string toString();
      void print();
   };

   class ThreadStackTracker : public SystemObject<>::Derived<ThreadStackTracker> {
   public:
      static const sat::Thread::ThreadObjectID ThreadObjectID = sat::Thread::STACK_TRACKER;
      static const int cThreadMaxStackBeacons = 1024;

      ThreadStackTracker(sat::Thread* thread);
      bool updateFrames(ThreadStackContext& context, sat::StackBeacon** beacons);
      bool captureFrames(std::function<void(StackFrame* topOfStack)> processing);
      uint64_t getStackStamp();

      virtual ~ThreadStackTracker() override { }

   public:
      StackBeacon* stackBeaconsBase[cThreadMaxStackBeacons];
      StackBeacon** stackBeacons;
      uint64_t stackLastId;

   private:
      sat::Thread* thread;
      StackFrame* lastStack;

      static const int reserve_batchSize = 512;
      StackFrame* _reserve;

      StackFrame* allocFrame() {
         StackFrame* frame = this->allocFrameBatch();
         this->_reserve = frame->next;
         return frame;
      }
      StackFrame* allocFrameBatch() {
         StackFrame* frames = this->_reserve;
         if (!frames) {
            frames = new StackFrame[reserve_batchSize];
            for (int i = 0; i < reserve_batchSize; i++) {
               frames[i].next = &frames[i + 1];
            }
            frames[reserve_batchSize - 1].next = this->_reserve;
         }
         this->_reserve = 0;
         return frames;
      }
   };

};
