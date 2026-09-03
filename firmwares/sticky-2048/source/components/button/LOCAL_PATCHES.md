# Local patches

This component is based on `espressif/button` 4.1.6.

Local changes:

- Make the GPIO power-save interrupt cache-safe. The ISR only defers work with
  `xTimerPendFunctionCallFromISR()`. Restarting the button timer and disabling
  GPIO wakeup are handled outside the ISR so button wakeups cannot execute flash
  code while the cache is disabled.
