// Dit is een aangepaste WLED usermod die zorgt dat bij presetwijzigingen een UART commando wordt gestuurd
// met minimaal 2000ms tussen elk bericht – ook tussen 'stop_xxx' en het nieuwe commando
// en niets doen bij preset 7 of als je NAAR preset 7 gaat

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
      if (lastCommand != "" && newCommand != "7") {
        Serial.println("stop_" + lastCommand);
        lastSendTime = millis();
        queuedCommand = newCommand != "7" ? newCommand : "";
        waitingForDelay = true;
      } else if (newCommand != "7") {
        Serial.println(newCommand);
        lastSendTime = millis();
        lastCommand = newCommand;
      }
      if (newCommand == "7") {
        lastCommand = "";
        queuedCommand = "";
        waitingForDelay = false;
      }
    }
  }

  void loop() override {
    if (waitingForDelay && millis() - lastSendTime >= 2000) {
      if (queuedCommand != "") {
        Serial.println(queuedCommand);
        lastSendTime = millis();
        lastCommand = queuedCommand;
        queuedCommand = "";
      }
      waitingForDelay = false;
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
      case 7: return "7";  // special rainbow sync preset
      default: return "";
    }
  }

  void addToConfig(JsonObject &root) override {}
  bool readFromConfig(JsonObject &root) override { return true; }
};

UARTPresetNotifier uartPresetNotifier;

void registerUsermod(Usermod *usermod) {
  usermod = &uartPresetNotifier;
}
