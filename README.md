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
1. Within Microsoft Visual Studio, when trying to build and/or upload the code. In main.cpp (within the src folder), change the **!!!WIFI_ID!!!** and **!!!WIFI_PASSWORD!!!** according to your own network credentials. While the **mqtt_server_IP_address** change it according to your own IPv4 address.
```
// WiFI & MQTT
const char* id = "gdWS1"; // Unique Scale ID
const char* ssid = "!!!WIFI_ID!!!";
const char* password = "!!!WIFI_PASSWORD!!!";
const char* mqtt_server = "192.168.0.164"
```

2. In ERPNext homepage; 

i. Underneath "My settings"

<img src= "https://user-images.githubusercontent.com/75675418/181437964-1305fefa-9fe0-460c-a80b-50efa9cf91e8.png" width="216" height="401" />

ii. You will able to find API Access that could "Generate API Key", likes so;


<img src= "https://user-images.githubusercontent.com/75675418/181438828-71cf2b39-27a0-4867-ba44-2961529f2f58.JPG" width="631" height="181" />

iii. Once generated, you can copy both the API Keys and the API Secret onto the Node-RED flow.

iv. By using change nodes and an inject node, you can make it so that the API Keys of the ERPNext to be global.

<img src= "https://user-images.githubusercontent.com/75675418/181440183-c336749e-6c41-46c6-9153-c10c1662c997.JPG" width="420" height="84" />

Inside the ERPNext domain node;

<img src= "https://user-images.githubusercontent.com/75675418/181440187-be28c1f1-1201-4d98-b21d-c54c6879d2ce.JPG" width="508" height="337" />

Inside the ERPNext token node;

<img src= "https://user-images.githubusercontent.com/75675418/181440177-a95ac427-3fde-4dbc-be94-2812a2074ef3.JPG" width="505" height="410" />

V. Once this is done, your ERPNext should be connected to the Node-RED

3. Before importing the Node-RED flow, you need to install some Node-RED modules;
  - node-red-contrib-influxdb
  - node-red-contrib-aedes
  - node-red-node-mysql
  - node-red-contrib-bcrypt
  - node-red-dashboard

4. To ensure that the data of the scale is being properly read, you need to ensure that influxdb is properly run. To do this, make a database called **location1** and **location2**.

<img src= "https://user-images.githubusercontent.com/75675418/181443483-63936997-03f6-49a8-a352-1fcdb2d0510f.JPG" width="618" height="160" />

Once done, proceed with inserting measurement onto our databases by using the command ```insert gdWS1/main value=0.1``` for scale 1 and ```insert gdWS2/main value=0.1``` for scale 2. Following is the successful configuration of both databases and measurements within scale 1;

<img src="https://user-images.githubusercontent.com/75675418/181444866-f88cdf0c-40d9-453d-8abe-6db3ee4f6944.JPG" width="263" height="156" />

Lastly, to see the value that is being read by using the command ```select * from “[Your Database Name]” order by time desc limit 5```;

<img src= "https://user-images.githubusercontent.com/75675418/181445328-999dcf15-3ba3-4d75-94b8-e85251dc8541.JPG" width="497" height="185" />

5. For mysql, there is a node in node-RED that users should be aware of when running the flow, it is the mysql node that looks like;

<img src= "https://user-images.githubusercontent.com/75675418/181446719-ce58e390-7abe-49db-9655-445a063ad76b.JPG" width="157" height="59" />

Upon importing the flow, the node should also be configured accordingly. In our case, we set it as;

<img src="https://user-images.githubusercontent.com/75675418/181447253-5d53da33-16b2-4470-9892-e79daaa72347.JPG" width="500" height="229" />

and

<img src= "https://user-images.githubusercontent.com/75675418/181447262-c13bf9b6-bc79-47a0-8db7-eac54bb34c71.JPG" width="511" height="577" />


## **Detailed Documentation** ##
Follow the [documentation](https://drive.google.com/drive/folders/1bG_YV6uIwTKT_12CuCw8axnyWVp6YvOv?usp=sharing) process for further detailed explaination.
