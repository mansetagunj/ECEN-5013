## Author - Gunj Manseta (University of Colorado Boulder)  
### Target Board - Tiva C Series Launchpad (EK-TM4C1294XL)  

1. APES_LEDBLINK_Q2  
  Bare metal firmware for EK-TM4C1294XL using TI peripheral driverlib.  
  2 user LED blinks at a rate of 2Hz and the UART(BAUD-115200) prints the number of times LED blinks.  
  ![Copy+paste Link - https://drive.google.com/open?id=1H3n0GBLAXw2mUtPgw5tPQzAxUubKIr_e](https://github.com/mansetagunj/ECEN-5013/blob/master/HW5/Videos/Q2.gif)  
2. APES_FreeRTOS_Q3  
  FreeRTOS example for EK-TM4C1294X along with TI peripheral driverlib. Use of Tasks, UART, Timers, Mutex.  
  2 tasks creates 2 timers for blinking the LED at 2Hz and 4Hz respectively.   
  UART is used as the STDIO.  
  ![Copy+paste Link - https://drive.google.com/open?id=1snzn5ufD2P789WRSQqrE9Zo15ti-YwqW](https://github.com/mansetagunj/ECEN-5013/blob/master/HW5/Videos/Q3.gif)  
3. APES_FreeRTOS_Q4  
  FreeRTOS example for EK-TM4C1294X along with TI peripheral driverlib. Use of Tasks, Notifications, UART, Timers, Mutex.
  A worker task waiting on other tasks to Notify the work to do.
  2 tasks creates 2 timers of 2Hz and 4Hz.   
    The 2Hz task sends notification of LOG_STRING to the worker task to log a Message & tickcount via a Queue onto the STDOUT(UART).    
    The 4Hz task sends notification of TOGGLE_LED to the worker task to toggle the 2 user LEDs.  
  UART is used as the STDIO.  
  ![Copy+paste Link - https://drive.google.com/open?id=1iHu_4ayCrYUPlaMb-QhUWd6xgZluIV5L](https://github.com/mansetagunj/ECEN-5013/blob/master/HW5/Videos/Q4.gif)  
