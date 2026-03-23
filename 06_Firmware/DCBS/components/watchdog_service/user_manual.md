# watchdog component

The **Watchdog Service** is a module for supervising FreeRTOS tasks and ensuring system reliability in the **Dynamic Cup Balancing System (DCBS)**. 
It monitors registered tasks and triggers a system reset if a task fails to feed the watchdog within its timeout period.


## Requirements.

- registers and monitors multiple FreeTos tasks.
- Each task had a configurable timeout in Seconds.
- automatically resets the ESP32 hardware watchdog for added safety.

## How it works:

### initialization:

- initializes the hardware watchdog and creates the watchdog monitoring task.
```c 
watchdog_init(hw_timeout_sec);
```

### registering Tasks
- each task to be supervised registers with a name and timeout.
- returns a handle used to feed the watchdog.
```c
watchdog_handle_t handle = watchdog_register("sensor_task", 5); // 5 sec timeout
```

### Feeding the watchdog 
- task must periodically call watchdog_feed() to indicate they are alive.

```c 
watchdog_feed(handle);
```

### Automatic monitoring
- the watchdog task continuously checks all registered tasks.
- if a task exceeds its timeout without feeding, the systeem prints a message and restarts.

## API reference:


| Function | Description |
|----------|-------------|
| `void watchdog_init(uint32_t hw_timeout_sec)` | Initializes the hardware watchdog with a global timeout in seconds and starts the watchdog monitoring task. |
| `watchdog_handle_t watchdog_register(const char *name, uint32_t timeout_sec)` | Registers a task with a specific timeout. Returns a handle that the task can use to feed the watchdog. |
| `void watchdog_feed(watchdog_handle_t handle)` | Feeds the watchdog for the specified task handle, preventing a timeout reset. |