#pragma once
#include <sat/memory/system-object.hpp>
#include <string>
#include <thread>
#include <functional>

namespace sat {

   class Thread : public ISystemObject {
   public:

      enum ThreadObjectID {
         GLOBAL_HEAP = 0,     // sat::mem::GlobalHeap
         LOCAL_HEAP = 1,      // sat::mem::LocalHeap
         STACK_TRACKER = 2,   // sat::stack_analysis::ThreadStackTracker
         WORKPOOL = 3,        // sat::worker::IWorkPool
         MaxObjectCount = 16,
      };
      int heapId;

      virtual uint64_t getID() = 0;
      virtual uintptr_t getNativeHandle() = 0;

      static SAT_API Thread* current();
      
      template <typename T>
      T* getObject() { return (T*)this->objects[T::ThreadObjectID]; }
      template <typename T>
      void setObject(T* object) { this->objects[T::ThreadObjectID] = object; }

   protected:
      ISystemObject* objects[MaxObjectCount] = { 0 };
   };

   class ThreadPool : public ISystemObject {
   public:
      virtual Thread* create(std::function<void()>&& entrypoint = nullptr) = 0;
      virtual void foreach(std::function<void(Thread*)>&& callback) = 0;
   };

   SAT_API ThreadPool* createThreadPool(std::function<void()>&& entrypoint = nullptr, int stacksize = 0);
}
