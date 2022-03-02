import paho.mqtt.client as mqtt
import json
from database import Database
import tabulate


class MQTTClient:

    def on_connect(self, client, userdata, flags, rc):
        print("connected with code: " + str(rc))
        self.client.subscribe("aqi-info")

    def on_message(self,client,userdata,msg):
        print("msg topic: " + str(msg.payload))
        data = json.loads(msg.payload)
        node_id = data["node"]
        aqi = data["aqi"]
        if aqi < 51:
            self.client.publish("led","good")
        elif aqi > 50 and aqi < 101:
            self.client.publish("led","moderate")
        elif aqi > 100:
            self.client.publish("led","bad")
        timestamp = data["timestamp"]
        with self.connection.cursor() as cursor:
            sql = "INSERT INTO `mqtt` (`node_id`, `aqi` , `timestamp`) VALUES (%s, %s, %s)"
            cursor.execute(sql, (node_id, aqi, timestamp))

        self.connection.commit()

        with self.connection.cursor() as new_cursor:
            sql = "SELECT * FROM `mqtt`"
            new_cursor.execute(sql)
            results = new_cursor.fetchall()
            header = results[0].keys()
            rows = [x.values() for x in results]
            print(tabulate.tabulate(rows, header, tablefmt='grid'))

    def mqtt_client(self):
        self.db = Database()
        self.connection = self.db.connect_db()
        print("Mqtt client starting")
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        try:
            self.client.connect("127.0.0.1", 1883, 60)
        except Exception as e:
            print(str(e))
        self.client.loop_forever()
