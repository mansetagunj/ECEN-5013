## Author - Gunj Manseta (University of Colorado Boulder)  
### Target Board - Tiva C Series Launchpad (EK-TM4C1294XL)  

1. APES_LEDBLINK_Q2  
  Bare metal firmware for EK-TM4C1294XL using TI peripheral driverlib.  
  2 user LED blinks at a rate of 2Hz and the UART(BAUD-115200) prints the number of times LED blinks. 
2. APES_FreeRTOS_Q3  
  FreeRTOS example for EK-TM4C1294X along with TI peripheral driverlib. Use of Tasks, UART, Timers, Mutex.  
  2 tasks creates 2 timers for blinking the LED at 2Hz and 4Hz respectively.   
  UART is used as the STDIO.
3. APES_FreeRTOS_Q4  
  FreeRTOS example for EK-TM4C1294X along with TI peripheral driverlib. Use of Tasks, Notifications, UART, Timers, Mutex.
  A worker task waiting on other tasks to Notify the work to do.
  2 tasks creates 2 timers of 2Hz and 4Hz.   
    The 2Hz task sends notification of LOG_STRING to the worker task to log a Message & tickcount via a Queue onto the STDOUT(UART).    
    The 4Hz task sends notification of TOGGLE_LED to the worker task to toggle the 2 user LEDs.  
  UART is used as the STDIO.
