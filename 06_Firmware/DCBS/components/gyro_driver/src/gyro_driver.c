#include "gyro_driver.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MPU6050_ADDR 0x68  // I2C address of the MPU6050 sensor! zie Datasheet pagina 9

//de registers
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 100000

static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ
    };

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK)
        return err;

    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

esp_err_t gyro_driver_init(void)
{
    esp_err_t err = i2c_master_init();
    if (err != ESP_OK)
        return err;

    uint8_t data[2];
    data[0] = MPU6050_PWR_MGMT_1;  // sensor wakker maken zie Datasheet pagina 24
    data[1] = 0x00;

    return i2c_master_write_to_device(
        I2C_MASTER_NUM,
        MPU6050_ADDR,
        data,
        2,
        pdMS_TO_TICKS(100));
}

esp_err_t gyro_driver_read_gyro(gyro_data_t *data)
{
    uint8_t reg = MPU6050_GYRO_XOUT_H;
    uint8_t buffer[6];

    esp_err_t err = i2c_master_write_read_device(
        I2C_MASTER_NUM,
        MPU6050_ADDR,
        &reg,
        1,
        buffer,
        6,
        pdMS_TO_TICKS(100));

    if (err != ESP_OK)
        return err;

    int16_t gx = (buffer[0] << 8) | buffer[1];
    int16_t gy = (buffer[2] << 8) | buffer[3];
    int16_t gz = (buffer[4] << 8) | buffer[5];

    data->gyro_x = gx / 131.0f;
    data->gyro_y = gy / 131.0f;
    data->gyro_z = gz / 131.0f;

    return ESP_OK;
}

esp_err_t gyro_driver_read_accel(accel_data_t *data)
{
    uint8_t reg = MPU6050_ACCEL_XOUT_H;
    uint8_t buffer[6];

    esp_err_t err = i2c_master_write_read_device(
        I2C_MASTER_NUM,
        MPU6050_ADDR,
        &reg,
        1,
        buffer,
        6,
        pdMS_TO_TICKS(100));

    if (err != ESP_OK)
        return err;

    int16_t ax = (buffer[0] << 8) | buffer[1];
    int16_t ay = (buffer[2] << 8) | buffer[3];
    int16_t az = (buffer[4] << 8) | buffer[5];

    data->accel_x = ax / 16384.0f;
    data->accel_y = ay / 16384.0f;
    data->accel_z = az / 16384.0f;

    return ESP_OK;
}