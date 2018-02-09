
#ifndef _IMPL_H
#define _IMPL_H
#include <queue>
#include <ucontext.h>
#include "thread.h"
#include "mutex.h"
#include "cpu.h"
#include <cassert>
#include <stdexcept>

class thread::impl {
public:
	static int count;
	int id;

	// a waiting queue for thread joiners
	std::queue<ucontext_t*> join_waiting_queue;
	bool is_finished;
	bool can_delete_impl_ptr;
	impl();
};

class cpu::impl {
public:
	impl();
	static std::queue<ucontext_t*> ready_queue;
	// store all finished ucontext
	static std::queue<ucontext_t*> finished_queue;
	static std::queue<cpu*> suspended_cpu_queue;
	ucontext_t* cpu_context;
	ucontext_t* running_context;
    static bool first;
    static void timer_handler();
	static void IPI_handler();
	static void func_wrapper(thread_startfunc_t func, void * arg, thread::impl* current);

	void clear_finish_queue();

	void disable_interrupt_and_assertion();

	void enable_interrupt_and_assertion();

	void swapcontext_invariant();

	void wake_up_suspended_cpu();

	void swap_and_set(ucontext_t* from, ucontext_t* to);
};


class mutex::impl {
public:
	std::queue<ucontext_t*> waiting_queue;
	bool is_busy;
	int lock_holder;
	impl():is_busy(false),lock_holder(-1){}

	void valid_lock_and_lock_holder();

	void unlock_helper();
};

class cv::impl {
public:
	std::queue<ucontext_t*> waiting_queue;

	void make_waiting_context_ready();
};


#endif
