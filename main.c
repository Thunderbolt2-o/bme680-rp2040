#include <stdio.h>
#include "bme68x/bme68x.h"
#include "bme68x/bme68x_defs.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "pico/binary_info.h"

/***********************************************************************/
/*                         Macros                                      */
/***********************************************************************/

/* Macro for count of samples to be displayed */
#define SAMPLE_COUNT  UINT16_C(300)

/***********************************************************************/
/*                         Test code                                   */
/***********************************************************************/


int write_blocking(uint8_t addr, const uint8_t *src, size_t len, bool nostop) {
        return i2c_write_blocking(i2c_default, addr, src, len, nostop);
    }

    int read_blocking(uint8_t addr, uint8_t *dst, size_t len, bool nostop) {
        return i2c_read_blocking(i2c_default, addr, dst, len, nostop);
    }


 static int write_bytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t length) {
    
                uint8_t buffer[length + 1];
                buffer[0] = reg_addr;
                for(uint32_t x = 0u; x < length; x++) {
                    buffer[x + 1] = reg_data[x];
                }

                int result = write_blocking(BME68X_I2C_ADDR_LOW, buffer, length + 1, false);

                return result == PICO_ERROR_GENERIC ? 1 : 0;
            };

static int read_bytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t length) {
                
                int result = write_blocking(BME68X_I2C_ADDR_LOW, &reg_addr, 1, true);
                result =read_blocking(BME68X_I2C_ADDR_LOW, reg_data, length, false);

                return result == PICO_ERROR_GENERIC ? 1 : 0;
            };

void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    busy_wait_us_32(period);
}

void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:

            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}



int main(void)
{
    stdio_init_all();

    sleep_ms(8000);

    #if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c / bmp280_i2c example requires a board with I2C pins
        puts("Default I2C pins were not defined");
    return 0;
    #else
    // useful information for picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    struct bme68x_dev bme;
    bme.intf = BME68X_I2C_INTF;
    bme.read = (bme68x_read_fptr_t)read_bytes;
    bme.write = (bme68x_write_fptr_t)write_bytes;
    bme.delay_us = (bme68x_delay_us_fptr_t)bme68x_delay_us;
    bme.amb_temp = 25;
    int8_t rslt;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data;
    uint32_t del_period;
    uint32_t time_ms = 0;
    uint8_t n_fields;
    uint16_t sample_count = 1;

    if (bme68x_init(&bme) != BME68X_OK) {
    char bme_msg[] = "BME680 Initialization Error\r\n";
    printf(bme_msg);
    } else {
    char bme_msg[] = "BME680 Initialized and Ready\r\n";
    printf("Driver code is connected \n");
    printf(bme_msg);
    }
        conf.filter = BME68X_FILTER_OFF;
        conf.odr = BME68X_ODR_NONE;
        conf.os_hum = BME68X_OS_16X;
        conf.os_pres = BME68X_OS_1X;
        conf.os_temp = BME68X_OS_2X;
        rslt = bme68x_set_conf(&conf, &bme);
        bme68x_check_rslt("bme68x_set_conf", rslt);

        heatr_conf.enable = BME68X_ENABLE;
        heatr_conf.heatr_temp = 300;
        heatr_conf.heatr_dur = 100;
        rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme);
        bme68x_check_rslt("bme68x_set_heatr_conf", rslt);

        printf("Sample, TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status\n");

        while (sample_count <= SAMPLE_COUNT)
        {
            rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);
            bme68x_check_rslt("bme68x_set_op_mode", rslt);

            /* Calculate delay period in microseconds */
            del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme) + (heatr_conf.heatr_dur * 1000);
            bme.delay_us(del_period, bme.intf_ptr);

            time_ms = time_us_32();

            /* Check if rslt == BME68X_OK, report or handle if otherwise */
            rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme);
            bme68x_check_rslt("bme68x_get_data", rslt);

            if (n_fields)
            {
    #ifdef BME68X_USE_FPU
                printf("%u, %lu, %.2f, %.2f, %.2f, %.2f, 0x%x\n",
                    sample_count,
                    (long unsigned int)time_ms,
                    data.temperature,
                    data.pressure,
                    data.humidity,
                    data.gas_resistance,
                    data.status);
    #else
                printf("%u, %lu, %d, %lu, %lu, %lu, 0x%x\n",
                    sample_count,
                    (long unsigned int)time_ms,
                    (data.temperature / 100),
                    (long unsigned int)data.pressure,
                    (long unsigned int)(data.humidity / 1000),
                    (long unsigned int)data.gas_resistance,
                    data.status);
    #endif
                sample_count++;
            }
        }

    #endif

        return rslt;
}