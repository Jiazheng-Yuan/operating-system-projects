#include "impl.h"

void cpu::init(thread_startfunc_t func, void * arg){
	cpu::self()->impl_ptr = new cpu::impl();

	// set IPI and TIMER interrupt handlers
	interrupt_vector_table[cpu::TIMER] = (void(*)()) &impl::timer_handler;
	interrupt_vector_table[cpu::IPI] = (void(*)()) &impl::IPI_handler;

	// the cpu which starts a nullptr function
	if (func != nullptr){
		// create the first thread
		thread(func, arg);

		impl_ptr->disable_interrupt_and_assertion();
	}
	
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();

	while(1) {
	    impl_ptr->disable_interrupt_and_assertion();
    	while(guard.exchange(true)) {}

    	assert(guard.load());

    	impl_ptr->clear_finish_queue();

		if(!cpu::impl::ready_queue.empty()) {
			ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;

	        ucontext_t* chosen_thread = cpu::impl::ready_queue.front();
	        cpu::impl::ready_queue.pop();    
	        
	        // switch to next ready context
	        cpu::self()->impl_ptr->swap_and_set(current_thread,chosen_thread);
	        cpu::self()->impl_ptr->swapcontext_invariant();

			impl_ptr->clear_finish_queue();

	        guard.store(false);
	        impl_ptr->enable_interrupt_and_assertion();
		} else { // no more ready context to run, suspend
			// add self to the suspended queue before suspend
			cpu::impl::suspended_cpu_queue.push(self());

			guard.store(false);
			cpu::interrupt_enable_suspend();
		}
	}
}



