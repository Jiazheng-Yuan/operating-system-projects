//
// Created by Jiazheng on 9/28/17.
//

//#include "cv.h"
#include "impl.h"

cv::cv(){
	impl_ptr = new cv::impl();
}

// TODO: whether the lock is held by current thread
void cv::wait(mutex& lock){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	assert_interrupts_disabled();
	assert(!guard.load());

	while(guard.exchange(true)) {}
	// this thread holds the lock
	assert(lock.impl_ptr->is_busy);
	ucontext_t* current_context = (cpu::self()->impl_ptr)->running_context;
	impl_ptr->waiting_queue.push(current_context);

	if(!cpu::self()->impl_ptr->ready_queue.empty()){

		ucontext_t* ready_context = (cpu::self()->impl_ptr)->ready_queue.front();
		(cpu::self()->impl_ptr)->ready_queue.pop();
		(cpu::self()->impl_ptr)->running_context = ready_context;

		// release lock before sleep
		lock.impl_ptr->is_busy = false;
		swapcontext(current_context, ready_context);

		assert(guard.load());
        assert_interrupts_disabled();
        // reacquire lock when gets back
        lock.impl_ptr->is_busy = true;
		(cpu::self()->impl_ptr)->running_context = current_context;
	}

	guard.store(false);
	//assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

// TODO: whether the lock is held by current thread
void cv::signal(){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	assert_interrupts_disabled();
	assert(!guard.load());

	while(guard.exchange(true)) {}

	if (!impl_ptr->waiting_queue.empty()){
		ucontext_t* signal_context = impl_ptr->waiting_queue.front();
		impl_ptr->waiting_queue.pop();
		// make context ready
		(cpu::self()->impl_ptr)->ready_queue.push(signal_context);
	}

	guard.store(false);
	//assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

void cv::broadcast(){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	assert_interrupts_disabled();
	assert(!guard.load());

	while(guard.exchange(true)) {}

	while (!impl_ptr->waiting_queue.empty()){
		ucontext_t* signal_context = impl_ptr->waiting_queue.front();
		impl_ptr->waiting_queue.pop();
		// make context ready
		(cpu::self()->impl_ptr)->ready_queue.push(signal_context);
	}

	guard.store(false);
	//assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

cv::~cv(){delete impl_ptr;}

