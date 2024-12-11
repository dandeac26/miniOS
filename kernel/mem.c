#include "mem.h"


///////////////////////////////////////////// FRAME ///////////////////////////////////////////


//BOOLEAN frame_alloc(QWORD* frame, DWORD frame_count)
//{
//	return FALSE;
//}
//
//
//void frame_free(QWORD frame, DWORD frame_count)
//{
//
//}
//
//
//BOOLEAN are_frames_free(QWORD start_frame, DWORD frame_count)
//{
//	return FALSE;
//}


#define FRAME_SIZE 4096 // 4KB frames
#define BITMAP_SIZE 0x100000 // Adjust this size based on your maximum RAM (e.g., 128MB => 4KB * 32,768 frames)

static BYTE frame_bitmap[BITMAP_SIZE]; // Bitmap to track frame allocations

static QWORD total_frames = 0; // Total number of frames available
static QWORD usable_base = 0;  // Starting address of usable RAM

// Initialize the frame allocator
void init_frame_allocator() {
    cl_memset(frame_bitmap, 0, sizeof(frame_bitmap));

    // Calculate total frames from the memory map
    for (int i = 0; i < sizeof(int15_e820_entries) / sizeof(INT15_MEMORY_MAP_ENTRY); i++) {
        if (int15_e820_entries[i].Type == MemoryMapTypeUsableRAM) {
            QWORD start = int15_e820_entries[i].BaseAddress / FRAME_SIZE;
            QWORD length = int15_e820_entries[i].Length / FRAME_SIZE;

            // Mark usable frames as free
            for (QWORD frame = start; frame < start + length; frame++) {
                if (usable_base == 0) {
                    usable_base = frame * FRAME_SIZE;
                }
                total_frames++;
            }
        }
    }
}

// Frame allocation function
BOOLEAN frame_alloc(QWORD* frame, DWORD frame_count) {
    if (!frame || frame_count == 0) {
        return FALSE;
    }

    QWORD free_count = 0;
    QWORD start_frame = 0;

    // Search the bitmap for contiguous free frames
    for (QWORD i = 0; i < total_frames; i++) {
        if (!(frame_bitmap[i / 8] & (1 << (i % 8)))) { // Check if the frame is free
            if (free_count == 0) {
                start_frame = i;
            }
            free_count++;
            if (free_count == frame_count) {
                break;
            }
        }
        else {
            free_count = 0;
        }
    }

    if (free_count < frame_count) {
        return FALSE; // Not enough free frames found
    }

    // Mark frames as allocated
    for (QWORD i = start_frame; i < start_frame + frame_count; i++) {
        frame_bitmap[i / 8] |= (1 << (i % 8));
    }

    *frame = start_frame * FRAME_SIZE;
    return TRUE;
}

// Frame freeing function
void frame_free(QWORD frame, DWORD frame_count) {
    if (frame % FRAME_SIZE != 0 || frame_count == 0) {
        return; // Invalid frame or frame count
    }

    QWORD start_frame = frame / FRAME_SIZE;

    // Mark frames as free
    for (QWORD i = start_frame; i < start_frame + frame_count; i++) {
        frame_bitmap[i / 8] &= ~(1 << (i % 8)); // Clear the bit
    }
}

// Check if frames are free
BOOLEAN are_frames_free(QWORD start_frame, DWORD frame_count) {
    if (frame_count == 0) {
        return FALSE;
    }

    QWORD start = start_frame / FRAME_SIZE;

    for (QWORD i = start; i < start + frame_count; i++) {
        if (frame_bitmap[i / 8] & (1 << (i % 8))) { // Check if the frame is allocated
            return FALSE;
        }
    }

    return TRUE;
}



///////////////////////////////////////////// PAGE ///////////////////////////////////////////



BOOLEAN page_alloc(void** page, DWORD page_count, QWORD frame)
{
	return FALSE;
}


void page_free(void* page, DWORD page_count, BOOLEAN free_backing)
{

}

BOOLEAN is_page_mapped(void* page)
{
	return FALSE;
}


///////////////////////////////////////////// HEAP ///////////////////////////////////////////


BOOLEAN heap_create(HEAP* heap, void* base, DWORD size)
{
	return FALSE;
}


void* heap_alloc(HEAP* heap, DWORD size)
{

}


void heap_free(HEAP* heap, void* address)
{

}


void heap_destroy(HEAP* heap)
{

}