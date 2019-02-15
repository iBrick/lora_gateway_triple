#ifndef SIMPLEGPIO_H_
#define SIMPLEGPIO_H_

 /****************************************************************
 * Constants
 ****************************************************************/

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define SYSFS_ADC_DIR "/sys/bus/iio/devices/iio:device0"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

#define LED_DIR "/sys/class/leds/beaglebone:green:usr"

enum PIN_DIRECTION{
	INPUT_PIN=0,
	OUTPUT_PIN=1
};

enum PIN_VALUE{
	P_LOW=0,
	P_HIGH=1
};

/****************************************************************
 * GPIO RELATED FUNCTIONS (DIGITAL)
 ****************************************************************/
int led_set_value(unsigned int led, PIN_VALUE value); // expects an led value : [0,3]
int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, PIN_DIRECTION out_flag);
int gpio_set_value(unsigned int gpio, PIN_VALUE value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);

/****************************************************************
 * ADC RELATED FUNCTIONS (ANALOG)
 ****************************************************************/
int adc_get_value(unsigned int adc, unsigned int *value);
int adc_fd_open(unsigned int adc);
int adc_fd_close(int fd);

#endif /* SIMPLEGPIO_H_ */
