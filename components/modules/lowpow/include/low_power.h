#ifndef _LOW_POWER_H
#define _LOW_POWER_H

#define LOW_POWER_SAVE_ENTRY_BASEBAND       0

#define LOW_POWER_RESTORE_ENTRY_BEGINNING   0
#define LOW_POWER_RESTORE_ENTRY_BASEBAND    1
#define LOW_POWER_RESTORE_ENTRY_DRIVER      2
#define LOW_POWER_RESTORE_ENTRY_MISC        3

void low_power_save(void);
void low_power_restore(void);

#endif //_LOW_POWER_H

