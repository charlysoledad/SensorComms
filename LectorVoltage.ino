#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define MAX_INPUT_VALUE 1500
#define MIN_INPUT_VALUE 150

const int ADC_VALUE = 15;

// COMMS
BluetoothSerial SerialBT;


String state = "";
int range = 4;
int oldRange = 4;

int tick = 0;
int standByTime = 15000; 
bool connectedBT = false;

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
    connectedBT = true;
  }
  if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("Client disconnected");
    connectedBT = false;
  }
}

void setup(){
    Serial.begin( 115200 );

    SerialBT.register_callback(callback);

    SerialBT.begin("SENSOR-DEVICE"); // Device name

    Serial.println("Device started !");
}
/*
    * MAX 1500
    * MID MAX 1499 - 1100
    * MID 750 > 550 - 1099
    * MID MIN 151 - 549
    * MIN 150
    * 
*/
void loop(){
    float voltage_value = (float) analogRead(ADC_VALUE) ;
    Serial.println(voltage_value); // To Serial 

    if(voltage_value >= MAX_INPUT_VALUE)
    {
        state = "100%";
        range = 4;
    } else if(voltage_value >= 1100 && voltage_value < 1499)
    {
        state = "75%";
        range = 3;
    } else if(voltage_value >= 550 && voltage_value < 1100)
    {
        state = "50%";
        range = 2;
    } else if(voltage_value >= 151 && voltage_value < 550){
        state = "25%";
        range = 1;
    } else if(voltage_value <= MIN_INPUT_VALUE){
        state = "0%";
        range = 0;
    }

    if((range != oldRange)){
        if (!connectedBT){
            SerialBT.connect();
        }
        if(connectedBT){
            SerialBT.println(state);
            tick = 0;
        }
        oldRange = range;
    }
    if(tick == standByTime){
        SerialBT.disconnet();
    }else {
        tick++;
    }

    delay(100);
}
