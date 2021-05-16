//COMPILE: gcc pub_ssl.c -o pubssl -lpaho-mqtt3cs
//RUN: ./clientpubssl

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "MQTTClient.h"

#include "config.h"


/*size of the message */
//#define Size_Byte 1024

int Size_Byte=0;
//uint8_t * mymsg; 


static uint8_t *rand_string(uint8_t *str, size_t size)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (size) 
	{
	    --size;
	    for (size_t n = 0; n< size; n++)
	    {
		int key = rand() % 16;
		str[n] = key;
		
	    }
	}
	return str;
}
uint8_t * rand_string_alloc(size_t size)
{
	uint8_t *s =(uint8_t *) malloc(size);
	if(s)
	{
		rand_string(s, size);
	    
	}
	return s;
}


int main(int argc, char* argv[])
{
    
    struct timeval curTime, lastTime;
    struct timeval constart, conend;
    struct timeval transmstart, transmend;
    
    int value = 0 ;
    
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

	
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 0;
    conn_opts.ssl = &ssl_opts;
    ssl_opts.trustStore = CERT;

    gettimeofday(&lastTime, NULL);

    printf("Size\tMSG_Id\tTTconnect\tTTtransmit\n", rc);
    int sizes[] = {1024, 5120,10240,15360,20480,25600};

   for (int ind= 0; ind<6; ind++)
   {
	Size_Byte = sizes[ind]; 
	
	uint8_t * mymsg  =rand_string_alloc(Size_Byte);
	value =0; 
	
	//for (value =0 ; value<20; value++)
	//{
       while(1==1)
       {
	    
	    gettimeofday(&curTime, NULL);
	    if ( curTime.tv_sec - lastTime.tv_sec  > TM) 
	    {
		    lastTime = curTime;
		    value ++;
		    if ( !MQTTClient_isConnected(client))
		    {
			gettimeofday(&constart, NULL);
			if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
			{
			    printf("Failed to connect, return code %d\n", rc);
			    exit(-1);
			}
			gettimeofday(&conend, NULL);
			//printf("Time to connect = %d  us \n", (conend.tv_usec - constart.tv_usec)); 
			printf("%d\t%d\t%d\t",Size_Byte, value,(conend.tv_usec - constart.tv_usec)); 
			
		    }
		    
		    gettimeofday(&transmstart, NULL);
		    pubmsg.payload = mymsg;
		    pubmsg.payloadlen = Size_Byte;
		    pubmsg.qos = QOS;
		    pubmsg.retained = 0;

		    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
		    
		  // rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		    gettimeofday(&transmend, NULL);
		    
		     printf("\t%d\n", (transmend.tv_usec - transmstart.tv_usec)); 
		      MQTTClient_disconnect(client, 10000);
	
	    }
	    if(value >100)
		break; 
	   
	}
	printf("--------------------------------\n");
	free(mymsg);
    }

    MQTTClient_destroy(&client);
    return rc;
}


