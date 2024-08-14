#include <pulchritude/mt.h>

#include <pulchritude/core.hpp>
#include <pulchritude/log.h>

#include <unordered_map>

#include <pthread.h>

// multi-threading API implementation using pthreads / POSIX threads


namespace pint {

struct Thread {
  pthread_t id;
};
struct Mutex {
  pthread_mutex_t id;
};

pule::ResourceContainer<pint::Thread, PuleMtThread> threads;
pule::ResourceContainer<pint::Mutex, PuleMtMutex> mutexes;

} // namespace pint


extern "C" {

// -- threads ------------------------------------------------------------------

PuleMtThread puleMtThreadCreate(PuleMtThreadCreateInfo ci) {
  pint::Thread thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(
    &attr, ci.detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE
  );
  pthread_create(&thread.id, nullptr, ci.fn, ci.userData);
  pthread_attr_destroy(&attr);
  return pint::threads.create(thread);
}

void * puleMtThreadJoin(PuleMtThread t) {
  void * ret;
  pthread_join(pint::threads.at(t)->id, &ret);
  return ret;
}

void puleMtThreadDetach(PuleMtThread t) {
  pthread_detach(pint::threads.at(t)->id);
}

void puleMtThreadDestroy(PuleMtThread t) {
  pint::threads.destroy(t);
}

void puleMtThreadExit(void * ret) {
  pthread_exit(ret);
}

void puleMtThreadYield() {
  sched_yield();
}

// -- mutexes ------------------------------------------------------------------

PuleMtMutex puleMtMutexCreate() {
  pint::Mutex mutex;
  pthread_mutex_init(&mutex.id, nullptr);
  return pint::mutexes.create(mutex);
}

void puleMtMutexLock(PuleMtMutex m) {
  pthread_mutex_lock(&pint::mutexes.at(m)->id);
}

void puleMtMutexUnlock(PuleMtMutex m) {
  pthread_mutex_unlock(&pint::mutexes.at(m)->id);
}

bool puleMtMutexTryLock(PuleMtMutex m) {
  return pthread_mutex_trylock(&pint::mutexes.at(m)->id) == 0;
}

} // extern C
