#include <Arduino.h>
#include <Bluepad32.h>
#include <Servo.h>

Servo ESC1;
Servo ESC2;
Servo ESC3;
Servo ESC4;
Servo ESC5;
Servo ESC6;

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

int lXaxis = 0;
int lYaxis = 0;
int rXaxis = 0;
int rYaxis = 0;

void setup() {
  Serial.begin(9600);
  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();

   ESC1.attach(2,1000,2000);
   ESC2.attach(3,1000,2000);
   ESC3.attach(5,1000,2000);
   ESC4.attach(6,1000,2000);
   ESC5.attach(9,1000,2000);
   ESC6.attach(10,1000,2000);

}

void loop() {
  BP32.update();
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr myController = myControllers[i];

    if (myController && myController->isConnected()) {
      if (myController->isGamepad()){
        processGamepad(myController);
      }
      
    }
  }
}

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.print("CALLBACK: Controller is connected, index=");
      Serial.println(i);
      myControllers[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      char buf[80];
      sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
      Serial.println(buf);
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
        "CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.print("CALLBACK: Controller is disconnected from index=");
      Serial.println(i);
      myControllers[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void processGamepad(ControllerPtr gamepad) {

   lXaxis =  gamepad->axisX();
   lYaxis =  gamepad->axisY();
   rXaxis =  gamepad->axisRX();
   rYaxis =  gamepad->axisRY();
  

  char buf[160];
  snprintf(
      buf, sizeof(buf) - 1,
      "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4li, %4li, axis "
      "R: %4li, %4li, brake: %4ld, throttle: %4li, misc: 0x%02x, battery: %d",
      gamepad->index(),        // Gamepad Index
      gamepad->dpad(),         // DPAD
      gamepad->buttons(),      // bitmask of pressed buttons
      gamepad->axisX(),        // (-511 - 512) left X Axis
      gamepad->axisY(),        // (-511 - 512) left Y axis
      gamepad->axisRX(),       // (-511 - 512) right X axis
      gamepad->axisRY(),       // (-511 - 512) right Y axis
      gamepad->brake(),        // (0 - 1023): brake button
      gamepad->throttle(),     // (0 - 1023): throttle (AKA gas) button
      gamepad->miscButtons(),  // bitmak of pressed "misc" buttons
      gamepad->battery()       // 0=Unknown, 1=empty, 255=full
  );
  Serial.println(buf);

  // You can query the axis and other properties as well. See
  // Controller.h For all the available functions.
}