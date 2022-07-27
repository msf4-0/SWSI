# Smart Weighing Scale &amp; Inventory
A wireless & portable smart weighing scale embedded system that interfaces with Node-Red for IoT connectivity and ERPNext for inventory & stock tracking.

The embedded system is developed on PlatformIO IDE and built on the Arduino framework.

<img src= "https://user-images.githubusercontent.com/76152507/164983018-ce873b46-68f4-4928-9d6d-3f753c45c488.png" width="500" height="400" />

## **Installation** ## 
1. This application is supported in both Windows and Ubuntu 20.04 
2. This application have been tested on vultr cloud platform, other cloud platforms have not been tested.

  - Install Node.js at https://nodejs.org/en/ 
  - Install Node-RED at https://nodered.org/docs/getting-started/local
  - Install MySQL on windows at https://dev.mysql.com/downloads/installer/ 
  - Install MySQL on Ubuntu 20.04 using the terminal
      1. Typed in ```sudo apt update``` 
      2. Install mySQL server package using the prompt ```sudo apt install mysql-server``` 
      3. Ensure that the server is running using the prompt ```sudo systemctl start mysql.service```
      4. Run the security script ```sudo mysql_secure_installation```, the first prompt will ask the user to validate the password. While the second prompt will be used for the MySQL root user.
      5. You can check the condition of the services by using the command prompt ```systemctl status mysql.service```
  - Install Influxdb at https://portal.influxdata.com/downloads/
  - Install ERPNext using vultr (A cloud platform) at https://www.vultr.com/

## **Basic User Guide** ##

There are several process that you should be aware of before running the flow;
2. In Microsoft Visual Studio, when trying to build and/or upload the code. In main.cpp, change Smart Weighing Scale configuration - Visual Studio Code - WIFI SSID, password
3. ERPNext token, secret
4. Node-red modules
5. influxdb location1 and location2 (in nodered)
6. mysql database node in nodered

## **Detailed Documentation** ##
Follow the [documentation](https://drive.google.com/drive/folders/1bG_YV6uIwTKT_12CuCw8axnyWVp6YvOv?usp=sharing) process for further detailed explaination.
