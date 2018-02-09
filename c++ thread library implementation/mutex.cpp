#include "impl.h"

mutex::mutex(){
	impl_ptr = new mutex::impl();
}

void mutex::lock(){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();

	while(guard.exchange(true)) {}
	assert(guard.load());

	// if lock is not busy, get the lock
	if(!impl_ptr->is_busy){
		impl_ptr->is_busy = true;
		impl_ptr->lock_holder = cpu::self()->impl_ptr->running_context->uc_stack.ss_flags;
	}
	else{ // put the current context to the lock waiting queue
		assert(guard.load());
		ucontext_t* temp_ptr = (cpu::self()->impl_ptr)->running_context;
		impl_ptr->waiting_queue.push(temp_ptr);
		
		// switch to next ready context
		if(!cpu::impl::ready_queue.empty()){
			ucontext_t* ready_ptr = cpu::impl::ready_queue.front();
			cpu::impl::ready_queue.pop();
			cpu::self()->impl_ptr->swap_and_set(temp_ptr,ready_ptr);
			cpu::self()->impl_ptr->swapcontext_invariant();

			cpu::self()->impl_ptr->clear_finish_queue();
		}
		else{ // no more ready context, go back to cpu init context
			cpu::self()->impl_ptr->swap_and_set(temp_ptr,(cpu::self()->impl_ptr)->cpu_context);
			cpu::self()->impl_ptr->clear_finish_queue();
		}
	}

	guard.store(false);

	cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

void mutex::unlock(){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();
	
	while(guard.exchange(true)) {}

	// error handling, check whether the lock is busy and the current thread hold the lock
	impl_ptr->valid_lock_and_lock_holder();

	impl_ptr->unlock_helper();
	
	assert(guard.load());
	guard.store(false);
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

mutex::~mutex(){
	delete impl_ptr;
}


