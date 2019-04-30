Project #1 IPK
==========================

    Date:   09.02.2019  
    Author: Jakub Senčák
    Login:  xsenca00
    Email:  xsenca00@stud.fit.vutbr.cz    

### Content
* 1.0 Introduction   
* 2.0 Requirements  
* 3.0 Description  
    * 3.1 Usage  
    * 3.2 Output  
* 4.0 Sources  

#### 1.0 Introduction

This project was made as homework for class Computer Communications and Networks at Brno University of Technology.

[logo]: https://www.fit.vutbr.cz/images/fitnewen.png 

----------------------

#### 2.0 Requirements

It is necessary to have these libraries installed: 

```
    import sys      # processes arguments and terminating application 
    import socket   # establishes connection sending and recveiving data
    import json     # parsess received data
```

----------------------

#### 3.0 Description

##### 3.1 Usage

Expected usage:

    make run make run api_key=<string> city=<name>
    
It is necessary to register at https://home.openweathermap.org/users/sign_up where you will obtain API key, which is used in ```api_key``` argument.  
Parameter ```city``` should contain name of city you would like to look up. It should be a string. Simple one word names like Prague don't need ```""``` but it is necessary to use them when your input consists of multiple words.

Example:

    make run make run api_key=5126a7a6c126b9950d8bef4522e7ae93 city="New York"

##### 3.2 Output

Program returns information about the city you gave as a parameter.

Example:
```
Brno, CZ
few clouds
temperature:     3.79 °C
humidity:        75
pressure:        1015 hPa
wind - speed:    3.1 km/h
wind - degrees:  170 deg.
```

Writing API key is the biggest issue for the user. When inputting wrong key the API returns error message. My program detects it and returns an error message to standard output. 

##### 3.3 Communication

This application is a TCP client. It establishes TCP connection with server using methods from the ```socket``` library.  
```python
socket.socket()              # creates a socket
socket.connect((host, port)) # where port is HTTP equal to 80  
socket.sendall(buff)         # sends all that is given
socket.recv(size)            # receives data - it is necessary to decode them
socket.close()               # closes socket
```  

If I want to get information from API I had to make a GET request. This is how it looks:
```"GET " + url + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n"```  
Because everything written in Python is UTF-8 it is necessary to encode the date that arrived.  
```str.encode()``` 

----------------------

#### 4.0 Sources

https://tools.ietf.org/html/rfc7231  
https://openweathermap.org/current

https://www.tutorialspoint.com/python3/python_networking.htm  
https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.23  
https://docs.python.org/3/library/socket.html  
https://wiki.python.org/moin/TcpCommunication

All sources were active at 9. 2. 2019 

----------------------

> Enjoy :)