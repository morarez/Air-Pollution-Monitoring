#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"
#include <stdio.h>
#include "coap-observe.h"
#include "../define_aqi.h"


//API FUNCTION DEFINITIONS
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);


//RESOURCE DEFINITION
EVENT_RESOURCE(res_aqi,
	"title=\"AQI Sensor\";obs;rt=\"AQI\"",
	res_get_handler,
	NULL,
	NULL,
	NULL,
	res_event_handler);


//API FUNCTIONS IMPLEMENTATIONS
static void res_event_handler(void){
	coap_notify_observers(&res_aqi);
}


static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	
	unsigned int accept = -1;
	coap_get_header_accept(request, &accept);

	if (accept == -1) {
    		coap_set_header_content_format(response, APPLICATION_JSON);
		snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"aqi\": %d, \"timestamp\": %lu}", aqi_value, clock_seconds());
		coap_set_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
	}

	else if (accept == APPLICATION_JSON){

		coap_set_header_content_format(response, APPLICATION_JSON);
		snprintf((char *)buffer, COAP_MAX_CHUNK_SIZE, "{\"aqi\": %d, \"timestamp\": %lu}", aqi_value, clock_seconds());
		coap_set_payload(response, buffer, strlen((char *)buffer));
	}
	else {
		coap_set_status_code(response, NOT_ACCEPTABLE_4_06);
		const char *msg = "Supporting content-type application/json";
   		 coap_set_payload(response, msg, strlen(msg));
	}
}


