import http.client
import urllib.parse
import time
import serial
from twilio.rest import Client

account_sid = "AC41cda92ea4fc2f76e5253b2f265dbe98"
auth_token = "650b7f37f490358a9c8cb74a51ffa98f"
client = Client(account_sid, auth_token)
ser = serial.Serial('/dev/ttyACM0', 115200)
z = 0

def loop():
    x = ser.readline()
    y = ser.readline()
    u = ser.readline()

    def a():
        params = urllib.parse.urlencode({'field5': x, 'key': 'OHB9RP2NDJI4BO2Q'})
        headers = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
        conn = http.client.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response = conn.getresponse()
            print(x)
            data = response.read()
            conn.close()
        except:
            print("connection failed")

    a()

    def b():
        params = urllib.parse.urlencode({'field6': y, 'key': 'OHB9RP2NDJI4BO2Q'})
        headers = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
        conn = http.client.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response = conn.getresponse()
            print(y)
            data = response.read()
            conn.close()
        except:
            print("connection failed")

    b()

    def c():
        params = urllib.parse.urlencode({'field7': u, 'key': 'OHB9RP2NDJI4BO2Q'})
        headers = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
        conn = http.client.HTTPConnection("api.thingspeak.com:80")
        try:
            conn.request("POST", "/update", params, headers)
            response = conn.getresponse()
            print(u)
            data = response.read()
            conn.close()
        except:
            print("connection failed")

    c()

if __name__ == "__main__":
    while True:
        loop()
        time.sleep(1)
