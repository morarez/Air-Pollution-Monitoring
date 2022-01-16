#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coap-engine.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler(void);

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
EVENT_RESOURCE(res_aqi,
         "title=\"AQI \";obs;rt=\"AQI\"",
         res_get_handler,
         NULL,
         NULL,
         NULL, 
	res_event_handler);


static void
res_event_handler(void)
{
    // Notify all the observers
    coap_notify_observers(&res_aqi);
}


static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
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
