.syntax unified
.cpu cortex-m4
.fpu vfpv4
.thumb

/* start address for the initialization values of the .data section.
 * defined in linker script
 */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

.section .text.reset_handler
.weak reset_handler
.type reset_handler, %function
reset_handler:

/* Copy the data segment initializers from flash to SRAM */
  movs r1, #0
  b    LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b    LoopFillZerobss

/* Zero fill the bss segment. */
FillZerobss:
  movs r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, =_ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call the clock system intitialization function.*/
  bl   SystemInit
/* Call the application's entry point.*/
  bl   main
  bx   lr
.size reset_handler, .-reset_handler
