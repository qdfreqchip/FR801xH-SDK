#ifndef _VERSION_H
#define _VERSION_H

/*********************************************************************
 * @fn      get_SDK_compile_date_time
 *
 * @brief   used to trace SDK version, usage is following:
 *          char *data, *time;
 *          get_SDK_compile_date_time(&data, &time);
 *
 * @param   date    - the date string when SDK is compiled, such as "Apr 17 2020".
 *          time    - the time string when SDK is compiled, such as "18:51:13".
 *
 * @return  None.
 */
void get_SDK_compile_date_time(char **date, char **time);

#endif  // _VERSION_H

