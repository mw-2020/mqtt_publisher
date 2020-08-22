/* 
* Name: mqtt_publisher.c
* Author: M. Wiehl
* Created on: 22.08.2020
* 
* Description: Simple client for a MQTT sensor node with dummy sensor values
* 
* Compile with gcc mqtt_pub.c -o mqtt_pub -l mosquitto
*/

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <mosquitto.h>

#define MQTT_HOST "192.168.43.6"
#define MQTT_PORT 1883
#define MQTT_NAME "Milling Machine 1"

static int run = 1;

void handle_signal(int s)
{
	run = 0;
}

int main(int argc, char *argv[])
{
	// initialize client
	struct mosquitto *mosq;
	int rc = 0;

	char mqtt_msg[10];
	int temperature = 0;

	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	mosquitto_lib_init();

	mosq = mosquitto_new(MQTT_NAME, true, NULL);

	if(mosq == 0)
	{
		printf("Error when creating instance. Error Code: %d\n", rc);
	}

	// main thread
	while(run)
	{
		// connect to broker
   	    	rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 5);
				
		// connection failed
		if(rc != MOSQ_ERR_SUCCESS)
		{
			printf("Client could not connect to brocker. Trying again. Error Code: %d\n", rc);
			sleep(1);
		}
		
		// connection succeded		
		if(rc == MOSQ_ERR_SUCCESS)
		{
			printf("Connection to broker successful\n");
		}

		// in latter case: transmit data
		while((rc == MOSQ_ERR_SUCCESS) && (run == 1))
		{
			// check connection to broker, timeout 1000 ms (default)
			rc = mosquitto_loop(mosq, -1, 1);
			
			if(rc == MOSQ_ERR_SUCCESS)
			{
				// Here we should call a sensor readout function
				// But for now we create a random value and convert to a message string
				// sample code for dummy values: temperature = rand() % 5 + 45;
				temperature = (temperature + 1) % 100;
				sprintf(mqtt_msg, "%d C", temperature);
				
				// send message to broker
				printf("Send message: %s\n",mqtt_msg);
				mosquitto_publish(mosq, NULL, "temperature", strlen(mqtt_msg), mqtt_msg, 0, 0);
			}
			else
			{
				printf("Connection to broker lost\n");
			}
			
			// wait a few seconds
			sleep(2);
		}

	}
	
	return rc;
}
