#ifndef LL_H_
#define LL_H_

#ifndef __arm__
#error "File only included with keil!"
#endif // __arm__

#include <stdint.h>

typedef unsigned int CPU_SR;

/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
void GLOBAL_INT_START(void);

/** @brief Disable interrupts globally in the system.
 * This macro must be used when the system wants to disable all the interrupt
 * it could handle.
 */
void GLOBAL_INT_STOP(void);

/** @brief Disable interrupts globally in the system.
 * This macro must be used in conjunction with the @ref GLOBAL_INT_RESTORE macro since this
 * last one will close the brace that the current macro opens.  This means that both
 * macros must be located at the same scope level.
 */
uint32_t CPU_SR_Save(uint8_t);
#define GLOBAL_INT_DISABLE()    CPU_SR cpu_sr;  {cpu_sr = CPU_SR_Save(0x20);}

/** @brief Restore interrupts from the previous global disable.
 * @sa GLOBAL_INT_DISABLE
 */
void CPU_SR_Restore(CPU_SR reg);
#define GLOBAL_INT_RESTORE()    CPU_SR_Restore(cpu_sr)

#endif // LL_H_

