#ifndef _MEM_H_
#define _MEM_H_


#include "main.h"


#ifndef TRUE
#define TRUE ( 1 == 1 )
#endif


#ifndef FALSE
#define FALSE ( 1 == 0 )
#endif


typedef BYTE BOOLEAN;

typedef struct _LIST_ENTRY 
{
    struct _LIST_ENTRY* Flink;
    struct _LIST_ENTRY* Blink;
} LIST_ENTRY;

typedef struct _HEAP_HEADER 
{
    LIST_ENTRY     Entry;
    DWORD          Size;
}HEAP_HEADER;

typedef struct _HEAP
{
    void* Base;
    DWORD        Size;
    LIST_ENTRY   FreeList;
}HEAP;


void init_memory_allocators();

///////////////////////////////////////////// FRAME ///////////////////////////////////////////

// alocates a new frame. Returns TRUE if the frame was successfully allocated, FALSE if there are no more free frames

BOOLEAN frame_alloc(QWORD* frame, DWORD frame_count);

// frees a previously allocated frame through frame_alloc

void frame_free(QWORD frame, DWORD frame_count);

// returns TRUE if all frames in the region are free or FALSE otherwise

BOOLEAN are_frames_free(QWORD start_frame, DWORD frame_count);



///////////////////////////////////////////// PAGE ///////////////////////////////////////////

// maps page_count new pages. Returns TRUE if the page was succesfully mapped, FALSE if there are no more free frames or pages.

// If *page != NULL then the function will map the frame to *page, else the allocator will determine the virtual address

// If frame == -1 then this function internally calls frame_alloc to obtain the frames to which the pages will be mapped - it is not necessary for the physical frames to be continous

// If frame != -1 then the pages will be mapped to [frame, frame+page_count)

BOOLEAN page_alloc(void** page, DWORD page_count, QWORD frame);

// unmaps the pages and if free_backing is TRUE then frees the backing physical frame as well

// virtual address must be invalidated from CPU TLB cache, you can use the __invlpg() intrinsic to achieve this

void page_free(void* page, DWORD page_count, BOOLEAN free_backing);

// returns TRUE if the page is mapped

BOOLEAN is_page_mapped(void* page);



///////////////////////////////////////////// HEAP ///////////////////////////////////////////

// Creates a new heap. This function internally calls page_alloc to allocate the pages in which the heap will reside.

BOOLEAN heap_create(HEAP* heap, void* base, DWORD size);

// Allocate from the heap a buffer of Size bytes or returns NULL if the allocation cannot be done

void* heap_alloc(HEAP* heap, DWORD size);

// Free a heap entry previously allocated through heap_alloc

void heap_free(HEAP* heap, void* address);

// Destroys a heap. The pages allocated in the heap will be freed

void heap_destroy(HEAP* heap);

#endif //_MEM_H_