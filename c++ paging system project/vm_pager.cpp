#include "impl.h"

void vm_init(size_t memory_pages, size_t swap_blocks) {
	//initialization
	total_virtual_pages = memory_pages;
	swap_block_num = swap_blocks;
	for(size_t i = 0; i != swap_blocks;i++){
		unused_swapfile.insert(i);
	}
	used_swap_blocks = 0;

	//initialize pinned page
	clear_memory_page(0);
	pinned_page = new pageInfo("", swap_blocks);      
	pinned_page->read = 1;
	pinned_page->write = 0;
	pinned_page->pageNumber = 0;
	pinned_page->dirty = false;
	pinned_page->resident = true;
	pinned_page->ref = true;

	for(size_t i=1; i != memory_pages; ++i) {   //initialize unused
		unused_pages.push_back(i);
	}
}

int vm_create(pid_t parent_pid, pid_t child_pid) {
	//fork
	if(pid_to_processInfo.count(parent_pid)) {
		processInfo* parentProcessInfo = pid_to_processInfo[parent_pid];
		// eager swap reservation
		if(parentProcessInfo->num_of_swap > (swap_block_num - used_swap_blocks)){
			return -1;
		}
		used_swap_blocks += parentProcessInfo->num_of_swap;

		//initialize child process, create a new info for it
		processInfo* child_process = new processInfo(child_pid);
		child_process->num_of_swap = parentProcessInfo->num_of_swap;
		child_process->currentBase = parentProcessInfo->currentBase;

		//copy parents arena and change mapping accordingly
		for(size_t i = 0; i != parentProcessInfo->virtual_page_info_directory.size(); ++i){
			pageInfo* temp = parentProcessInfo->virtual_page_info_directory[i];
			if (is_swap_backed_page(temp)) { // change write permission 
				temp->write = 0;
			}
			parentProcessInfo->pageTable->ptes[i].write_enable = temp->write;
			pageInfo_virtualPageNoPerProcess[temp].push_back(std::make_pair(child_process,i));
			child_process->virtual_page_info_directory.push_back(temp);
		}

		for(int i = 0; i != VM_ARENA_SIZE/VM_PAGESIZE; ++i) {
			child_process->pageTable->ptes[i].ppage = parentProcessInfo->pageTable->ptes[i].ppage;
			child_process->pageTable->ptes[i].read_enable = parentProcessInfo->pageTable->ptes[i].read_enable;
			child_process->pageTable->ptes[i].write_enable = parentProcessInfo->pageTable->ptes[i].write_enable;
		}
		
		pid_to_processInfo[child_pid] = child_process;

	}
	else {  //create the first process
		processInfo* newProcess = new processInfo(child_pid);
		pid_to_processInfo[child_pid] = newProcess;
	}

	return 0;
}

void vm_switch(pid_t pid) {
	//point PTBR to the page table of another process
	page_table_base_register = pid_to_processInfo[pid]->pageTable;
	current_process = pid;
}

void *vm_map(const char *filename, size_t block) {
	uintptr_t baseAddr = (uintptr_t)VM_ARENA_BASEADDR;
	processInfo* current_process_Info = pid_to_processInfo[current_process];

	//check whether filename is in the valid portion of arena
	if(filename && (uintptr_t) filename < baseAddr) return nullptr;      
	if(filename && current_process_Info->currentBase <= filename) return nullptr;

	//check whether arena is full
	char* tempBase = current_process_Info->currentBase;
	if(tempBase == ((char*)VM_ARENA_BASEADDR) + VM_ARENA_SIZE) return nullptr;

	int virtualPageNo = (tempBase - (char*)VM_ARENA_BASEADDR)/VM_PAGESIZE;

	//mapping swap-backed page to pinned page
	if(!filename) {  
		//eager reservation
		if(used_swap_blocks == swap_block_num) return nullptr;
		used_swap_blocks++;
		current_process_Info->num_of_swap++;
		current_process_Info->pageTable->ptes[virtualPageNo].ppage = 0;
		current_process_Info->pageTable->ptes[virtualPageNo].read_enable = 1;
		current_process_Info->virtual_page_info_directory.push_back(pinned_page);

		pageInfo_virtualPageNoPerProcess[pinned_page].push_back(std::make_pair(current_process_Info, virtualPageNo));
	}
	else {  //mapping for file-backed page
		bool more_pages = true;
		int filename_virtual_page_number = (filename - (char*) VM_ARENA_BASEADDR) / VM_PAGESIZE;
		int current_filename_vpage = filename_virtual_page_number;
		size_t offset = (uintptr_t)filename % VM_PAGESIZE;
		size_t current_offset = offset;
		std::string filename_string;

		//extract the actual filename string from the virtual address passed into vm_map
		while (more_pages){

			//before we find '\0' we already reached the invalid portion of arena
			if ((size_t) current_filename_vpage == current_process_Info->virtual_page_info_directory.size()){
				return nullptr;
			}

			pageInfo* filename_pageInfo = current_process_Info->virtual_page_info_directory[current_filename_vpage];
			if (!filename_pageInfo->resident) {// non-resident

				//bring the page back from disk, regardless of whether the phys_mem is full. If file read fails, return nullptr
				if (non_resident(filename_pageInfo) == -1){
					return nullptr;
				}
			}

			//change the status of the page just read by kernal, and change the status of virtual pages mapped to this page
			set_read_enable(filename_pageInfo);
			
			//when the page is dirty, write is false, and when it's  swap-backed page with only one owner or filebacked, we change the write of the page to 1
			if (judge_write_enable(filename_pageInfo)){
				filename_pageInfo->write = 1;
				update_pageTables(filename_pageInfo, true, true, -1);
			}
			//find the end of the string, if not find we continue to search for the next virtual page
			int ppage = current_process_Info->pageTable->ptes[current_filename_vpage].ppage;
			for (size_t i = current_offset; i != VM_PAGESIZE; ++i){
				char c = ((char*) vm_physmem)[ppage * VM_PAGESIZE + i];
				if (c == '\0') {
					more_pages = false;
					break;
				}
				//the extracted string is stored here
				filename_string += c;
			}
			//haven't reach the terminating character, read the next virtual page
			current_filename_vpage += 1;
			current_offset = 0;
		}

		fileblock fb(filename_string, block);
		fb.is_file_back_page = true;
		//if the page was not mapped before, generate a new pageInfo for the page
		if(!fileblock_pageInfo.count(fb)) {    
			pageInfo* newPageInfo = new pageInfo(filename_string, block);
			newPageInfo->read = 0;
			newPageInfo->write = 0;
			newPageInfo->dirty = false;
			newPageInfo->resident = false;
			newPageInfo->ref = false;
			fileblock_pageInfo.insert(std::make_pair(fb, newPageInfo));
			current_process_Info->virtual_page_info_directory.push_back(newPageInfo);
			if(filename_string.empty()) {
				newPageInfo->is_unnamed_fbpage = true;
			}
			pageInfo_virtualPageNoPerProcess[newPageInfo].push_back(std::make_pair(current_process_Info, virtualPageNo));		
		}
		else {   //pageInfo already exists,map to the existing page
			pageInfo* filePageInfo = fileblock_pageInfo[fb];
			current_process_Info->virtual_page_info_directory.push_back(filePageInfo);
			pageInfo_virtualPageNoPerProcess[filePageInfo].push_back(std::make_pair(current_process_Info, virtualPageNo));
			if(filePageInfo->resident) {
				current_process_Info->pageTable->ptes[virtualPageNo].ppage = filePageInfo->pageNumber;
			}
			if(filename_string.empty()) {
				filePageInfo->is_unnamed_fbpage = true;
			}
			current_process_Info->pageTable->ptes[virtualPageNo].read_enable = filePageInfo->read;
			current_process_Info->pageTable->ptes[virtualPageNo].write_enable = filePageInfo->write;
		}
		
	}

	current_process_Info->currentBase += VM_PAGESIZE;

	return tempBase;
}



int vm_fault(const void *addr, bool write_flag){
    uintptr_t Addr = (uintptr_t)addr;
	uintptr_t baseAddr = (uintptr_t)VM_ARENA_BASEADDR;
	processInfo* current_process_Info = pid_to_processInfo[current_process];

	//check the addr is in valid portion of arena
	if(Addr < baseAddr) return -1;       
	if(current_process_Info->currentBase <= (char*)addr) return -1;

	int virtual_page_number = (Addr - baseAddr) / VM_PAGESIZE;

	pageInfo* fault_page = pid_to_processInfo[current_process]->virtual_page_info_directory[virtual_page_number];
	std::string filename_string = fault_page->filename_string;
	size_t block = fault_page->block;


	if(!fault_page->resident) {  //page not in memory
		
		if (non_resident(fault_page) == -1){
			return -1;
		}
		fileblock fb(filename_string, block);
		fb.is_file_back_page = !is_swap_backed_page(fault_page);

		fileblock_pageInfo[fb] = fault_page;
	}

	set_read_enable(fault_page);

	if (!write_flag && judge_write_enable(fault_page)){
		fault_page->write = 1;
		update_pageTables(fault_page, true, true, -1);
	}
	
	
	if(write_flag) {
		if (is_swap_backed_page(fault_page)){ // swap-back page
			if (fault_page->pageNumber == 0){ // if pinned page, copy-on-write
				int success = copy_on_write(current_process_Info, pinned_page,virtual_page_number);
				if(success == -1) {
					return -1;
				}
			} else { // other swap-back page
				if(pageInfo_virtualPageNoPerProcess[fault_page].size() == 1) { // not shared swap-backed page
					fault_page->dirty = true;
					fault_page->write = 1;
					current_process_Info->pageTable->ptes[virtual_page_number].write_enable = 1;
				}
				else { // shared swap-backed page
					int success = copy_on_write(current_process_Info, fault_page, virtual_page_number);
					if(success == -1) {
						return -1;
					}
				}
			}
		} else { // file-back page
			fault_page->write = 1;
			fault_page->dirty = 1;
			update_pageTables(fault_page, false, true, -1);
		}
	}
	
	return 0;
}



void vm_destroy(){
	processInfo* current_process_Info = pid_to_processInfo[current_process];
	used_swap_blocks -= current_process_Info->num_of_swap;

	//loop over all the virtual pages of the current process
	for(size_t i = 0; i != current_process_Info->virtual_page_info_directory.size(); ++i) {
		pageInfo* tempPageInfo = current_process_Info->virtual_page_info_directory[i];
		if(!tempPageInfo->pageNumber) { //pinned page
			for(auto it = pageInfo_virtualPageNoPerProcess[tempPageInfo].begin(); it != pageInfo_virtualPageNoPerProcess[tempPageInfo].end(); ++it) {
				if(it->first == current_process_Info) {
					pageInfo_virtualPageNoPerProcess[tempPageInfo].erase(it);
					break;
				}
			}
			continue;
		}

		if(is_swap_backed_page(tempPageInfo)) {   //swap back page
			if(pageInfo_virtualPageNoPerProcess[tempPageInfo].size() == 1) {  //only one owner
				if(tempPageInfo->resident) {//erase from the page_clock
					for(auto it = page_clock.begin(); it != page_clock.end(); ++it) {
						if(*it == tempPageInfo) {
							page_clock.erase(it);
							break;
						}
					}
					//one more free physical page in memory
					unused_pages.push_back(tempPageInfo->pageNumber);
				}
				pageInfo_virtualPageNoPerProcess.erase(tempPageInfo);
				size_t block = tempPageInfo->block;
				fileblock fb("", block);
				fb.is_file_back_page = false;
				fileblock_pageInfo.erase(fb);
				//one more free swap block
				unused_swapfile.insert(block);
				delete tempPageInfo;
				tempPageInfo = nullptr;
			}
			else {//the swap-backed page has multiple owners, only erase its mapping to the virtual pages of the current process

				for(auto it = pageInfo_virtualPageNoPerProcess[tempPageInfo].begin(); it != pageInfo_virtualPageNoPerProcess[tempPageInfo].end(); ++it) {
					if(it->first == current_process_Info) {
						pageInfo_virtualPageNoPerProcess[tempPageInfo].erase(it);
						break;
					}
				}
				//after erase mapping, if only one owner is left for a dirty resident referenced and read enabled swap-backed page, change its write bit to 1
				if(pageInfo_virtualPageNoPerProcess[tempPageInfo].size() == 1) {
					tempPageInfo->write = tempPageInfo->resident && tempPageInfo->ref 
										 && tempPageInfo->read && tempPageInfo->dirty;
				}
				update_pageTables(tempPageInfo, false, tempPageInfo->write, -1);
			}
			

		}
		else {  //file back page, erase its mapping to the virtual pages of the current process
			for(auto it = pageInfo_virtualPageNoPerProcess[tempPageInfo].begin(); it != pageInfo_virtualPageNoPerProcess[tempPageInfo].end(); ++it) {
				if(it->first == current_process_Info) {
					pageInfo_virtualPageNoPerProcess[tempPageInfo].erase(it);
					break;
				}
			}
		}
	}
	
	pid_to_processInfo.erase(current_process);
	delete current_process_Info;
	current_process_Info = nullptr;
} 
