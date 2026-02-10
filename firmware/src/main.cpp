/**
 * Telemetry Node — ESP32
 * FreeRTOS: timer task produces samples, sender task encodes and streams over UART.
 * Sensors (IMU, temp, ADC) to be wired in; for now sends dummy samples.
 */
#include <Arduino.h>
#include "config.h"
#include "protocol.h"
#include "sensors.h"

static QueueHandle_t sample_queue;
static sample_t sample_pool[SAMPLE_QUEUE_LEN];
static uint8_t frame_buf[FRAME_SIZE];

static void sender_task(void *arg) {
    (void)arg;
    for (;;) {
        sample_t *s = (sample_t *)0;
        if (xQueueReceive(sample_queue, &s, portMAX_DELAY) != pdTRUE || s == (sample_t *)0)
            continue;
        size_t n = protocol_build_frame(s, frame_buf, sizeof(frame_buf));
        if (n > 0)
            Serial.write(frame_buf, n);
    }
}

static void timer_task(void *arg) {
    (void)arg;
    TickType_t last = xTaskGetTickCount();
    size_t idx = 0;
    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(SAMPLE_RATE_MS));
        sample_t *s = &sample_pool[idx % SAMPLE_QUEUE_LEN];
        s->timestamp_ms = (uint32_t)(last * portTICK_PERIOD_MS);
        sensors_read(s);
        xQueueSend(sample_queue, &s, 0);
        idx++;
    }
}

void setup() {
    Serial.begin(UART_BAUD);
    sensors_init();
    sample_queue = xQueueCreate(SAMPLE_QUEUE_LEN, sizeof(sample_t *));
    xTaskCreate(sender_task, "sender", 2048, NULL, 2, NULL);
    xTaskCreate(timer_task, "timer", 2048, NULL, 1, NULL);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
