#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>

/****************************
 * Remember to add line:
 * CONFIG_HEAP_MEM_POOL_SIZE=1024
 * to prj.conf
 ****************************/

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5


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
//

// Button interrupt handlers
//
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 0 pressed\n");
}

void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 1 pressed\n");
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 2 pressed\n");
}

void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 3 pressed\n");
}

void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button_4 pressed\n");
}
//

//Configure leds
static const struct gpio_dt_spec red = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec green = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
//

//Tasks
void red_led_task(void *, void *, void*);
void yellow_led_task(void *, void *, void*);
void green_led_task(void *, void *, void*);
void dispatcher_task(void *, void *, void*);
void uart_task(void *, void *, void*);
//

// Define threads for dispatcher and uart
K_THREAD_DEFINE(dis_thread,STACKSIZE,dispatcher_task,NULL,NULL,NULL,PRIORITY,0,0);
K_THREAD_DEFINE(uart_thread,STACKSIZE,uart_task,NULL,NULL,NULL,PRIORITY,0,0);

//Define stacks for thread memory
K_THREAD_STACK_DEFINE(red_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(yellow_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(green_stack, STACKSIZE);

//Struct for data about the thread for the kernel
static struct k_thread red_thread_data;
static struct k_thread yellow_thread_data;
static struct k_thread green_thread_data;


// UART initialization
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);



// Create dispatcher FIFO buffer
K_FIFO_DEFINE(dispatcher_fifo);

// FIFO dispatcher data type
struct data_t {
	// Add fifo_reserved below
	void *fifo_reserved;
	char msg[20];
};

// UART initialization call in MAIN
int init_uart(void) {

	if (!device_is_ready(uart_dev)) {
		return 1;
	} 
	return 0;
}

// BUTTON initilization call in  MAIN
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

// LED initilization call in MAIN
int  init_led() {

	// RED LED INIT
	int ret = gpio_pin_configure_dt(&red, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error: Led configure failed\n");		
		return ret;
	}
	gpio_pin_set_dt(&red,0);
	printk("Led initialized ok\n");

	//GREEN LED INIT
	ret = gpio_pin_configure_dt(&green, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Error: Led configure failed\n");		
		return ret;
	}
	gpio_pin_set_dt(&green,0);
	printk("Led initialized ok\n");

	return 0;
}



int main(void)
{
	int ret = init_uart();
	if (ret != 0) {
		printk("UART initialization failed!\n");
		return ret;
	}

	ret = init_led();
	if (ret != 0) {
		printk("LED initilization failed\n");
		return ret;
	}

	ret = init_button();
	if (ret != 0) {
		printk("BUTTON initilization failed\n");
		return ret;
	}
	return 0;
}

/********************
 * UART task
 */
void uart_task(void *unused1, void *unused2, void *unused3)
{
	// Received character from UART
	char rc=0;
	// Message from UART
	char uart_msg[20];
	memset(uart_msg,0,20);
	int uart_msg_cnt = 0;

	while (true) {

		// Ask UART if data available
		if (uart_poll_in(uart_dev,&rc) == 0) {

			// printk("Received: %c\n",rc);
			// If character is not newline, add to UART message buffer
			if (rc != '\r') {
				uart_msg[uart_msg_cnt] = rc;
				uart_msg_cnt++;

			// Character is newline, copy dispatcher data and put to FIFO buffer
			} else {
				printk("UART msg: %s\n", uart_msg);
                
				struct data_t *buf = k_malloc(sizeof(struct data_t));
				if (buf == NULL) {
					return;
				}

				// Copy UART message to dispatcher data
				snprintf(buf->msg, 20, "%s", uart_msg);

				// You need to:
				// Put dispatcher data to FIFO buffer
				k_fifo_put(&dispatcher_fifo, buf);

				// Clear UART receive buffer
				uart_msg_cnt = 0;
				memset(uart_msg, 0, sizeof(uart_msg));

			}
		}
		k_msleep(10);
	}
}

/********************
 * Dispatcher task
 */
void dispatcher_task(void *unused1, void *unused2, void *unused3)
{
	while (true) {
		// Receive dispatcher data from uart_task fifo
		struct data_t *rec_item = k_fifo_get(&dispatcher_fifo, K_FOREVER);
		char sequence[20];
		memcpy(sequence,rec_item->msg,20);
		k_free(rec_item);

		printk("Dispatcher: %s\n", sequence);
		int cnt = 0;

		//tulostetaan merkki kerrallaan
		while (sequence[cnt] != 0){
			//printk("%c\n", sequence[cnt]);

			if (sequence[cnt] == 'R'){
				printk("RED\n");
				//Create a thread
				k_thread_create(&red_thread_data,red_stack,
								K_THREAD_STACK_SIZEOF(red_stack),
								red_led_task,
								NULL,
								NULL,
								NULL,
								PRIORITY,
								0,
								K_NO_WAIT
							);
				//Sleep until the thread exits (red thread runs through)
				k_thread_join(&red_thread_data,K_FOREVER);
			}
			
			else if (sequence[cnt] == 'Y'){
				printk("YELLOW\n");
				//Create a thread
				k_thread_create(&yellow_thread_data,yellow_stack,
								K_THREAD_STACK_SIZEOF(yellow_stack),
								yellow_led_task,
								NULL,
								NULL,
								NULL,
								PRIORITY,
								0,
								K_NO_WAIT
							);
				//Sleep until the thread exits (red thread runs through)
				k_thread_join(&yellow_thread_data,K_FOREVER);
			}

			else if (sequence[cnt] == 'G'){
				printk("GREEN\n");
				//Create a thread
				k_thread_create(&green_thread_data,green_stack,
								K_THREAD_STACK_SIZEOF(green_stack),
								green_led_task,
								NULL,
								NULL,
								NULL,
								PRIORITY,
								0,
								K_NO_WAIT
							);
				//Sleep until the thread exits (red thread runs through)
				k_thread_join(&green_thread_data,K_FOREVER);
			} else {
				printk("Unknown input\n");
			}
			cnt ++;
		}
	}
}

void red_led_task(void *, void *, void*) {
	printk("Thread API red_led_task started\n");

		// LED ON
		gpio_pin_set_dt(&red,1);
		printk("Red on\n");

		//SLEEP
		k_sleep(K_SECONDS(1));

		//LED OFF
		gpio_pin_set_dt(&red,0);
		printk("Red off\n");
}

void green_led_task(void *, void *, void*) {
	printk("API Green led thread started\n");
		// 1. set led on 
		gpio_pin_set_dt(&green,1);
		printk("GReen on\n");

		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));

		// 3. set led off
		gpio_pin_set_dt(&green,0);
		printk("Green off\n");
}

void yellow_led_task(void *, void *, void*) {
	printk("API yellow led thread started\n");

		// 1. set led on 
		gpio_pin_set_dt(&red,1);
		gpio_pin_set_dt(&green,1);
		printk("yellow on\n");

		// 2. sleep for 2 seconds
		k_sleep(K_SECONDS(1));

		// 3. set led off
		gpio_pin_set_dt(&red,0);
		gpio_pin_set_dt(&green,0);
		printk("yellow off\n");
}