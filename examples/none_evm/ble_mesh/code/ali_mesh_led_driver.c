/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES 
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "driver_gpio.h"
#include "driver_system.h"
#include "driver_pwm.h"
#include "driver_pmu.h"
#include "co_printf.h"

#include "ali_mesh_info.h"
#include "ali_mesh_led_driver.h"

/*
 * MACROS 
 */

/*
 * CONSTANTS 
 */

/* pwm output frequency is 8K */
#define FREQUENCY_SET	8000
#define PORT_FUNC_PWM	0x03
#define PORT_FUNC_GPIO	0x00
#define TEMP_WARM       0x4e20
#define TEMP_COLD       0x320
#define R_LED           0
#define G_LED           1
#define B_LED           2
#define W_LED           0
#define C_LED           1
/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */

/*
 * LOCAL VARIABLES 
 */
static struct app_led_state_t app_led_state;
/* use PD4/PD5 and PD6 for led control */
static const uint8_t app_led_pwm_map[APP_LED_COUNT] = {6, 4, 5};
static uint8_t app_led_inited = false;

static void app_led_set_state(void);

// HSL TO RGB***************
#if 0
void RGB2HSL(TColor AColor, double &H,double &S,double &L)
{    
	double R,G,B,Max,Min,del_R,del_G,del_B,del_Max;   
	R = GetRValue(AColor) / 255.0;       //Where RGB values = 0 / 255  
	G = GetGValue(AColor) / 255.0;   
	B = GetBValue(AColor) / 255.0;   
	Min = min(R, min(G, B));    //Min  value of RGB  
	Max = max(R, max(G, B));    //Max. value of RGB  
	del_Max = Max - Min;        //Delta RGB value  
	L = (Max + Min) / 2.0;  
	if (del_Max == 0)           //This is a gray, no chroma...   
	{       
		//H = 2.0/3.0;          //Windows env S = 0,H=160(2/3*240)       
		H = 0;                  //HSL results = 0 / 1      
		S = 0;   
		}   
	else                        //Chromatic data...  
	{       
		if (L < 0.5) S = del_Max / (Max + Min);     
		else         S = del_Max / (2 - Max - Min);     
		del_R = (((Max - R) / 6.0) + (del_Max / 2.0)) / del_Max;     
		del_G = (((Max - G) / 6.0) + (del_Max / 2.0)) / del_Max;      
		del_B = (((Max - B) / 6.0) + (del_Max / 2.0)) / del_Max;   
		if      (R == Max) H = del_B - del_G;       
		else if (G == Max) H = (1.0 / 3.0) + del_R - del_B;    
		else if (B == Max) H = (2.0 / 3.0) + del_G - del_R;      
		if (H < 0) 
			H += 1;      
		if (H > 1) 
			H -= 1;   
		}
}
#endif

double Hue2RGB(double v1, double v2, double vH)
{   
	if (vH < 0)
	{
		vH += 10;
	}
	if (vH > 10)
		vH -= 10;   
	if (6 * vH < 10) 
		return v1 + (v2 - v1) * 6 * vH/100;   
	if (2 * vH < 10) 
		return v2;  
	if (3 * vH < 20) 
		return v1 + (v2 - v1) * ((20 / 3) - vH) * 6/100;   
	return (v1);
}

void HSL2RGB(uint16_t H,uint16_t S,uint16_t L)
{    
	uint16_t R,G,B;   
	uint16_t var_1, var_2; 
	uint16_t h = 10*H/65535;
	uint8_t s = 10*S/65535;
	uint8_t l = 10*L/65535;
    //co_printf("%x %x %x",H,S,L);
	// L:[0-1] S:[0-1] H:[0-1]   * 10
	// L:[0-10] S:[0-10] H:[0-10]
	if (s == 0)                           
	{        
		R = l * 0xffff/10;                    
		G = l * 0xffff/10;     
		B = l * 0xffff/10;  
	}   
	else   
	{        
		if (l < 5) 
		{
			var_2 = l * (10 + s)/10;  
		}
		else        
			var_2 = (l + s) - (s * l/10);   
		var_1 = 2 * l - var_2;    
		R = 0xffff * Hue2RGB(var_1, var_2, h + (10 / 3))/10;    
		G = 0xffff * Hue2RGB(var_1, var_2, h)/10;     
		B = 0xffff * Hue2RGB(var_1, var_2, (char)(h - (10 / 3)))/10;   
	}    

    //co_printf("R=%x,G=%x,B=%x\r\n",(uint16_t)R,(uint16_t)G,(uint16_t)B);
	app_led_state.level[R_LED] = R*app_led_state.ctl_lightness/0xffff;
	app_led_state.level[G_LED] = G*app_led_state.ctl_lightness/0xffff;
	app_led_state.level[B_LED] = B*app_led_state.ctl_lightness/0xffff;

	if(APP_LED_COUNT == 3)
	{
        app_led_set_state();
	}
}
// HSL TO RGB***************


static void app_led_calc_pwm_count(uint8_t *high_duty,uint16_t level)
{
  *high_duty = 100*level/0xffff;
}

/*********************************************************************
 * @fn      app_led_set_state
 *
 * @brief   update LED state.
 *
 * @param   index   - which led should be updated.
 *
 * @return  None.
 */
static void app_led_set_state(void)
{
    uint8_t high_duty,i,pwm_chl;

    for(i = 0;i < APP_LED_COUNT;i++)
    {
        if(app_led_state.on_off_state[i] == 0)
        {
            gpio_portd_write(gpio_portd_read() & (~(1<<(app_led_pwm_map[i]))));
            system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)(app_led_pwm_map[i]), PORT_FUNC_GPIO);
        }
        else
        {
            if(app_led_state.level[i] == 0xFFFF)
            {
                gpio_portd_write(gpio_portd_read() | (1<<(app_led_pwm_map[i])));
                system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)(app_led_pwm_map[i]), PORT_FUNC_GPIO);
            }
            else
            {
                app_led_calc_pwm_count(&high_duty, app_led_state.level[i]);
                if(i == 0)
                    pwm_chl = 0;
                else
                    pwm_chl = app_led_pwm_map[i];
                pwm_update((enum pwm_channel_t)pwm_chl, FREQUENCY_SET, high_duty);
                pwm_start((enum pwm_channel_t)pwm_chl);
                system_set_port_mux(GPIO_PORT_D, (enum system_port_bit_t)app_led_pwm_map[i], PORT_FUNC_PWM);
            }
        }
    }
    if(app_led_inited)
    {
        app_mesh_user_data_update((void *)&app_led_state, sizeof(app_led_state), 2000);
    }
}

/*********************************************************************
 * @fn      app_led_init
 *
 * @brief   initialize LED state after power on.
 *
 * @param   None.
 *
 * @return  None.
 */
void app_led_init(void)
{
    /* set LED output to low level */
    gpio_portd_write(gpio_portd_read() & 0x1f);
    
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_4, PORTD4_FUNC_D4);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_5, PORTD5_FUNC_D5);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, PORTD6_FUNC_D6);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_4, GPIO_DIR_OUT);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_5, GPIO_DIR_OUT);
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_6, GPIO_DIR_OUT);
    system_set_port_pull(GPIO_PD4|GPIO_PD5|GPIO_PD6, true);

    /* set lightness control PA2 */
    #if 0
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_2, PORTA2_FUNC_PWM2);
    pwm_init(PWM_CHANNEL_2, 8000, 90);
    pwm_start(PWM_CHANNEL_2);
    #endif
    /* confirm this pin is controlled by CPU */
    pmu_set_pin_to_CPU(GPIO_PORT_D, (1<<GPIO_BIT_4)|(1<<GPIO_BIT_5)|(1<<GPIO_BIT_6));

    memset(&app_led_state, 0, sizeof(app_led_state));

    if(app_mesh_user_data_load((uint8_t *)&app_led_state, sizeof(app_led_state)) == false)
    {
        for(uint8_t i = 0;i < APP_LED_COUNT;i++)
        {
            app_led_state.on_off_state[i] = 1;
            app_led_state.level[i] = 0xFFFF;
        }
        app_led_state.ctl_lightness = 0xFFFF;
        app_led_state.ctl_temperature = TEMP_WARM;
        // red
        app_led_state.hsl_hue = 0x8000;
        app_led_state.hsl_saturation = 0x0000;
        app_led_state.hsl_lightness = 0xFFFF;
        app_led_state.remote_dev_src = 0;
        app_led_state.appkey_id = 0;
    }

    /* use stored information or default value to update LED state */
    app_led_set_state();
    //app_led_set_state(1);
    app_mesh_store_user_data_timer_init();

    app_led_inited = true;
}

/*********************************************************************
 * @fn      app_led_set_remote_msg
 *
 * @brief   Store the msg about Tmall-elves.
 *
 * @param   src_id       - Source address about Tmall-elves.
 *
 * @param   appkey       - Application Key Local identifier.
 *
 * @return  None.
 */
void app_led_set_remote_msg(uint16_t src_id,uint8_t appkey)
{
    app_led_state.remote_dev_src = src_id;
    app_led_state.appkey_id = appkey;
    app_mesh_user_data_update((void *)&app_led_state, sizeof(app_led_state), 2000);
}

/*********************************************************************
 * @fn      app_led_get_remote_msg
 *
 * @brief   Get the msg about Tmall-elves.
 *
 * @param   src_id       - Source address about Tmall-elves.
 *
 * @param   appkey       - Application Key Local identifier.
 *
 * @return  None.
 */
void app_led_get_remote_msg(uint16_t * src_id,uint8_t * appkey)
{
    *src_id = app_led_state.remote_dev_src;
    *appkey = app_led_state.appkey_id;
}


/*********************************************************************
 * @fn      app_led_get_onoffstate
 *
 * @brief   Get specified current LED OnOff state.
 *
 * @param   index       - which LED OnOff state should get.
 *
 * @return  uint8_t     - returned LED OnOff state.
 */
uint8_t app_led_get_onoffstate(uint8_t index)
{
    return app_led_state.on_off_state[index];
}

/*********************************************************************
 * @fn      app_led_get_level
 *
 * @brief   Get specified current LED level.
 *
 * @param   index       - which LED level should get.
 *
 * @return  uint8_t     - returned LED level.
 */
uint16_t app_led_get_level(uint8_t index)
{
    return app_led_state.level[index];
}

/*********************************************************************
 * @fn      app_led_get_ctl_lightness
 *
 * @brief   Get specified current LED ctl lightness number.
 *
 * @param   None.
 *
 * @return  uint8_t     - returned LED lightness.
 */
uint16_t app_led_get_ctl_lightness(void)
{
    return app_led_state.ctl_lightness;
}

/*********************************************************************
 * @fn      app_led_get_hsl
 *
 * @brief   Get specified current LED hsl number.
 *
 * @param   None.
 *
 * @return  uint16_t     - returned LED hsl number.
 */
uint16_t app_led_get_hsl(void)
{
    return app_led_state.hsl_hue;
}

/*********************************************************************
 * @fn      app_led_get_ctl_temperature
 *
 * @brief   Get specified current LED temperature number.
 *
 * @param   None.
 *
 * @return  uint16_t     - returned LED temperature number.
 */
uint16_t app_led_get_ctl_temperature(void)
{
    return app_led_state.ctl_temperature;
}

/*********************************************************************
 * @fn      app_led_set_onoffstate
 *
 * @brief   Set specified LED OnOff state, used to turn on/off LED.
 *
 * @param   state  - on/off state of the led.
 *
 * @return  None.
 */
void app_led_set_onoffstate(uint8_t state)
{
    for(uint8_t i = 0;i < APP_LED_COUNT;i++)
    {
        if(app_led_state.on_off_state[i] != state)
            app_led_state.on_off_state[i] = state;
    }
    app_led_set_state();
}

/*********************************************************************
 * @fn      app_led_set_hsl
 *
 * @brief   Set specified LED hsl number.
 *
 * @param   hue - hue value to set.
 *
 * @param   saturation - saturation value to set.
 *
 * @param   lightness - lightness value to set.
 *
 * @return  None.
 */
void app_led_set_hsl(uint16_t hue,uint16_t saturation,uint16_t lightness)
{
    if((app_led_state.hsl_lightness == lightness) && (app_led_state.hsl_hue == hue) &&\
        (app_led_state.hsl_saturation == saturation))
    {
        return;
    }

    app_led_state.hsl_hue = hue;
    app_led_state.hsl_saturation = saturation;
    app_led_state.hsl_lightness = lightness;
    HSL2RGB(hue,saturation,lightness);
    //app_led_set_state();
}

/*********************************************************************
 * @fn      app_led_set_lightness
 *
 * @brief   Set specified LED lightness.
 *
 * @param   lightness - lightness value to set.
 *
 * @return  None.
 */
void app_led_set_lightness(uint16_t lightness)
{
    if(app_led_state.ctl_lightness == lightness)
    {
        return;
    }

    app_led_state.ctl_lightness = lightness;
    if(APP_LED_COUNT == 2)
    {
        app_led_state.level[W_LED] = (TEMP_WARM-app_led_state.ctl_temperature)*lightness/(TEMP_WARM-TEMP_COLD);
        app_led_state.level[C_LED] = (app_led_state.ctl_temperature-TEMP_COLD)*lightness/(TEMP_WARM-TEMP_COLD);
        app_led_set_state();
    }
    else if(APP_LED_COUNT == 3)
    {
        co_printf("=led lightness=%x\r\n",app_led_state.ctl_lightness);
        HSL2RGB(app_led_state.hsl_hue,app_led_state.hsl_saturation,app_led_state.hsl_lightness);
    }
}

/*********************************************************************
 * @fn      app_led_set_CTL
 *
 * @brief   Set specified LED level.
 *
 * @param   lightness       - lightness value to set.
 *          temperature     - temperature valueto set
 *
 * @return  None.
 */
void app_led_set_CTL(uint16_t lightness, uint16_t temperature)
{
    if((app_led_state.ctl_lightness == lightness)
        && (app_led_state.ctl_temperature == temperature))
    {
        return;
    }

    app_led_state.ctl_lightness = lightness;
    app_led_state.ctl_temperature = temperature;

    app_led_state.level[W_LED] = (TEMP_WARM-temperature)*lightness/(TEMP_WARM-TEMP_COLD);
    app_led_state.level[C_LED] = (temperature-TEMP_COLD)*lightness/(TEMP_WARM-TEMP_COLD);
    app_led_set_state();
}

