import getopt
import sys
import json
import threading
import time
from coapthon.server.coap import CoAP
from coapthon.resources.resource import Resource
from coapthon.client.helperclient import HelperClient
from coapthon.messages.request import Request
from coapthon.messages.response import Response
from coapthon import defines
from mote_resource import MoteResource


class AdvancedResource(Resource):
    def __init__(self, name="Advanced"):
        super(AdvancedResource, self).__init__(name)
        self.payload = "Advanced resource"

    def render_GET_advanced(self, request, response):
        if request.payload is None:
            print("empty payload")
            mr =MoteResource(request.source)
            return self, None
        else:
             moteInfo = json.loads(request.payload)
             response.payload = self.payload
             response.max_age = 20
             response.code = defines.Codes.CONTENT.number
             moteResource = MoteResource(request.source)
             return self, response
