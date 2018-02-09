#include "impl.h"

cv::cv(){
	impl_ptr = new cv::impl();
}

void cv::wait(mutex& lock){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();

	while(guard.exchange(true)) {}

	// error handling, check whether the lock is busy and the current thread hold the lock
	lock.impl_ptr->valid_lock_and_lock_holder();

	ucontext_t* current_context = (cpu::self()->impl_ptr)->running_context;

	// wait in the cv waiting queue
	impl_ptr->waiting_queue.push(current_context);

	// unlock the lock
	lock.impl_ptr->unlock_helper();

	// switch to next ready context
	if(!cpu::impl::ready_queue.empty()){
		ucontext_t* ready_context = cpu::impl::ready_queue.front();
		cpu::impl::ready_queue.pop();
		
		cpu::self()->impl_ptr->swap_and_set(current_context, ready_context);

		cpu::self()->impl_ptr->swapcontext_invariant();

        cpu::self()->impl_ptr->clear_finish_queue();
	} else { // no more ready context, go back to cpu init context
		cpu::self()->impl_ptr->swap_and_set(current_context,cpu::self()->impl_ptr->cpu_context);
		cpu::self()->impl_ptr->swapcontext_invariant();

        cpu::self()->impl_ptr->clear_finish_queue();
	}

	guard.store(false);
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();

	// reacquire the lock
	lock.lock();
}

void cv::signal(){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();

	while(guard.exchange(true)) {}
	// make next context on the waiting ready to run
	if (!impl_ptr->waiting_queue.empty()){
		impl_ptr->make_waiting_context_ready();
	}

	guard.store(false);
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

void cv::broadcast(){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();

	while(guard.exchange(true)) {}
	// make all contexts on the waiting ready to run
	while (!impl_ptr->waiting_queue.empty()){
		impl_ptr->make_waiting_context_ready();
	}

	guard.store(false);
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

cv::~cv(){
	delete impl_ptr;
}

