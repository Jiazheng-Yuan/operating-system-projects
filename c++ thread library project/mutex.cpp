//
// Created by Jiazheng on 9/28/17.
//
//
//#include "mutex.h"
#include "impl.h"
//#include <atomic>
//#include <ucontext.h>
mutex::mutex(){
	impl_ptr = new mutex::impl();
}
void mutex::lock(){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	assert_interrupts_disabled();
	assert(!guard.load());

	while(guard.exchange(true)) {}

	if(!impl_ptr->is_busy){
		impl_ptr->is_busy = true;
	}
	else{
		assert(guard.load());
		ucontext_t* temp_ptr = (cpu::self()->impl_ptr)->running_context;
		impl_ptr->waiting_queue.push(temp_ptr);
		if(!cpu::self()->impl_ptr->ready_queue.empty()){
			ucontext_t* ready_ptr = (cpu::self()->impl_ptr)->ready_queue.front();
			(cpu::self()->impl_ptr)->ready_queue.pop();
			(cpu::self()->impl_ptr)->running_context = ready_ptr;

			swapcontext(temp_ptr,ready_ptr);
			assert(guard.load());
	        assert_interrupts_disabled();
			(cpu::self()->impl_ptr)->running_context = temp_ptr;
		}
	}

	guard.store(false);
	//assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

void mutex::unlock(){
	assert_interrupts_enabled();
	cpu::interrupt_disable();
	assert_interrupts_disabled();
	assert(!guard.load());
	
	while(guard.exchange(true)) {}

	impl_ptr->is_busy = false;

	if(!impl_ptr->waiting_queue.empty()){
		assert(guard.load());
		ucontext_t* waiting_ptr = impl_ptr->waiting_queue.front();
		impl_ptr->waiting_queue.pop();
		cpu::self()->impl_ptr->ready_queue.push(waiting_ptr);
		impl_ptr->is_busy = true;
	}

	assert(guard.load());
	guard.store(false);
	assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

mutex::~mutex(){delete impl_ptr;}


