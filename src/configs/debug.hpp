#if !defined(_CONFIG_DEBUG_COLOER_HPP_)
#define _CONFIG_DEBUG_COLOER_HPP_

#include <stdio.h>

#define     NONE            "\033[m"
 
#define     RED             "\033[0;32;31m"
 
#define     LIGHT_RED       "\033[1;31m"
 
#define     GREEN           "\033[0;32;32m"
 
#define     LIGHT_GREEN     "\033[1;32m"
 
#define     LUE             "\033[0;32;34m"
 
#define     LIGHT_BLUE      "\033[1;34m"
 
#define     DARY_GRAY       "\033[1;30m"
 
#define     CYAN            "\033[0;36m"
 
#define     LIGHT_CYAN      "\033[1;36m"
 
#define     PURPLE          "\033[0;35m"
 
#define     LIGHT_PURPLE    "\033[1;35m"
 
#define     BROWN           "\033[0;33m"
 
#define     YELLOW          "\033[1;33m"
 
#define     LIGHT_GRAY      "\033[0;37m"
 
#define     WHITE           "\033[1;37m"

#define _BEAN_DEBUG_    true
#define _BRAM_DEBUG_    true
#define _CAN_DEBUG_     true
#define _GPIO_DEBUG_    false
#define _IIO_DEBUG_     false
#define _TCP_DEBUG_     true
#define _THREAD_MANAGE_DEBUG_     true
#define _SYNC_DEBUG_     true

#define _COMMAND_DEBUG_ true
#define _BIT_SYNC_DEBUG_ true

#define _T_L_RATE_DEBUG_ true


#if     _BIT_SYNC_DEBUG_ 
#define BIT_SYNC_MSG(...) printf(__VA_ARGS__)
#define BIT_SYNC_DEBUG(...) printf(__VA_ARGS__)
#else
#define BIT_SYNC_MSG(...)
#define BIT_SYNC_DEBUG(...)  
#endif


#if     _T_L_RATE_DEBUG_ 
#define T_L_RATE_MSG(...) printf(__VA_ARGS__)
#define T_L_RATE_DEBUG(...) printf(__VA_ARGS__)
#else
#define T_L_RATE_MSG(...)
#define T_L_RATE_DEBUG(...)  
#endif


#if     _SYNC_DEBUG_ 
#define SYNC_MSG(...) printf(__VA_ARGS__)
#define SYNC_DEBUG(...) printf(__VA_ARGS__)
#else
#define SYNC_MSG(...)
#define SYNC_DEBUG(...)  
#endif
#endif // _CONFIG_LOG_COLOER_HPP
