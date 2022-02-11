from server import *
from coapthon.server.coap import CoAP
import threading
from mqtt_collector import MQTTClient
import logging

# Unspecified IPv6 address
ip = "::"
port = 5683


class CoAPServer(CoAP):
    def __init__(self, host, port):
        CoAP.__init__(self, (host, port), False)
        self.add_resource("registry", AdvancedResource())

def test():
    logging.getLogger("coapthon.server.coap").setLevel(logging.WARNING)
    logging.getLogger("coapthon.layers.messagelayer").setLevel(logging.WARNING)
    logging.getLogger("coapthon.client.coap").setLevel(logging.WARNING)

    client = MQTTClient()
    thread = threading.Thread(target=client.mqtt_client, args=(), kwargs={})
    thread.start()
    server = CoAPServer(ip, port)
    try:
        server.listen(100)
    except KeyboardInterrupt:
        thread.join()
        server.close()
        print("SHUTDOWN")


if __name__ == '__main__':
    test()
