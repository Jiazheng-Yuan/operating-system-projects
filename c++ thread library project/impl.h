
#ifndef _IMPL_H
#define _IMPL_H
#include <queue>
#include <ucontext.h>
#include "thread.h"
#include "mutex.h"
#include "cpu.h"
#include <ucontext.h>
#include <cassert>

class thread::impl {
public:
	//ucontext_t* self_context;
	static void func_wrapper(thread_startfunc_t func, void * arg);
};

class cpu::impl {
public:
	impl();
	static std::queue<ucontext_t*> ready_queue;
	static std::queue<ucontext_t*> finished_queue;
	ucontext_t* running_context;
    static bool first;
    void timer_handler();
};


class mutex::impl {
public:
	std::queue<ucontext_t*> waiting_queue;
	bool is_busy;
	impl():is_busy(false){}
};

class cv::impl {
public:
	std::queue<ucontext_t*> waiting_queue;
};

#endif
