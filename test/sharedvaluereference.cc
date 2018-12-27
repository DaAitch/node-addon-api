#include "napi.h"
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>

using namespace Napi;

Value TestEmptyIsEmpty(const CallbackInfo& info) {
  SharedValueReference<Object> empty_shared_ref;
  return Boolean::New(info.Env(), empty_shared_ref.IsEmpty());
}

Value TestEmptyRefIsNullptr(const CallbackInfo& info) {
  SharedValueReference<Object> empty_shared_ref;
  return Boolean::New(info.Env(), empty_shared_ref.RefId() == 0);
}

Value TestEmptyValueIsEmpty(const CallbackInfo& info) {
  SharedValueReference<Object> empty_shared_ref;
  return Boolean::New(info.Env(), empty_shared_ref.Value().IsEmpty());
}

Value TestEmptyIsMovable(const CallbackInfo& info) {
  SharedValueReference<Object> empty_shared_ref;
  auto moved_shared_ref = std::move(empty_shared_ref);

  return Boolean::New(info.Env(), moved_shared_ref.IsEmpty() && empty_shared_ref.IsEmpty());
}

Value TestInitializedIsNotEmpty(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);

  return Boolean::New(info.Env(), !shared_ref.IsEmpty());
}

Value TestInitializedRefIsNotNullptr(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);

  return Boolean::New(info.Env(), shared_ref.RefId() != 0);
}

Value TestInitializedResetRefcountZero(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);

  return Boolean::New(info.Env(), shared_ref.ResetMtUnsafeRefcount() == 0);
}

Value TestInitializedIsMovable(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);
  uint64_t refid_before_move = shared_ref.RefId();

  auto moved_shared_ref = std::move(shared_ref);
  return Boolean::New(info.Env(), shared_ref.IsEmpty() && !moved_shared_ref.IsEmpty()
                                  && moved_shared_ref.RefId() == refid_before_move);
}

Value TestCopyRefAreEqual(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);
  auto copy_shared_ref = shared_ref;

  return Boolean::New(info.Env(), copy_shared_ref.RefId() == shared_ref.RefId());
}

Value TestCopyResetRefcountOne(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);
  auto copy_shared_ref = shared_ref;

  return Boolean::New(info.Env(), shared_ref.ResetMtUnsafeRefcount() == 1);
}

Value TestRefcountResetInOtherThreads(const CallbackInfo& info) {
  Object obj = Object::New(info.Env());
  auto&& shared_ref = MakeShared(obj);

  const int thread_count = info[0].As<Number>().Int32Value();
  std::vector<std::thread> threads(thread_count);

  std::atomic_int thread_counter(0);
  for (auto&& thread : threads) {
    thread = std::thread([shared_ref, &thread_counter]() mutable {
      std::this_thread::sleep_for(std::chrono::microseconds(1));
      shared_ref.Reset();

      thread_counter++;
    });
  }

  for (auto&& thread : threads) {
    thread.join();
  }

  return Boolean::New(info.Env(), shared_ref.ResetMtUnsafeRefcount() == 0 && (int)thread_counter == thread_count);
}


Object InitSharedValueReference(Env env) {
  Object exports = Object::New(env);

  exports["testEmptyIsEmpty"] = Function::New(env, TestEmptyIsEmpty);
  exports["testEmptyRefIsNullptr"] = Function::New(env, TestEmptyRefIsNullptr);
  exports["testEmptyValueIsEmpty"] = Function::New(env, TestEmptyValueIsEmpty);
  exports["testEmptyIsMovable"] = Function::New(env, TestEmptyIsMovable);
  exports["testInitializedIsNotEmpty"] = Function::New(env, TestInitializedIsNotEmpty);
  exports["testInitializedRefIsNotNullptr"] = Function::New(env, TestInitializedRefIsNotNullptr);
  exports["testInitializedResetRefcountZero"] = Function::New(env, TestInitializedResetRefcountZero);
  exports["testInitializedIsMovable"] = Function::New(env, TestInitializedIsMovable);
  exports["testCopyRefAreEqual"] = Function::New(env, TestCopyRefAreEqual);
  exports["testCopyResetRefcountOne"] = Function::New(env, TestCopyResetRefcountOne);
  exports["testRefcountResetInOtherThreads"] = Function::New(env, TestRefcountResetInOtherThreads);

  return exports;
}
