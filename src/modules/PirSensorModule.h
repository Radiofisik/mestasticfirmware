#pragma once
#include "ProtobufModule.h"
#include "../mesh/generated/meshtastic/pir.pb.h"

class PirSensorModule : public ProtobufModule<meshtastic_PirData>, private concurrency::OSThread
{
  public:
    PirSensorModule()
        : ProtobufModule("pir", meshtastic_PortNum_PIR_SENSOR_APP, &meshtastic_PirData_msg), OSThread("PirSensorModule")
    {
    }

  protected:
    virtual int32_t runOnce() override;

  private:
    bool firstTime = true;
    bool lastState = true;
    bool moveDetected = false;
    uint32_t lastSentToMesh = 0;
    void sendDetectionMessage();
    void sendNoDetectionMessage();
    bool hasDetectionEvent();
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_PirData *pptr);
};

extern PirSensorModule *pirSensorModule;