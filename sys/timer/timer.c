#include <defs.h>
#include <sys/irq.h>
#include <sys/kstring.h>
#include <sys/kstdio.h>

/* This will keep track of how many ticks that the system
*  has been running for */
int timer_ticks = 0;

uint32_t time_sec = 0;
uint32_t time_min = 0;
uint32_t time_hour = 0;

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler()
{
    static int once = 0;
    static char *video_buf = NULL;

    if(once == 0)
    {
	once = 1;

	video_buf = (char*)(0xb8000 + 4000);     /* end of video memory */
	*(video_buf-2) = *(video_buf-4) = '0';
	*(video_buf-8) = *(video_buf-10) = '0';
	*(video_buf-14) = *(video_buf-16) = '0';
	*(video_buf-6) = *(video_buf-12) = ':';
    }

    /* Increment our 'tick count' */
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
    if (timer_ticks % PIT_CLOCK_HZ == 0)
    {
        timer_ticks = 0;

	if(++time_sec == 60)
        {
            time_sec = 0;
	    if(++time_min == 60)
            {
                 time_min = 0;
                 ++time_hour;

                 *(video_buf-16) = '0' + (time_hour/10);
                 *(video_buf-14) = '0' + (time_hour%10);
            }

            *(video_buf-10) = '0' + (time_min/10);
            *(video_buf-8) = '0' + (time_min%10);	    	
        } 

        *(video_buf-4) = '0' + (time_sec/10);
	*(video_buf-2) = '0' + (time_sec%10);

        //printf("%d:%d:%d\n",time_hour,time_min,time_sec);
    }
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);

    /* Configure timer to 100 Hz */
    timer_phase(PIT_CLOCK_HZ); /* Sys clock is 1193190 Hz */
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

