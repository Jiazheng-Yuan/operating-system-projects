#include "impl.h"

size_t total_virtual_pages;
size_t swap_block_num;
size_t used_swap_blocks;

char copy_buffer[VM_PAGESIZE];
std::set<size_t> unused_swapfile;

std::map<pid_t, processInfo*> pid_to_processInfo;

std::vector<int> unused_pages;

std::deque<pageInfo*> page_clock;

std::map<fileblock, pageInfo*> fileblock_pageInfo;

pageInfo* pinned_page;

pid_t current_process;

std::map<pageInfo*, std::vector<std::pair<processInfo*, int> > >  pageInfo_virtualPageNoPerProcess; 

processInfo::processInfo(pid_t self_pid) {
	pid = self_pid;
	num_of_swap = 0;
	baseAddr = VM_ARENA_BASEADDR;
	pageTable = new page_table_t();
	for(int i = 0; i != VM_ARENA_SIZE/VM_PAGESIZE; ++i) {
		pageTable->ptes[i].read_enable = 0;
		pageTable->ptes[i].write_enable = 0;
	}
	currentBase = (char*) VM_ARENA_BASEADDR;
}

processInfo::~processInfo() {
	delete pageTable;
  pageTable = nullptr;
}


pageInfo::pageInfo(std::string inputFilename, size_t inputBlock) {
	filename_string = inputFilename;
	block = inputBlock;
  is_unnamed_fbpage = false;
}

fileblock::fileblock(std::string name, size_t input_block){
  file_name_string = name;
  block = input_block;
}

bool fileblock::operator<(const fileblock& fb) const
{
   return (file_name_string < fb.file_name_string || 
    (file_name_string == fb.file_name_string && block < fb.block) ||
    (file_name_string == fb.file_name_string && block == fb.block && is_file_back_page < fb.is_file_back_page));
}

void clear_memory_page(int pageNumber) {
  for(int i=0; i != VM_PAGESIZE; ++i) {   
    ((char *)vm_physmem)[pageNumber * VM_PAGESIZE + i] = 0;
  }
}

void update_pageTables(pageInfo* tempPageInfo, bool reading, bool writing, int newPageNumber) {

  for(size_t i = 0; i != pageInfo_virtualPageNoPerProcess[tempPageInfo].size(); ++i) {
    processInfo* tempProcess = pageInfo_virtualPageNoPerProcess[tempPageInfo][i].first;
    int tempVirtualPageNo = pageInfo_virtualPageNoPerProcess[tempPageInfo][i].second;
    if(newPageNumber >= 0) tempProcess->pageTable->ptes[tempVirtualPageNo].ppage = newPageNumber;
    if(reading) tempProcess->pageTable->ptes[tempVirtualPageNo].read_enable = 1;
    if(writing) tempProcess->pageTable->ptes[tempVirtualPageNo].write_enable = 1;
  }
}

void set_read_enable(pageInfo* page){
  page->resident = true;
  page->ref = true;
  page->read = 1;
  update_pageTables(page, true, false, -1);
}
void clear_pageTables(pageInfo* tempPageInfo) {
  for(size_t i = 0; i != pageInfo_virtualPageNoPerProcess[tempPageInfo].size(); ++i) {
    processInfo* tempProcess = pageInfo_virtualPageNoPerProcess[tempPageInfo][i].first;
    int tempVirtualPageNo = pageInfo_virtualPageNoPerProcess[tempPageInfo][i].second;
    tempProcess->pageTable->ptes[tempVirtualPageNo].read_enable = 0;
    tempProcess->pageTable->ptes[tempVirtualPageNo].write_enable = 0;
  }
}

bool is_swap_backed_page(pageInfo* page){
  return !page->is_unnamed_fbpage && page->filename_string.empty();
}
bool judge_write_enable(pageInfo* page){
  return (page->dirty && (!is_swap_backed_page(page) || (is_swap_backed_page(page) && pageInfo_virtualPageNoPerProcess[page].size() == 1)));
}

int evict_page() {
  // clock sweep
  while((page_clock.front())->ref) {
    pageInfo* temp = page_clock.front();
    page_clock.pop_front();
    temp->ref = false;
    temp->read = 0;
    temp->write = 0;

    clear_pageTables(temp);
    page_clock.push_back(temp);
  }

  // update evict page info
  pageInfo* evictPage = page_clock.front();
  page_clock.pop_front();
  evictPage->read = 0;
  evictPage->write = 0;
  evictPage->resident = false;

  clear_pageTables(evictPage);

  int newPageNumber = evictPage->pageNumber;
  std::string evictfilename = evictPage->filename_string;
  size_t evictBlock = evictPage->block;

  // if victim is dirty, write back to disk
  if(evictPage->dirty) {
    char* buf = ((char *)vm_physmem) + newPageNumber * VM_PAGESIZE;
    
    const char* filename_cstr = evictfilename.empty() ? nullptr : evictfilename.c_str();
    int success = file_write(filename_cstr, evictBlock, buf);
    evictPage->dirty = false;
    if(success == -1) return -1;
  }

  return newPageNumber;
}


int copy_on_write(processInfo* current_process_Info, pageInfo* parentPageInfo,int virtual_page_number) {

  int parentPageNumber = parentPageInfo->pageNumber;
  parentPageInfo->ref = true;
  parentPageInfo->read = 1;
  
  update_pageTables(parentPageInfo, true, false, -1);
  
  //copy to buffer
  for(int i = 0; i != VM_PAGESIZE; ++i) {
    copy_buffer[i] = ((char*)vm_physmem)[parentPageNumber * VM_PAGESIZE + i];
  }
  //generate a new pageInfo
  size_t swapFileIndex = *(unused_swapfile.rbegin());
  unused_swapfile.erase(*(unused_swapfile.rbegin()));
  pageInfo* newSwapPageInfo = new pageInfo("", swapFileIndex);

  int newPageNumber;

  if (!unused_pages.empty()){ // there is an unused memory page
    newPageNumber = unused_pages.back();
    unused_pages.pop_back();
    
  } else { // no more physical memory page, evict 
    newPageNumber = evict_page();
    if(newPageNumber == -1) return -1;
  }
  // finish evicting or there is a free space in memory
  newSwapPageInfo->pageNumber = newPageNumber;
  newSwapPageInfo->resident = true;
  newSwapPageInfo->ref = true;

  newSwapPageInfo->read = 1;
  newSwapPageInfo->write = 1;

  newSwapPageInfo->dirty = true;

  //copy buffer to physical memory
  for(int i=0; i != VM_PAGESIZE; ++i) {   
    ((char *)vm_physmem)[newPageNumber * VM_PAGESIZE + i] = copy_buffer[i];
  }

  current_process_Info->pageTable->ptes[virtual_page_number].ppage = newPageNumber;
  current_process_Info->pageTable->ptes[virtual_page_number].read_enable = 1;
  current_process_Info->pageTable->ptes[virtual_page_number].write_enable = 1;

  current_process_Info->virtual_page_info_directory[virtual_page_number] = newSwapPageInfo;
  page_clock.push_back(newSwapPageInfo);
  fileblock fb("", swapFileIndex);
  fb.is_file_back_page = false;
  fileblock_pageInfo.insert(std::make_pair(fb, newSwapPageInfo));

  //erase the mapping of the old page to the virtual page which just did copy on write
  for(auto it = pageInfo_virtualPageNoPerProcess[parentPageInfo].begin(); it != pageInfo_virtualPageNoPerProcess[parentPageInfo].end(); ++it) {
    if(it->first == current_process_Info && it->second == virtual_page_number) {
      pageInfo_virtualPageNoPerProcess[parentPageInfo].erase(it);
      break;
    }
  }

  if(pageInfo_virtualPageNoPerProcess[parentPageInfo].size() == 1) {  //only one owner left
    parentPageInfo->write = parentPageInfo->read && parentPageInfo->ref
                            && parentPageInfo->dirty && parentPageInfo->resident;

    update_pageTables(parentPageInfo, false, parentPageInfo->write, -1);
  }

  pageInfo_virtualPageNoPerProcess[newSwapPageInfo].push_back(std::make_pair(current_process_Info, virtual_page_number));

  return 0;
}

int non_resident(pageInfo* tempPageInfo) {
  int newPageNumber;

  //if there is a free page in memory, use it, otherwise evict a page
  if (!unused_pages.empty()){
    newPageNumber = unused_pages.back();
    unused_pages.pop_back();
  } else {
    newPageNumber = evict_page();
  }
  tempPageInfo->pageNumber = newPageNumber;

  //clear the memory page just evicted or chosen
  clear_memory_page(newPageNumber);

  char* buf = ((char *)vm_physmem) + newPageNumber * VM_PAGESIZE;

  std::string filename_string = tempPageInfo->filename_string;
  int success;
  //read the page from the disk to memory
  if (tempPageInfo->is_unnamed_fbpage){
    success = file_read(filename_string.c_str(), tempPageInfo->block, buf);
  } else {
    const char* filename_cstr = filename_string.empty() ? nullptr : filename_string.c_str();
    success = file_read(filename_cstr, tempPageInfo->block, buf);
  }
  if (success == -1) {
    unused_pages.push_back(newPageNumber);
    return -1;
  }
  //update mapping from this physical page to virtual pages
  update_pageTables(tempPageInfo, true, false, newPageNumber);
  page_clock.push_back(tempPageInfo);
  return 0;
}