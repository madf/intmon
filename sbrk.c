#include <stdint.h>
#include <errno.h>

static uint8_t* heapPtr = NULL; // Top of the allocated space

/*
 * +----------------+-------+
 * | Heap       *   | Stack |
 * +----------------+-------+
 * ^            ^   ^       ^
 * |            |   |       |
 * +-_ssys_ram  |   |       +-_estack
 *              |   +-heapEnd
 *              +-heapPtr
 */
void* _sbrk(ptrdiff_t inc)
{
    extern uint8_t _ssys_ram; /* Comes from the linker script */
    extern uint8_t _estack; /* Comes from the linker script */
    extern uint32_t _Min_Stack_Size; /* Comes from the linker script */

    const uint32_t stackLimit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
    const uint8_t *heapEnd = (uint8_t *)stackLimit;
    uint8_t *res; // Start of new allocated chunk

    /* Initialize to the start of the heap */
    if (heapPtr == NULL)
        heapPtr = &_ssys_ram;

    if (heapPtr + inc > heapEnd)
    {
        // Out of memory
        errno = ENOMEM;
        return (void *)-1;
    }

    res = heapPtr; // New allocation goes to top
    heapPtr += inc;

    return res;
}
