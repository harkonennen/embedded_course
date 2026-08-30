// Viikkotehtävä 2. RTOS-ohjelmointi (osa 1)
// Tavoiteltu pistemäärä 3/3
//Koodissa on kaikki tehtävänannossa kerrotut ominaisuudet
//Toimii Nordicilla vaatimuksien mukaisesti
// Buttonien handlerit ja taskit voi varmasti tehdä fiksummin, mutta sitä toivottavasti oppii kurssin aikana



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

volatile int tila = 0; // o = idle, 1 = red, 2 = yellow, 3 = green, 4 = blue
volatile int suunta = 0; //0 = alas 1 = ylös
volatile int button_0_state;
volatile int button_1_state;
volatile int button_2_state;
volatile int button_3_state;
volatile int button_4_state;
volatile int button_4_press = 0;


// Configure buttons
#define BUTTON_0 DT_ALIAS(sw0)
static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET_OR(BUTTON_0, gpios, {0});
static struct gpio_callback button_0_data;

#define BUTTON_1 DT_ALIAS(sw1)
static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET_OR(BUTTON_1, gpios, {0});
static struct gpio_callback button_1_data;

#define BUTTON_2 DT_ALIAS(sw2)
static const struct gpio_dt_spec button_2 = GPIO_DT_SPEC_GET_OR(BUTTON_2, gpios, {0});
static struct gpio_callback button_2_data;

#define BUTTON_3 DT_ALIAS(sw3)
static const struct gpio_dt_spec button_3 = GPIO_DT_SPEC_GET_OR(BUTTON_3, gpios, {0});
static struct gpio_callback button_3_data;

#define BUTTON_4 DT_ALIAS(sw4)
static const struct gpio_dt_spec button_4 = GPIO_DT_SPEC_GET_OR(BUTTON_4, gpios, {0});
static struct gpio_callback button_4_data;


// Led pin configurations
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec blue = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

// Led thread initialization
#define STACKSIZE 500
#define PRIORITY 5
//Red led
void red_led_task(void *, void *, void*);
K_THREAD_DEFINE(red_thread,STACKSIZE,red_led_task,NULL,NULL,NULL,PRIORITY,0,0);

// Green led thread initialization
void green_led_task(void *, void *, void*);
K_THREAD_DEFINE(green_thread,STACKSIZE,green_led_task,NULL,NULL,NULL,PRIORITY,0,0);

// Blue led thread initialization
void blue_led_task(void *, void *, void*);
K_THREAD_DEFINE(blue_thread,STACKSIZE,blue_led_task,NULL,NULL,NULL,PRIORITY,0,0);

// Yellow led thread initialization
void yellow_led_task(void *, void *, void*);
K_THREAD_DEFINE(yellow_thread,STACKSIZE,yellow_led_task,NULL,NULL,NULL,PRIORITY,0,0);

//Button 4 task
void button_4_task(void *, void *, void*);
K_THREAD_DEFINE(button_4_task_thread, STACKSIZE, button_4_task,NULL, NULL, NULL, PRIORITY,0,0);

// Button interrupt handler
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 0 pressed\n");
	if (tila > 0){
		button_0_state = tila;
		tila = 0;
		printk("tila = %d \n button_state = %d\n", tila, button_0_state);
		gpio_pin_set_dt(&red,0);
		gpio_pin_set_dt(&green,0);
		printk("Leds off");
	} else if (tila == 0) {
		printk("tila = %d \n button_state = %d\n", tila, button_0_state);
		tila = button_0_state;
	}
}

void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 1 pressed\n");
	if (tila == 0 && button_1_state == 0){
		button_1_state = 1;
		gpio_pin_set_dt(&red,1);
		printk("button_1_state = %d\n",button_1_state);
	} else if (tila == 0 && button_1_state == 1) {
		button_1_state = 0;
		gpio_pin_set_dt(&red,0);
		printk("button_1_state = %d\n",button_1_state);
	}
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 2 pressed\n");
	if (tila == 0 && button_2_state == 0){
		button_2_state = 1;
		gpio_pin_set_dt(&red,1);
		gpio_pin_set_dt(&green,1);
		printk("button_2_state = %d\n",button_2_state);
	} else if (tila == 0 && button_2_state == 1) {
		button_2_state = 0;
		gpio_pin_set_dt(&red,0);
		gpio_pin_set_dt(&green,0);
		printk("button_2_state = %d\n",button_2_state);
	}
}

void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 3 pressed\n");
	if (tila == 0 && button_3_state == 0){
		button_3_state = 1;
		gpio_pin_set_dt(&green,1);
		printk("button_3_state = %d\n",button_3_state);
	} else if (tila == 0 && button_3_state == 1) {
		button_3_state = 0;
		gpio_pin_set_dt(&green,0);
		printk("button_3_state = %d\n",button_3_state);
	}
}

void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button_4 pressed\n");

	if(tila == 0){
		button_4_press = 1;
	}
}



// Main program
int main(void)
{
	init_led();
	
	int ret = init_button();
	if (ret < 0) {
		return 0;
	}
	//init state
	tila = 5;
	suunta = 0;
	return 0;
}

// Initialize leds
int  init_led() {

	// Red led pin initialization
	int ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error: red Led configure failed\n");		
		return ret;
	}
	// set led off
	gpio_pin_set_dt(&red,0);

	printk("Red led initialized ok\n");
	
	// Green led pin initialization
	ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error: Green led configure failed\n");		
		return ret;
	}
	// set led off
	gpio_pin_set_dt(&green,0);

	printk("Green led initialized ok\n");
	
	// BLue Led pin initialization
	ret = gpio_pin_configure_dt(&blue, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error: Blue led configure failed\n");		
		return ret;
	}
	// set led off
	gpio_pin_set_dt(&blue,0);

	printk("Blue led initialized ok\n");
	

	return 0;
}

// Button initialization
int init_button() {

	int ret;
	//Button 0
	if (!gpio_is_ready_dt(&button_0)) {
		printk("Error: button 0 is not ready\n");
		return -1;
	}

	ret = gpio_pin_configure_dt(&button_0, GPIO_INPUT);
	if (ret != 0) {
		printk("Error: failed to configure pin\n");
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button_0, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error: failed to configure interrupt on pin\n");
		return -1;
	}

	gpio_init_callback(&button_0_data, button_0_handler, BIT(button_0.pin));
	gpio_add_callback(button_0.port, &button_0_data);
	printk("Set up button 0 ok\n");
	
	//Button 1
    if (!gpio_is_ready_dt(&button_1)) {
        printk("Error: button 1 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_1, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure interrupt on pin\n");
        return -1;
    }

    gpio_init_callback(&button_1_data, button_1_handler, BIT(button_1.pin));
    gpio_add_callback(button_1.port, &button_1_data);
    printk("Set up button 1 ok\n");

	//Button 2
    if (!gpio_is_ready_dt(&button_2)) {
        printk("Error: button 2 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_2, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_2, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure interrupt on pin\n");
        return -1;
    }

    gpio_init_callback(&button_2_data, button_2_handler, BIT(button_2.pin));
    gpio_add_callback(button_2.port, &button_2_data);
    printk("Set up button 2 ok\n");

	//Button 3
    if (!gpio_is_ready_dt(&button_3)) {
        printk("Error: button_3 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_3, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_3, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure interrupt on pin\n");
        return -1;
    }

    gpio_init_callback(&button_3_data, button_3_handler, BIT(button_3.pin));
    gpio_add_callback(button_3.port, &button_3_data);
    printk("Set up button_3 ok\n");

	//Button 4
    if (!gpio_is_ready_dt(&button_4)) {
        printk("Error: button_4 is not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_4, GPIO_INPUT);
    if (ret != 0) {
        printk("Error: failed to configure pin\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button_4, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error: failed to configure interrupt on pin\n");
        return -1;
    }

    gpio_init_callback(&button_4_data, button_4_handler, BIT(button_4.pin));
    gpio_add_callback(button_4.port, &button_4_data);
    printk("Set up button_4 ok\n");

	return 0;
}

// Task to handle red led
void red_led_task(void *, void *, void*) {
	printk("Red led thread started\n");
	while (true) {
		if (tila == 1) {
		// 1. set led on 
		gpio_pin_set_dt(&red,1);
		printk("Red on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
		if (tila != 1) continue;
		// 3. set led off
		gpio_pin_set_dt(&red,0);
		printk("Red off\n");
		// 4. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
		if (tila != 1) continue;
		
		tila = 2;
		suunta = 0;
		}
		k_yield();
	}
}

// Task for Yellow
void yellow_led_task(void *, void *, void*) {
	
	printk("Yellow led thread started\n");
	while (true) {
		if (tila == 2){

			// 1. set led on 
			gpio_pin_set_dt(&red,1);
			gpio_pin_set_dt(&green,1);
			printk("Yellow on\n");
			// 2. sleep for 2 seconds
			k_sleep(K_SECONDS(1));
			if (tila != 2) continue;
			// 3. set led off
			gpio_pin_set_dt(&red,0);
			gpio_pin_set_dt(&green,0);
			printk("Yellow off\n");
			// 4. sleep for 2 seconds
			k_sleep(K_SECONDS(1));
			if (tila != 2) continue;

			if (suunta == 0){
				tila = 3;
			} else if (suunta == 1){
				tila = 1;
			}
		}
		k_yield();
	}
}


// Task to handle Green led
void green_led_task(void *, void *, void*) {
	
	printk("Green led thread started\n");
	while (true) {
		if (tila == 3){
			
			// 1. set led on 
			gpio_pin_set_dt(&green,1);
			printk("green on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
		if (tila != 3) continue;
		// 3. set led off
		gpio_pin_set_dt(&green,0);
		printk("Green off\n");
		// 4. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
		if (tila != 3) continue;
		
		suunta = 1;
		tila = 2;
		}
		k_yield();	
	}
}

// Task to handle BLue led
void blue_led_task(void *, void *, void*) {
	
	printk("Blue led thread started\n");
	while (true) {
		if(tila == 5){

			// 1. set led on 
			gpio_pin_set_dt(&blue,1);
			printk("Blue on\n");
		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(3));
		if (tila != 5) continue;
		// 3. set led off
		gpio_pin_set_dt(&blue,0);
		printk("Blue off\n");
		// 4. sleep for 2 seconds
		k_sleep(K_SECONDS(1));
		if (tila != 5) continue;

		tila = 1;
		}
		k_yield();
	}
}

//Task for button 4 flashing
void button_4_task(void*, void*, void*){
	printk("Button 4 task thread started\n");
	while(true){
		if (tila ==  0 && button_4_press == 1){
			button_4_press = 0;
			button_4_state = !button_4_state;
			printk("Button 4 task\n");
		} if (tila == 0 && button_4_state == 1){
			gpio_pin_set_dt(&red, 1);
			gpio_pin_set_dt(&green,1);
			k_sleep(K_SECONDS(1));
			gpio_pin_set_dt(&red, 0);
			gpio_pin_set_dt(&green, 0);
			k_sleep(K_SECONDS(1));
		} else { 	
			k_yield();
		}
	}
}


