#ifndef _DRIVER_I2S_H
#define _DRIVER_I2S_H

#include <stdint.h>

#define I2S_FIFO_DEPTH  64

#define I2S_INT_STATUS_RX_FULL          (1<<0)
#define I2S_INT_STATUS_RX_HFULL         (1<<1)
#define I2S_INT_STATUS_RX_EMPTY         (1<<2)
#define I2S_INT_STATUS_TX_FULL          (1<<3)
#define I2S_INT_STATUS_TX_HEMPTY        (1<<4)
#define I2s_INT_STATUS_TX_EMPTY         (1<<5)

enum i2s_dir_t
{
    I2S_DIR_TX = (1<<0),
    I2S_DIR_RX = (1<<1),
};

enum i2s_mode_t
{
    I2S_MODE_SLAVE = 0,
    I2S_MODE_MASTER = 1,
};

enum i2s_data_type_t
{
    I2S_DATA_MONO = 0,
    I2S_DATA_STEREO = 1,
};
/*
 * TYPEDEFS
 */
struct i2s_control_t {
	uint32_t en:1;
	uint32_t format:1;
	uint32_t dlyen:1;
	uint32_t frminv:1;
	uint32_t bclkinv:1;
	uint32_t mode:1;	// 1: master, 0: slave
	uint32_t lp:1;
	uint32_t rx_int_en:1;
	uint32_t tx_int_en:1;
	uint32_t inten:1;
	uint32_t reserved:22;
};

struct i2s_int_status_t {
	uint32_t rx_full:1;
	uint32_t rx_half_full:1;
	uint32_t rx_empty:1;
	uint32_t tx_full:1;
	uint32_t tx_half_empty:1;
	uint32_t tx_emtpy:1;
	uint32_t reserved:26;
};

struct i2s_int_en_t {
	uint32_t rx_full:1;
	uint32_t rx_half_full:1;
	uint32_t rx_empty:1;
	uint32_t tx_full:1;
	uint32_t tx_half_empty:1;
	uint32_t tx_emtpy:1;
	uint32_t reserved:26;
};

struct i2s_reg_t {
	struct i2s_control_t ctrl;		// @0x00
	uint32_t bclk_div;
	uint32_t frm_div;
	uint32_t data;
	struct i2s_int_status_t status; // @0x10
	struct i2s_int_en_t mask;
};

extern volatile struct i2s_reg_t *i2s_reg;
/*********************************************************************
 * @fn      i2s_init
 *
 * @brief   Initialize i2s.
 *
 * @param   type        - @ref i2s_dir_t.
 *          sample_rate - i2s bus bit clock
 *          mode        - which mode i2s works in. @ref i2s_mode_t
 *
 * @return  None.
 */
void i2s_init(uint8_t type, uint32_t sample_rate, uint8_t mode);

/*********************************************************************
 * @fn      i2s_start
 *
 * @brief   start i2s.
 *
 * @param   None.
 *
 * @return  None.
 */
void i2s_start(void);

/*********************************************************************
 * @fn      i2s_stop
 *
 * @brief   stop i2s.
 *
 * @param   None.
 *
 * @return  None.
 */
void i2s_stop(void);

/*********************************************************************
 * @fn      i2s_get_int_status
 *
 * @brief   get current i2s interrupt status.
 *
 * @param   None.
 *
 * @return  current interrupt status.
 */
uint32_t i2s_get_int_status(void);

/*********************************************************************
 * @fn      i2s_get_data
 *
 * @brief   read i2s data from rx fifo.
 *
 * @param   buffer  - pointer to a buffer used to store data.
 *          length  - how many data to read, should no larger than I2S_FIFO_DEPTH
 *          type    - data is mono (2 bytes for one sample) or stereo (4 bytes for
 *                    one sample), @ref i2s_data_type_t.
 *
 * @return  None.
 */
void i2s_get_data(void *buffer, uint8_t length, uint8_t type);

/*********************************************************************
 * @fn      i2s_send_data
 *
 * @brief   write data to i2s tx fifo.
 *
 * @param   buffer  - pointer to a buffer store avaliable data.
 *          length  - how many data to send, should no larger than I2S_FIFO_DEPTH
 *          type    - data is mono (2 bytes for one sample) or stereo (4 bytes for
 *                    one sample), @ref i2s_data_type_t.
 *
 * @return  None.
 */
void i2s_send_data(void *buffer, uint8_t length, uint8_t type);

#endif  // _DRIVER_I2S_H

