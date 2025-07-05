#include "wled.h"

class UARTPresetNotifier : public Usermod {
private:
  String lastCommand = "";
  unsigned long lastSendTime = 0;
  String queuedCommand = "";
  bool waitingForDelay = false;

public:
  void onPresetChanged(uint8_t presetId) override {
    String newCommand = getCommandFromPreset(presetId);

    if (newCommand != lastCommand) {
      if (lastCommand != "" && lastCommand != "rainbow" && newCommand != "rainbow") {
        Serial.println("stop_" + lastCommand);
        lastSendTime = millis();
        queuedCommand = newCommand;
        waitingForDelay = true;
      } else if (lastCommand != "rainbow" && newCommand == "rainbow") {
        Serial.println("stop_" + lastCommand);
        lastSendTime = millis();
        lastCommand = newCommand;
      } else if (lastCommand == "rainbow" && newCommand != "rainbow") {
        Serial.println(newCommand);
        lastSendTime = millis();
        lastCommand = newCommand;
      } else {
        Serial.println(newCommand);
        lastSendTime = millis();
        lastCommand = newCommand;
      }
    }
  }

  void loop() override {
    if (waitingForDelay && millis() - lastSendTime >= 2000) {
      Serial.println(queuedCommand);
      lastSendTime = millis();
      lastCommand = queuedCommand;
      queuedCommand = "";
      waitingForDelay = false;
    }
  }

  void sendDirectUARTCommand(String cmd) {
    if (millis() - lastSendTime >= 2000) {
      Serial.println(cmd);
      lastSendTime = millis();
    }
  }

  String getCommandFromPreset(uint8_t presetId) {
    switch (presetId) {
      case 1: return "links";
      case 2: return "rechts";
      case 3: return "voor";
      case 4: return "achter";
      case 5: return "onder";
      case 6: return "boven";
      case 7: return "rainbow";
      default: return "";
    }
  }

  void addToConfig(JsonObject &root) override {}
  bool readFromConfig(JsonObject &root) override { return true; }
};

UARTPresetNotifier uartPresetNotifier;

void sendCustomUARTCommand(String cmd) {
  uartPresetNotifier.sendDirectUARTCommand(cmd);
}

void registerUsermod(Usermod *usermod) {
  usermod = &uartPresetNotifier;
}