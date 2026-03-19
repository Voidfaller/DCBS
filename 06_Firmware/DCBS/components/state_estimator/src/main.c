
// alleen voor het testen van de state estimator, niet in de uiteidelijke codebase!
// print hoeken
gyro_data_t gyro;
accel_data_t accel;

while (1)
{
    gyro_driver_read_gyro(&gyro);
    gyro_driver_read_accel(&accel);

    state_estimator_update(&gyro, &accel, 0.01f); // 100 Hz

    orientation_t o = state_estimator_get_orientation();

    printf("Pitch: %.2f | Roll: %.2f\n", o.pitch, o.roll);

    vTaskDelay(pdMS_TO_TICKS(10));
}