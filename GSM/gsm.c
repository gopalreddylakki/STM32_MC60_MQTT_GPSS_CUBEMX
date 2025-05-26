/*
 * gsm.c
 *
 *  Created on: Dec 13, 2025
 *      Author:GOPALREDDY LAKKIREDDY
 *
 * 
 */

#include "gsm.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "../Systick/systick.h"
#include "string.h"
#include "main.h"
#include "stdlib.h"
#include <stdio.h>

#define MAX_READ_DATA 1024

enum  { Idle, MessageReceived, SMSReceived };

char gsm_data_buffer[MAX_READ_DATA] = { 0 };
volatile uint16_t read_data_buffer  = 0;
volatile uint16_t state             = Idle;
volatile uint32_t capture_mask      = 0;

//tcpObject tcpConnectionObject;
//tcpObject secondaryConnectionObject;
TCP_IP_STATE tcpIpConnectionState;

uint8_t phoneNumber[14] = "";
uint8_t smsMessage[170];
extern uint8_t imeii[17];

uint8_t tcp_domain[30] = {"sytiqhub.in"};
uint8_t porttcp={80};

uint8_t mqtt_domain[30] = {".............."};
uint8_t mqtt_username[30] = {".............."};
uint8_t mqtt_password[30] = {".............."};
uint8_t mqtt_pubtopic[30] = {".............."};
uint8_t mqtt_subtopic[30] = {".............."};
int con=0;

GPS_t GPS;


void gsmSplitString(uint8_t *charArray, char delimiter, int length, uint8_t *position)
{
	int counter,pos;
	pos = 0;
	for(counter=0;counter<length;counter++){
		if (*charArray == delimiter) position[pos++] = counter;
		charArray++;
	}
}

void gsmSplitStringWithDelims(uint8_t *charArray, char delimiter_1, char delimiter_2, int length, uint8_t *position)
{
    int counter = 0; 
    int pos = 0;
    uint8_t accum = 0;
    for(; counter < length ;)
    {
        if((*charArray == delimiter_2) && (accum == delimiter_1))
        {
            position[pos++] = counter;
        }
        counter++;
        accum = *charArray;
        charArray ++;
    }
}


void gsmSplitStringWithDelims1(uint8_t *charArray, char delimiter_1, int length, uint8_t *position)
{
    int counter = 0;
    int pos = 0;
    uint8_t accum = 0;
    for(; counter < length ;)
    {
        if( (accum == delimiter_1))
        {
            position[pos++] = counter-1;
        }
        counter++;
        accum = *charArray;
        charArray ++;
    }
}
void gsmCopyString(uint8_t *src, uint8_t *dst, int maximum, int start_pos, int end_pos)
{
    int counter;
    src +=start_pos;
    for(counter = start_pos; counter < end_pos ;counter++)
    {
        *dst = *src;
        src++;
        dst++;
        if((counter - start_pos) > maximum)
        {
            return;
        }
    }
}

uint32_t gsmParseInt(uint8_t *charArray, int start_pos, int end_pos)
{
    char string_value[11] = { 0 };
    int counter,index = 0;
    char *end;

    charArray += start_pos;
    for(counter = start_pos; counter < end_pos; counter++)
    {
        string_value[index] = *charArray++;
        index++;
    }
    string_value[index] = 0;
    index++;

    return strtol(string_value, &end, 10);
}

uint8_t initializeSIMModule( GSMModuleValue *gsmvalue  )
{
    static int counter = 0;
    if (sendATCommand("AT", sizeof("AT"), "OK", 300) == 0)
    {
        counter++;
        if (counter > 20)
        {
            serialPrint("\nUnable to initialize GSM \r\n");
            serialPrint("\nStarting device \r\n");
            startSIMModule(GSM_POWER_PORT, GSM_POWER_PIN, 3000);
            return 0;
        }
    }
    serialPrint("\nSIM Initialized successfully\r\n");
    if (sendATCommand("ATE1", sizeof("ATE1"), "OK\r\n", 20))
    {
        serialPrint("\nNo Echo \r\n");
    }

    if (sendATCommand("ATI", sizeof("ATI"), "OK\r\n", 20))
    {
        serialPrint("\nNo Echo \r\n");
    }
    if (sendATCommand("AT+CPIN?", sizeof("AT+CPIN?"), "+CPIN: READY\r\n", 20))
    {
        serialPrint("\n  +CPIN: READY -- \r\n");
    }
    //AT+CMEE=2
    if (sendATCommand("AT+CMEE=2", sizeof("AT+CMEE=2"), "OK\r\n", 20))
    {
        serialPrint("\n   enable result code and use verbose values -- \r\n");
    }
    //AT+QGNSSC=1
    if (sendATCommand("AT+QGNSSC?", sizeof("AT+QGNSSC?"), "AT+QGNSSC=0\r\n", 20))
    {
        serialPrint("\n  GPS_POWER_OFF -- \r\n");
    }
    //AT+GSN -IMEI /* Use AT+GSN to query the IMEI of module */
    if (sendATCommand("AT+GSN", sizeof("AT+GSN"), "OK\r\n", 20))
      {
    	uint8_t delimiter[20];
    	uint8_t accum[17];
    	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
    	gsmCopyString((uint8_t *)gsm_data_buffer,accum, 15, (delimiter[0])+1, (delimiter[0])+16);
    	serialPrint((char *)accum);
    	memcpy(imeii, accum, sizeof(accum));

    	// memcpy(imeii, accum, 15 * 15);
      }
    ///* Use AT+QCCID to query ICCID number of SIM card */
    if (sendATCommand("AT+QCCID", sizeof("AT+QCCID"), "OK\r\n", 20))
      {
    	uint8_t delimiter[20];
    	uint8_t accum[21];
    	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
    	gsmCopyString((uint8_t *)gsm_data_buffer,accum, 21, (delimiter[0])+1, (delimiter[0])+21);
    	serialPrint((char *)accum);
    	gsmvalue->ICCID=accum;
      }
    //GPS POWER
			if (sendATCommand("AT+QGNSSC=1", sizeof("AT+QGNSSC=1"), "OK\r\n", 20))
			{
				serialPrint("\n  GPS_POWER_ON -- \r\n");
			}
			if (sendATCommand("AT+QGNSSC=1", sizeof("AT+QGNSSC=1"), "OK\r\n", 20))
			{
				serialPrint("\n  GPS_POWER_ON -- \r\n");
			}
//    if (sendATCommand("AT+CMGF=1", sizeof("AT+CMGF=1"), "OK\r\n", 20))
//    {
//        serialPrint("\nText mode message format\r\n");
//    }
//    if (sendATCommand("AT&W", sizeof("AT&W"), "OK\r\n", 20))
//    {
//        serialPrint("\nSettings saved\r\n");
//    }
//    if (sendATCommand("AT+GMR", sizeof("AT+GMR"), "OK\r\n", 20))
//    {
//        serialPrint("\nTA identified\r\n");
//    }
    state = Idle;
    return 1;
}

uint8_t gsmKeepAlive()
{
    if (sendATCommand("AT \r", sizeof("AT \r"), "OK", 30))
    {
        command_failed_count = 0;
        if (!checkSIMNetworkState())
        {
            sim_disconnected++;
            int net_state = checkSIMNetworkState();
            serialPrint("\nNetwork state: %i \r\n", net_state);
        }
        else
        {
            sim_disconnected = 0;
        }
        if (sim_disconnected > 20)
        {
            gsmModuleState = On;
            sim_disconnected = 0;
            return 0;
        }
    }else {
        command_failed_count++;
        if (command_failed_count > 10)
        {
            serialPrint("\nCan not communicate with module \r\n");
            gsmModuleState = Off;
            command_failed_count = 0;
            return 0;
        }
    }
    return 1;
}

uint8_t checkSIMNetworkState()
{//|| (sendATCommand("AT+CREG?",sizeof("AT+CREG?"), "+CREG: 0,5", 20))
    if ((sendATCommand("AT+CREG?",sizeof("AT+CREG?"),"+CREG: 0,1", 20)) )
    {
        serialPrint("\nRegistered to network\r\n");
        return 1;
    }
    return 0;
}

uint8_t sendATCommand(void *command, int data_size, char *response, int timeout)
{
    char control_character = '\r';
    char *control_character_ptr = &control_character;
    read_data_buffer = 0;
    memset(&gsm_data_buffer[0], 0, MAX_READ_DATA);
    UART2Send((uint8_t *)command, (data_size - 1));
    UART2Send((uint8_t *)control_character_ptr, 1);
    state = Idle;
    gsmTick = 0;
    while (gsmTick < timeout)
    {
        while (UART2Probe())
        {
            if (read_data_buffer < MAX_READ_DATA)
            {
                gsm_data_buffer[read_data_buffer++] =  UART2GetChar();
            }
            else
            {
                read_data_buffer = 0;
            }
            state = MessageReceived;
            HAL_Delay(10);
        }
        if (state == MessageReceived)
        {
            state = Idle;
            if (strstr(gsm_data_buffer, response) != NULL)
            {
                read_data_buffer = 0;
                return 1;
            }
        }
    }
    read_data_buffer = 0;
    return 0;
}

uint8_t sendATCommand1(void *command, int data_size, char *response, int timeout,int pos)
{
    char control_character = '\r';
    char *control_character_ptr = &control_character;
    read_data_buffer = 0;
    memset(&gsm_data_buffer[0], 0, MAX_READ_DATA);
    UART2Send((uint8_t *)command, (data_size - 1));
    UART2Send((uint8_t *)control_character_ptr, 1);
    state = Idle;
    gsmTick = 0;
    while (gsmTick < timeout)
    {
        while (UART2Probe())
        {
            if (read_data_buffer < MAX_READ_DATA)
            {
                gsm_data_buffer[read_data_buffer++] =  UART2GetChar();
            }
            else
            {
                read_data_buffer = 0;
            }
            state = MessageReceived;
            HAL_Delay(10);
        }
        if (state == MessageReceived)
        {
            state = Idle;
            if (strstr(gsm_data_buffer, response) != NULL)
            {
                read_data_buffer = 0;
                return 1;
            }
        }
    }
    read_data_buffer = 0;
    return 0;
}


uint8_t getATCommandReply(char *response, int timeout)
{
    read_data_buffer = 0;
    memset(&gsm_data_buffer[0], 0, MAX_READ_DATA);
    state = Idle;
    gsmTick = 0;
    while (gsmTick < timeout)
    {
        while (UART2Probe())
        {
            gsm_data_buffer[read_data_buffer++] =  UART2GetChar();
            state = MessageReceived;
            //HAL_Delay(10);
        }
        if (state == MessageReceived)
        {
            state = Idle;
            if (strstr(gsm_data_buffer, response) != NULL)
            {
                read_data_buffer = 0;
                return 1;
            }
        }
    }
    read_data_buffer = 0;
    return 0;
}


uint8_t getATCommandReply1(char *response, int timeout)
{
    read_data_buffer = 0;
    memset(&gsm_data_buffer[0], 0, MAX_READ_DATA);
    state = Idle;
    gsmTick = 0;
    while (gsmTick < timeout)
    {
        while (UART2Probe())
        {
            gsm_data_buffer[read_data_buffer++] =  UART2GetChar();
            state = MessageReceived;
            //HAL_Delay(10);
        }
        if (state == MessageReceived)
        {
            state = Idle;
            if (strstr(gsm_data_buffer, response) != NULL)
            {
                read_data_buffer = 0;
                return 1;
            }
        }
    }
    read_data_buffer = 0;
    return 0;
}

void resetSIMModule(GPIO_TypeDef *pin_peripheral, uint16_t gsm_pin, int duration)
{

   // HAL_GPIO_WritePin(pin_peripheral, gsm_pin, GPIO_PIN_RESET);
    HAL_Delay(duration);
    HAL_GPIO_WritePin(pin_peripheral,gsm_pin,GPIO_PIN_SET);
    serialPrint("\nGSM Module Restarted \r\n");
}

void startSIMModule(GPIO_TypeDef *pin_peripheral, uint16_t gsm_pin, int duration)
{

    HAL_GPIO_WritePin(pin_peripheral, gsm_pin, GPIO_PIN_SET);
    HAL_Delay(duration);
    HAL_GPIO_WritePin(pin_peripheral,gsm_pin,GPIO_PIN_RESET);
    serialPrint("\nGSM Module Started \r\n");
}

uint8_t setupTCP(GSMModuleValue *gsmvalue)
{

    char data_bucket[64];
    uint8_t length;
    if (checkSIMNetworkState())
    {

        if (sendATCommand("AT+CGATT?", sizeof("AT+CGATT?"), "+CGATT: 0\r\n", 20))
        {

        if (sendATCommand("AT+QIMODE=0", sizeof("AT+QIMODE=0"), "OK\r\n", 20))
        {
            serialPrint("\r\nManual data fetch enabled\r\n");
        }
        if (sendATCommand("AT+QIMUX=1", sizeof("AT+QIMUX=1"), "OK\r\n", 20))
              {
                  serialPrint("\r\nManual data fetch enabled\r\n");
              }
        length = snprintf(data_bucket, 64, "AT+QICSGP=1,\"%s\",\"%s\",\"%s\"\r", apn_netw, apn_user, apn_pass);
        if (sendATCommand(data_bucket, length, "OK\r\n", 300))
        {
           serialPrint("\r\nAPN1 OK\r\n");
        }

        length = snprintf(data_bucket, 64, "AT+QIREGAPP=\"%s\",\"%s\",\"%s\"\r", apn_netw, apn_user, apn_pass);
        if (sendATCommand(data_bucket, length, "OK\r\n", 300))
        {
           serialPrint("\r\nAPN2 OK\r\n");
        }
   ///AT+CGATT?


				if(sendATCommand("AT+QISRVC=1", sizeof("AT+QISRVC=1"), "OK\r\n", 3000))
				{
					serialPrint("\r\nAttached to network\r\n");

					if (sendATCommand("AT+QIACT", sizeof("AT+QIACT"), "OK\r\n", 7000))
					{
						serialPrint("\r\nWireless connection successful\r\n");

					}
					else
					{
						serialPrint("\r\nWireless connection failed\r\n");
					}


				}
       }
                if (sendATCommand("AT+QILOCIP", sizeof("AT+QILOCIP"), "ERROR", 3000))
                 {
                     serialPrint("\r\nUnable to get local IP \r\n");
                     return 0;
                 } else {
                     serialPrint("\r\nLocal IP issued\r\n");
                     uint8_t delimiter[20];
                     uint8_t accum[17];
                     gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
                     gsmCopyString((uint8_t *)gsm_data_buffer,accum, 17, (delimiter[0]+1), (delimiter[1]-1));
                     serialPrint((char *)accum);
                     //gsmvalue->IP=accum;
                     return 1;
                 }

    }
    return 0;
}


//uint8_t setupTCPGET();

uint8_t setupTCPGET(GSMModuleValue *gsmvalue, char * url)
{
    char data_bucket[1024];
    char data_bucket1[20];
    uint8_t length,length1;
    if (checkSIMNetworkState())
    {
        if (sendATCommand("AT+QICLOSE=0", sizeof("AT+QICLOSE=0"), "OK\r\n", 20))
        {
            serialPrint("\r\n AT+QICLOSE=0 CLOSEd\r\n");
        }
        //AT+QIDNSIP=1
        if (sendATCommand("AT+QIDNSIP=1", sizeof("AT+QIDNSIP=1"), "OK\r\n", 20))
        {
            serialPrint("\r\n AT+QIDNSIP=1   \r\n");
        }
        length = snprintf(data_bucket, 64, "AT+QIOPEN=0,\"TCP\",\"%s\",%d\r", tcp_domain, 80);
        if (sendATCommand(data_bucket, length, "OK\r\n", 300))
        {
           serialPrint("\r\n Connected to TCP OK\r\n");

           if (getATCommandReply("0, CONNECT OK\r\n", 5000))
           {
               serialPrint("\nSent data\r\n");
               //return 1;
        //0, CONNECT OK
        length = snprintf(data_bucket, 1024, "GET %s HTTP/1.1 \r\nHost:%s:%d\r\nConnection: Keep-Alive \r\n\r\n%d", url, tcp_domain, 80,26);
        length1 = snprintf(data_bucket1, 30, "AT+QISEND=0,%d\r\n", length);
			if (sendATCommand(data_bucket1, length1, "\r\n", 20))
			{
						if (sendATCommand(data_bucket, length, "OK\r\n", 200))
						{


							serialPrint("\nDATA REVICE\r\n");
						}

						serialPrint("\nDATA REVICE\r\n");




			}
           }
           else
           {
             serialPrint("\nUnable to send data\r\n");
           }
        }




    }

    return 0;
}


////mqtt connect

uint8_t setupMQTT()
{

	  uint8_t length;
	  uint8_t imq[16]="862430056368828";
	  uint8_t delimiter[20];
	  uint8_t uu=0;
	  char data_bucket[512];
	    if (checkSIMNetworkState())
	    {
	    	//AT+QMTCLOSE=0
	    	// if (sendATCommand("AT+QMTCLOSE=0\r\n", sizeof("AT+QMTCLOSE=0\r\n"), "OK\r\n", 7000))
	    	//            {
	    	length = snprintf(data_bucket, 64, "AT+QMTOPEN=0,\"%s\",%d\r\n", mqtt_domain, 1883);
	        if (sendATCommand(data_bucket, length, "+QMTOPEN: 0,", 8000))
	        	{
	        	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
	        	uu=gsm_data_buffer[delimiter[3]-2];
	        	 serialPrint("opening mqtt:%d",uu);
	        	 if(uu==48 || uu==50)
	        	 {
	        		 serialPrint("\r\n Connected to OPEN MqttOK\r\n");
	        		 length = snprintf(data_bucket, 64, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", imq,mqtt_username,mqtt_password);
	        		 if (sendATCommand(data_bucket, length, "+QMTCONN:", 5000))
	        		 {
	     	        	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
	     	        	uu=gsm_data_buffer[delimiter[3]-2];
	     	        	 serialPrint("Connect cline mqtt:%d",uu);
	        		 }

	        		 length = snprintf(data_bucket, 64, "AT+QMTSUB=0,1,\"%s%s\",0\r\n", mqtt_subtopic, imq);
	        		 if (sendATCommand(data_bucket, length, "+QMTSUB: 0,1,0", 5000))
	        			  {
	        			     	        	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
	        			     	        	uu=gsm_data_buffer[delimiter[3]-2];
	        			     	        	serialPrint("SUBtopic is : %d",uu);
	        			   }




	        	 }
	        	 else
	        	 {
	        		 serialPrint("\r\n Connected to OPEN NOT MqttOK\r\n");
	        	 }

	        	}
	        else
	        {
	        	serialPrint("\r\n Connected toNot MqttOK\r\n");
	        }




	    }


return 1;
}


uint8_t mqtt_pubmsg( char * top,char * msg,int len)
{
	 uint8_t length;
 	 uint8_t uu=0;
	 char data_bucket[200];
	 uint8_t delimiter[10];

	length = snprintf(data_bucket, 64, "AT+QMTPUB=0,0,0,0,\"%s\",%d\r\n", top, len);
	if (sendATCommand(data_bucket, length, "\r\n", 1000))
		        {
					if (sendATCommand(msg, len, "+QMTPUB: 0", 5000))
			        	{
			        	gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
			        	uu=gsm_data_buffer[delimiter[3]-2];
			        	serialPrint("Pub is : %d",uu);
			        	if(uu==48)return 1;
			        	else return 0;
			        	}


		        }
	return 0;

}


uint8_t mqtt_check()
{
	        //UART2clear();
	        if (UART2Probe()!=0)
	        {


            	memset(&gsm_data_buffer, 0, MAX_READ_DATA);
            	read_data_buffer = 0;
                while (UART2Probe())
                {
                    if (read_data_buffer < MAX_READ_DATA)
                    {
                        gsm_data_buffer[read_data_buffer++] =  UART2GetChar();
                    }
                    else
                    {
                        read_data_buffer = 0;
                    }
                    HAL_Delay(2);
                }

                if (strstr(gsm_data_buffer, "+QMTRECV") != NULL)
                            {

                       	uint8_t delimiter[10];
                       	uint8_t delimiter1[10];
                       	uint8_t Subtop[50];
                       	uint8_t Subdata[50];
                	    gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
                	    gsmSplitStringWithDelims1((uint8_t *)gsm_data_buffer, ',', MAX_READ_DATA, delimiter1);
                	    gsmCopyString((uint8_t *)gsm_data_buffer,Subtop, 30, (delimiter1[1]+1), (delimiter1[2]));
                	    gsmCopyString((uint8_t *)gsm_data_buffer,Subdata, 50, (delimiter1[2]+1), (delimiter[1]));
                		serialPrint("\r\n  Subtopic : %s data: %s \r\n",(char *)Subtop,(char *)Subdata);
                         return 1;
                            }

	       }
	return 0;
}

uint8_t Gpscheck()
{
//AT+QGNSSRD?
    if (sendATCommand("AT+QGNSSRD?",sizeof("AT+QGNSSRD?"),  "OK", 500))
    {

       	//uint8_t delimiter[10];
       	uint8_t delimiter1[10];
       	uint8_t GNGGA[80];
       	//uint8_t GNRMC[100];
       	//uint8_t GNGLL[100];
       	//uint8_t GNVTG[100];


       //gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
        gsmSplitStringWithDelims1((uint8_t *)gsm_data_buffer, '$', MAX_READ_DATA, delimiter1);
        //gsmCopyString((uint8_t *)gsm_data_buffer,GNRMC, 100, (delimiter1[0]), (delimiter1[1]-1));
        //gsmCopyString((uint8_t *)gsm_data_buffer,GNVTG, 100, (delimiter1[1]), (delimiter1[2]-1));
        gsmCopyString((uint8_t *)gsm_data_buffer,GNGGA, 100, (delimiter1[2]), (delimiter1[3]-1));
        //gsmCopyString((uint8_t *)gsm_data_buffer,GNGLL, 100, (delimiter1[6]), (delimiter1[6]+80));
       // gsmCopyString((uint8_t *)gsm_data_buffer,Subdata, 50, (delimiter1[2]+1), (delimiter[1]));
        //serialPrint("\r\n  Subtopic : %s data: %s \r\n",(char *)Subtop,(char *)Subdata);

        return GPS_parse(GNGGA);
    }
   return 0;
}

uint8_t writeToTCPSocket()
{
    serialPrint("\nSending data \r\n");
    if (sendATCommand("AT+CIPSEND=0,7",sizeof("AT+CIPSEND=0,7"),  ">", 300))
    {
        serialPrint("\n..\r\n");
        UART2Send((uint8_t *)"HELLO\r\n",7);
        if (getATCommandReply("SEND", 5000))
        {
            serialPrint("\nSent data\r\n");
            return 1;
        }else{
            serialPrint("\nUnable to send data\r\n");
        }

    }
    return 0;
}

uint8_t  GPS_parse(char *GPSstrParse){

    if(!strncmp(GPSstrParse, "$GNGGA", 6)){
    	if (sscanf(GPSstrParse, "$GNGGA,%f,%f,%c,%f,%c,%d,%d,%f,%f,%c", &GPS.utc_time, &GPS.nmea_latitude, &GPS.ns, &GPS.nmea_longitude, &GPS.ew, &GPS.lock, &GPS.satelites, &GPS.hdop, &GPS.msl_altitude, &GPS.msl_units) >= 1){
    		GPS.dec_latitude = GPS_nmea_to_dec(GPS.nmea_latitude, GPS.ns);
    		GPS.dec_longitude = GPS_nmea_to_dec(GPS.nmea_longitude, GPS.ew);
    		return 1;
    	}
    }
    return 0;
//    else if (!strncmp(GPSstrParse, "$GPRMC", 100)){
//    	if(sscanf(GPSstrParse, "$GPRMC,%f,%f,%c,%f,%c,%f,%f,%d", &GPS.utc_time, &GPS.nmea_latitude, &GPS.ns, &GPS.nmea_longitude, &GPS.ew, &GPS.speed_k, &GPS.course_d, &GPS.date) >= 1)
//    		return;
//
//    }
//    else if (!strncmp(GPSstrParse, "$GPGLL", 01000)){
//        if(sscanf(GPSstrParse, "$GPGLL,%f,%c,%f,%c,%f,%c", &GPS.nmea_latitude, &GPS.ns, &GPS.nmea_longitude, &GPS.ew, &GPS.utc_time, &GPS.gll_status) >= 1)
//            return;
//    }
//    else if (!strncmp(GPSstrParse, "$GPVTG", 6)){
//        if(sscanf(GPSstrParse, "$GPVTG,%f,%c,%f,%c,%f,%c,%f,%c", &GPS.course_t, &GPS.course_t_unit, &GPS.course_m, &GPS.course_m_unit, &GPS.speed_k, &GPS.speed_k_unit, &GPS.speed_km, &GPS.speed_km_unit) >= 1)
//            return;
//    }
}
 void GPS_PRINT()
 {
	 serialPrint("\ndec_latitude:%f longitude: %f .kn.lkn, \r\n",GPS.dec_latitude,GPS.dec_longitude);
 }


float GPS_nmea_to_dec(float deg_coord, char nsew) {
    int degree = (int)(deg_coord/100);
    float minutes = deg_coord - degree*100;
    float dec_deg = minutes / 60;
    float decimal = degree + dec_deg;
    if (nsew == 'S' || nsew == 'W') { // return negative
        decimal *= -1;
    }
    return decimal;
}


//uint8_t getTCPStatus(int timeout)
//{
//    uint8_t delimiter[20];
//    uint8_t position[20];
//    uint8_t accum_max   = 32;
//    uint8_t accum[accum_max];
//    char *data;
//
//    read_data_buffer = 0;
//    memset(&gsm_data_buffer[0], 0, MAX_READ_DATA);
//    UART2Send((uint8_t *)"AT+QISTATE=0,0\r\n", sizeof("AT+QISTATE=0,0\r\n"));
//    state = Idle;
//    gsmTick = 0;
//    while (gsmTick < timeout)
//    {
//        while (UART2Probe())
//        {
//            if (read_data_buffer < MAX_READ_DATA)
//            {
//                gsm_data_buffer[read_data_buffer++] = UART2GetChar();
//            } else {
//                read_data_buffer = 0;
//            }
//            state = MessageReceived;
//            HAL_Delay(10);
//        }
//        if(state == MessageReceived)
//        {
//            gsmSplitStringWithDelims((uint8_t *)gsm_data_buffer, '\r', '\n', MAX_READ_DATA, delimiter);
//            gsmCopyString((uint8_t *)gsm_data_buffer,accum, accum_max, (delimiter[2]), (delimiter[3]));
//            if (strstr((char*)accum, "IP INITIAL"))
//            {
//                tcpIpConnectionState = IP_INITIAL;
//            }else if (strstr((char*)accum, "IP START"))
//            {
//                tcpIpConnectionState = IP_START;
//            }else if (strstr((char*)accum, "IP CONFIG"))
//            {
//                tcpIpConnectionState = IP_CONFIG;
//            }else if (strstr((char*)accum, "IP GPRSACT"))
//            {
//                tcpIpConnectionState = IP_GPRSACT;
//            }else if (strstr((char*)accum, "IP STATUS"))
//            {
//                tcpIpConnectionState = IP_STATUS;
//            }else if (strstr((char*)accum, "IP PROCESSING"))
//            {
//                tcpIpConnectionState = IP_PROCESSING;
//            }else if (strstr((char*)accum, "PDP DEACT"))
//            {
//                tcpIpConnectionState = PDP_DEACT;
//            }
//
//            data = &gsm_data_buffer[delimiter[4]];
//            gsmSplitString((uint8_t *)data, ',', (delimiter[5] - delimiter[4]), position);
//            gsmCopyString((uint8_t *)gsm_data_buffer, accum,  accum_max, (delimiter[4] + delimiter[4]), (delimiter[5]));
//            if (strstr((char*)accum, "INITIAL"))
//            {
//                tcpConnectionObject.state = INITIAL;
//            }else if (strstr((char*)accum, "CONNECTING"))
//            {
//                tcpConnectionObject.state = CONNECTING;
//            }else if (strstr((char*)accum, "CONNECTED"))
//            {
//                tcpConnectionObject.state = CONNECTED;
//            }else if (strstr((char*)accum, "REMOTE CLOSING"))
//            {
//                tcpConnectionObject.state = REMOTE_CLOSING;
//            }else if (strstr((char*)accum, "CLOSING"))
//            {
//                tcpConnectionObject.state = CLOSING;
//            }else if (strstr((char*)accum, "CLOSED"))
//            {
//                tcpConnectionObject.state = CLOSED;
//            }
//
//            state = Idle;
//        }
//    }
//    read_data_buffer = 0;
//    return 0;
//}

// TODO: Limitation... SIM Pin can not be set
// TODO: SMS ... Add SMS functionalities
