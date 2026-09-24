// Debug tehtäviin on toteutettu 1 pisteen osio, jossa mitataan led taskien ajat
//Moodlen palautuksessa ajat debug tekstit päällä / pois

// Tähän koodiin on nyt yhdistetty toimintoja RTOS 1 ja RTOS 2 koodeista.
//Valoja voidaan ohjata laudan napeilla, sekä terminaaliin kirjoittamalla.

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/timing/timing.h>
#include <string.h>
#include <stdio.h>

// Thread initializations
#define STACKSIZE 500
#define PRIORITY 5

//Total timer for a light sequence in microseconds
uint64_t sequence_total_us = 0;

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


// Function for sending sequence to dispatcher
static void send_sequence_to_dispatcher(const char *sequence);

// Button work handler declarations
static void button_0_work_handler(struct k_work *work);
static void button_1_work_handler(struct k_work *work);
static void button_2_work_handler(struct k_work *work);
static void button_3_work_handler(struct k_work *work);
static void button_4_work_handler(struct k_work *work);

// Button work initializations
K_WORK_DEFINE(button_0_work, button_0_work_handler);
K_WORK_DEFINE(button_1_work, button_1_work_handler);
K_WORK_DEFINE(button_2_work, button_2_work_handler);
K_WORK_DEFINE(button_3_work, button_3_work_handler);
K_WORK_DEFINE(button_4_work, button_4_work_handler);


// Button interrupt handlers
//
void button_0_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 0 pressed\n");
	k_work_submit(&button_0_work);
}

void button_1_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 1 pressed\n");
	k_work_submit(&button_1_work);
}

void button_2_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 2 pressed\n");
	k_work_submit(&button_2_work);
}

void button_3_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button 3 pressed\n");
	k_work_submit(&button_3_work);
}

void button_4_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button_4 pressed\n");
	k_work_submit(&button_4_work);
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
void yellow_flash_task(void *, void *, void*);
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
K_THREAD_STACK_DEFINE(flash_stack, STACKSIZE);


//Struct for data about the thread for the kernel
static struct k_thread red_thread_data;
static struct k_thread yellow_thread_data;
static struct k_thread green_thread_data;
static struct k_thread flash_thread_data;


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


// Function for sending sequence to dispatcher FIFO
static void send_sequence_to_dispatcher(const char *sequence)
{
	struct data_t *buf = k_malloc(sizeof(struct data_t));

	if (buf == NULL) {
		printk("FIFO malloc failed\n");
		return;
	}

	snprintf(buf->msg,20,"%s",sequence);

	k_fifo_put(&dispatcher_fifo,buf);
}


// Button work handlers
//
static void button_0_work_handler(struct k_work *work)
{
	// Full sequence
	send_sequence_to_dispatcher("RYGYR");
}

static void button_1_work_handler(struct k_work *work)
{
	// Red
	send_sequence_to_dispatcher("R");
}

static void button_2_work_handler(struct k_work *work)
{
	// Yellow
	send_sequence_to_dispatcher("Y");
}

static void button_3_work_handler(struct k_work *work)
{
	// Green
	send_sequence_to_dispatcher("G");
}

static void button_4_work_handler(struct k_work *work)
{
	// Flash yellow
	send_sequence_to_dispatcher("F");
}
//


// UART initialization call in MAIN
int init_uart(void) {

	if (!device_is_ready(uart_dev)) {
		return 1;
	} 
	return 0;
}

// BUTTON initilization call in MAIN
int init_button(){

	
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
int init_led() {
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
	timing_init();

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

			// If character is not newline, add to UART message buffer
			if (rc != '\r' && rc != '\n') {

				if (uart_msg_cnt < sizeof(uart_msg)-1) {
					uart_msg[uart_msg_cnt] = rc;
					uart_msg_cnt++;
				} else {
					printk("UART message too long\n");
					uart_msg_cnt = 0;
					memset(uart_msg,0,sizeof(uart_msg));
				}

			// Character is newline, copy dispatcher data and put to FIFO buffer
			} else if (rc == '\r') {

				if (uart_msg_cnt > 0) {
					printk("UART msg: %s\n",uart_msg);

					// Put UART message to dispatcher FIFO
					send_sequence_to_dispatcher(uart_msg);
				}

				// Clear UART receive buffer
				uart_msg_cnt = 0;
				memset(uart_msg,0,sizeof(uart_msg));
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
		// Receive dispatcher data from fifo
		struct data_t *rec_item = k_fifo_get(&dispatcher_fifo,K_FOREVER);
		char sequence[20];
		memcpy(sequence,rec_item->msg,20);
		k_free(rec_item);

		printk("Dispatcher: %s\n",sequence);
		int cnt = 0;
		sequence_total_us = 0;

		// Go through sequence one character at a time
		while (sequence[cnt] != 0){

			if (sequence[cnt] == 'R'){
				printk("Dispatcher RED\n");

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

				//Sleep until the thread exits
				k_thread_join(&red_thread_data,K_FOREVER);
			}
			
			else if (sequence[cnt] == 'Y'){
				printk("Dispatcher YELLOW\n");

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

				//Sleep until the thread exits
				k_thread_join(&yellow_thread_data,K_FOREVER);
			}

			else if (sequence[cnt] == 'G'){
				printk("Dispatcher GREEN\n");

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

				//Sleep until the thread exits
				k_thread_join(&green_thread_data,K_FOREVER);
			}

			else if (sequence[cnt] == 'F'){
				printk("Dispatcher FLASH YELLOW\n");

				//Create a thread
				k_thread_create(&flash_thread_data,flash_stack,
								K_THREAD_STACK_SIZEOF(flash_stack),
								yellow_flash_task,
								NULL,
								NULL,
								NULL,
								PRIORITY,
								0,
								K_NO_WAIT
							);

				//Sleep until the thread exits
				k_thread_join(&flash_thread_data,K_FOREVER);
			}

			else {
				printk("Unknown input\n");
			}

			cnt ++;
		}
		printk("Sequence total: %lld\n", sequence_total_us);
	}
}


// Red led task
void red_led_task(void *, void *, void*) {

	//Start timer
	timing_start();
	timing_t red_start_time = timing_counter_get();

	printk("Thread API red_led_task started\n");

	// LED ON
	gpio_pin_set_dt(&red,1);
	printk("Red on\n");

	//SLEEP
	k_sleep(K_SECONDS(1));

	//LED OFF
	gpio_pin_set_dt(&red,0);
	printk("Red off\n");

	//Stop timer
	timing_t red_end_time = timing_counter_get();
	timing_stop();
	//Read time to nanoseconds with start and end time
	uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&red_start_time, &red_end_time));
	//In microseconds
	uint64_t timing_us = timing_ns / 1000;
	//Add to total counter
	sequence_total_us += timing_us;
	printk("Red task timing %lld\n", timing_us);
}


// Green led task
void green_led_task(void *, void *, void*) {

	timing_start();
	timing_t green_start_time = timing_counter_get();

	printk("API Green led thread started\n");

	// 1. set led on 
	gpio_pin_set_dt(&green,1);
	printk("Green on\n");

	// 2. sleep for 1 second
	k_sleep(K_SECONDS(1));

	// 3. set led off
	gpio_pin_set_dt(&green,0);
	printk("Green off\n");

	timing_t green_end_time = timing_counter_get();
	timing_stop();
	//Read time to nanoseconds with start and end time
	uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&green_start_time, &green_end_time));
	//In microseconds
	uint64_t timing_us = timing_ns / 1000;
	//Add to total counter
	sequence_total_us += timing_us;
	printk("green task timing %lld\n", timing_us);
}


// Yellow led task
void yellow_led_task(void *, void *, void*) {
	timing_start();
	timing_t yellow_start_time = timing_counter_get();

	printk("API yellow led thread started\n");

	// 1. set led on 
	gpio_pin_set_dt(&red,1);
	gpio_pin_set_dt(&green,1);
	printk("Yellow on\n");

	// 2. sleep for 1 second
	k_sleep(K_SECONDS(1));

	// 3. set led off
	gpio_pin_set_dt(&red,0);
	gpio_pin_set_dt(&green,0);
	printk("Yellow off\n");

	timing_t yellow_end_time = timing_counter_get();
	timing_stop();
	//Read time to nanoseconds with start and end time
	uint64_t timing_ns = timing_cycles_to_ns(timing_cycles_get(&yellow_start_time, &yellow_end_time));
	//In microseconds
	uint64_t timing_us = timing_ns / 1000;
	//Add to total counter
	sequence_total_us += timing_us;
	printk("yellow task timing %lld\n", timing_us);
}


// Yellow flashing task
void yellow_flash_task(void *, void *, void*) {
	printk("API yellow flashing thread started\n");

	for (int i = 0; i < 3; i++) {

		// Yellow on
		gpio_pin_set_dt(&red,1);
		gpio_pin_set_dt(&green,1);
		printk("Yellow on\n");

		k_sleep(K_MSEC(500));

		// Yellow off
		gpio_pin_set_dt(&red,0);
		gpio_pin_set_dt(&green,0);
		printk("Yellow off\n");

		if (i < 2) {
			k_sleep(K_MSEC(500));
		}
	}
}