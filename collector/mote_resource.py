from coapthon.server.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.client.helperclient import HelperClient
from coapthon.messages.request import Request
from coapthon.messages.response import Response
from coapthon import defines
import json
import time
import server
import threading
from database import Database
import tabulate
import logging


class MoteResource:

    def __init__(self,source_address):
        self.db = Database()
        self.connection = self.db.connect_db()
        self.actuator_resource = "LEDs"
        self.address = source_address
        self.resource = "aqi"
        self.aqi = 0
        self.ts = 0
        self.start_observing()

    def execute_query(self):
        with self.connection.cursor() as cursor:
            # Create a new record
            sql = "INSERT INTO `coap` (`aqi`, `timestamp`) VALUES (%s, %s)"
            cursor.execute(sql, (self.aqi, self.ts))
        
        self.connection.commit()
        self.show_log()

    def observer(self, response):
        print("callback called")
        if response.payload is None:
            print("response is none")
        if response.payload is not None:
            print("response:")
            print(response.payload)
            node_data = json.loads(response.payload)
            if not node_data["aqi"]:
                print("empty")
                return
            self.aqi = int(node_data["aqi"])
            self.ts = int(node_data["timestamp"])
            if self.aqi < 51:
                response = self.client.post(self.actuator_resource, "mode=good")
            elif 50 < self.aqi < 101:
                response = self.client.post(self.actuator_resource, "mode=moderate")
            elif self.aqi > 100:
                response = self.client.post(self.actuator_resource, "mode=unhealthy")
            self.execute_query()

    def show_log(self):

        with self.connection.cursor() as cursor:
            sql = "SELECT * FROM `coap`"
            cursor.execute(sql)
            results = cursor.fetchall()
            header = results[0].keys()
            rows = [x.values() for x in results]
            print(tabulate.tabulate(rows, header, tablefmt='grid'))

    def start_observing(self):
        logging.getLogger("coapthon.server.coap").setLevel(logging.WARNING)
        logging.getLogger("coapthon.layers.messagelayer").setLevel(logging.WARNING)
        logging.getLogger("coapthon.client.coap").setLevel(logging.WARNING)
        self.client = HelperClient(self.address)
        self.client.observe(self.resource, self.observer)
