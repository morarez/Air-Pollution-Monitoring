#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "sys/etimer.h"
#include "coap-blocking-api.h"
#include "random.h"
#include "node-id.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP


#define SERVER "coap://[fd00::1]:5683"


static struct etimer periodic_timer;
bool registered = false;
static int period = 0;

void client_chunk_handler(coap_message_t *response)
{
	const uint8_t *chunk;

	if(response == NULL) {
		LOG_INFO("Request timed out");
		return;
	}
	registered = true;
	int len = coap_get_payload(response, &chunk);
	LOG_INFO("|%.*s \n", len, (char *)chunk);
}

PROCESS(node, "node");
AUTOSTART_PROCESSES(&node);

int aqi_value = 50;

extern coap_resource_t res_leds;
extern coap_resource_t res_aqi;


PROCESS_THREAD(node, ev, data)
{
  srand(time(NULL));
  static coap_endpoint_t my_server;
  static coap_message_t request[1];


  PROCESS_BEGIN();

  PROCESS_PAUSE();

  LOG_INFO("Starting sensor node\n");

  coap_activate_resource(&res_leds, "LEDs");
  coap_activate_resource(&res_aqi, "AQI");

  coap_endpoint_parse(SERVER, strlen(SERVER), &my_server);

  coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
  coap_set_header_uri_path(request, "registry");

  LOG_DBG("Registering with server\n");
  COAP_BLOCKING_REQUEST(&my_server, request, client_chunk_handler);

  while(!registered){

    LOG_DBG("Retrying with server\n");
    COAP_BLOCKING_REQUEST(&my_server, request, client_chunk_handler);

  }

  etimer_set(&periodic_timer, 30*CLOCK_SECOND);
  
  while(1) {
    PROCESS_WAIT_EVENT();

      if (ev == PROCESS_EVENT_TIMER && data == &periodic_timer){
	  aqi_value = rand() % 400
	  res_aqi.trigger();
	  if(period%5==0) {
		    LOG_DBG("Retrying/Pinging the server\n");
	  	    COAP_BLOCKING_REQUEST(&my_server, request, client_chunk_handler);
	  }
	  period++;
	  etimer_reset(&periodic_timer);
      }
    }

  PROCESS_END();
}