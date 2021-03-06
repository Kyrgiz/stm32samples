/*
 * main.c
 *
 * Copyright 2017 Edward V. Emelianoff <eddy@sao.ru, edward.emelianoff@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "hardware.h"
#include "usart.h"
#include "usb.h"
#include "usb_lib.h"

volatile uint32_t Tms = 0;

/* Called when systick fires */
void sys_tick_handler(void){
    ++Tms;
}

void iwdg_setup(){
    uint32_t tmout = 16000000;
    /* Enable the peripheral clock RTC */
    /* (1) Enable the LSI (40kHz) */
    /* (2) Wait while it is not ready */
    RCC->CSR |= RCC_CSR_LSION; /* (1) */
    while((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY){if(--tmout == 0) break;} /* (2) */
    /* Configure IWDG */
    /* (1) Activate IWDG (not needed if done in option bytes) */
    /* (2) Enable write access to IWDG registers */
    /* (3) Set prescaler by 64 (1.6ms for each tick) */
    /* (4) Set reload value to have a rollover each 2s */
    /* (5) Check if flags are reset */
    /* (6) Refresh counter */
    IWDG->KR = IWDG_START; /* (1) */
    IWDG->KR = IWDG_WRITE_ACCESS; /* (2) */
    IWDG->PR = IWDG_PR_PR_1; /* (3) */
    IWDG->RLR = 1250; /* (4) */
    tmout = 16000000;
    while(IWDG->SR){if(--tmout == 0) break;} /* (5) */
    IWDG->KR = IWDG_REFRESH; /* (6) */
}
/*
char *parse_cmd(char *buf){
    IWDG->KR = IWDG_REFRESH;
    if(buf[1] != '\n') return buf;
    switch(*buf){
        case 'p':
            pin_toggle(USBPU_port, USBPU_pin);
            USB_send("USB pullup is ");
            if(pin_read(USBPU_port, USBPU_pin)) USB_send("off\n");
            else USB_send("on\n");
            return NULL;
        break;
        case 'L':
            USB_send("Very long test string for USB (it's length is more than 64 bytes).\n"
                     "This is another part of the string! Can you see all of this?\n");
            return "OK\n";
        break;
        case 'R':
            USB_send("Soft reset\n");
            NVIC_SystemReset();
        break;
        case 'S':
            USB_send("Test string for USB\n");
            return "OK\n";
        break;
        case 'W':
            USB_send("Wait for reboot\n");
            while(1){nop();};
        break;
        default: // help
            return
            "'p' - toggle USB pullup\n"
            "'L' - send long string over USB\n"
            "'R' - software reset\n"
            "'S' - send short string over USB\n"
            "'W' - test watchdog\n"
            ;
        break;
    }
    return NULL;
}*/

/*
char *get_USB(){
    static char tmpbuf[65];
    int x = USB_receive(tmpbuf, 64);
    tmpbuf[x] = 0;
    if(!x) return NULL;
    return tmpbuf;
}

// usb getline
char *get_USB(){
    static char tmpbuf[128], *curptr = tmpbuf;
    static int rest = 127;
    int x = USB_receive(curptr, rest);
    curptr[x] = 0;
    if(!x) return NULL;
    if(curptr[x-1] == '\n'){
        //DBG("Got \\n");
        curptr = tmpbuf;
        rest = 127;
        return tmpbuf;
    }
    curptr += x; rest -= x;
    if(rest <= 0){ // buffer overflow
        //DBG("Buffer full");
        curptr = tmpbuf;
        rest = 127;
        return tmpbuf;
    }
    return NULL;
}*/


static uint32_t newrate = 0;
// redefine weak handlers
void linecoding_handler(usb_LineCoding *lcd){
    newrate = lcd->dwDTERate;
}
static uint16_t cl = 0xffff;
void clstate_handler(uint16_t val){
    cl = val;
}
static int8_t br = 0;
void break_handler(){
    br = 1;
}
/*
char *u2str(uint32_t val){
    static char strbuf[11];
    char *bufptr = &strbuf[10];
    *bufptr = 0;
    if(!val){
        *(--bufptr) = '0';
    }else{
        while(val){
            *(--bufptr) = val % 10 + '0';
            val /= 10;
        }
    }
    return bufptr;
}*/

int main(void){
    uint32_t lastT = 0;
    sysreset();
    StartHSE();
    SysTick_Config(72000);

    hw_setup();
    USBPU_OFF();
    usart_setup();
    DBG("Start");
    if(RCC->CSR & RCC_CSR_IWDGRSTF){ // watchdog reset occured
        SEND("WDGRESET=1"); newline();
    }
    if(RCC->CSR & RCC_CSR_SFTRSTF){ // software reset occured
        SEND("SOFTRESET=1"); newline();
    }
    RCC->CSR |= RCC_CSR_RMVF; // remove reset flags

    USB_setup();
    iwdg_setup();
    USBPU_ON();

    //uint32_t ctr = 0;
    while (1){
        IWDG->KR = IWDG_REFRESH; // refresh watchdog
        if(Tms - lastT > 499){
            LED_blink(LED0);
            lastT = Tms;
            transmit_tbuf();
        }
        usb_proc();
        if(bufovr){
            bufovr = 0;
            USB_send((uint8_t*)"USART overflow!\n", 16);
        }
        uint8_t tmpbuf[USB_RXBUFSZ], *txt;
        uint8_t x = USB_receive(tmpbuf);
        if(x){
            //for(int _ = 0; _ < 7000000; ++_)nop();
            //USB_send(tmpbuf, x);
            usart_senddata(tmpbuf, x);
            //transmit_tbuf();
           /*char *ans = parse_cmd(txt);
           if(ans) USB_send(ans);*/
        }
        if((x = usart_get(&txt))){
            USB_send(txt, x);
        }
        int n = 0;
        if(newrate){SEND("new speed: "); printu(newrate); n = 1; newrate = 0;}
        if(cl!=0xffff){SEND("controls: "); printuhex(cl); n = 1; cl = 0xffff;}
        if(br){SEND("break"); n = 1; br = 0;}
        if(n){newline(); transmit_tbuf();}
    }
    return 0;
}

