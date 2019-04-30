#!/usr/bin/python3

"""This is simple weather program, that uses OpenWeatherMap API"""

import sys
import socket
import json

FAILURE = 1
SUCCESS = 0
HTTP = 80


def print_weather(data):
    """Prints weather information"""

    print(data["name"] + ", " + data["sys"]["country"])
    print(data["weather"][0]["description"])
    print("temperature:\t", data["main"]["temp"], "°C")
    print("humidity:\t", data["main"]["humidity"])
    print("pressure:\t", data["main"]["pressure"], "hPa")
    print("wind - speed:\t", data["wind"]["speed"], "km/h")
    print("wind - degrees:\t", data["wind"]["deg"], "deg.")


def main():
    """Does what main() does"""

    # process arguments
    if len(sys.argv) != 3 or sys.argv[2] == "":
        if sys.argv[2] == "":
            print("Missing city parameter.")

        print("Expected usage:")
        print("make run make run api_key=<string> city=<name>")
        sys.exit(FAILURE)

    api_key = sys.argv[1]
    city = sys.argv[2]

    # Initialize networking
    host = "api.openweathermap.org"
    url = '/data/2.5/weather?q=' + city + '&units=metric' +'&appid=' + api_key
    # print(host+url)
    port = HTTP

    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    soc.connect((host, port))

    request = "GET " + url + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n"
    request_bytes = str.encode(request)
    soc.sendall(request_bytes)

    receive_data = soc.recv(4096)

    soc.close()

    data = receive_data.decode('utf-8')
    split_data = data.split("\r\n\r\n")
    json_data = json.loads(split_data[1])
    # User-friendly output :)
    # print("data", json_data["cod"]) # used to show HTTP error type
    if json_data["cod"] != 200:
        print("Error:", json_data["message"])
        sys.exit(SUCCESS) #FAIL????

    print_weather(json_data)
    return


if __name__ == "__main__":
    main()
