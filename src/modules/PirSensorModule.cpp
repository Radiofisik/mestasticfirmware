#ifdef PIR_SENDER
#include "PirSensorModule.h"
#include "Default.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "configuration.h"
#include "main.h"

PirSensorModule *pirSensorModule;

#define GPIO_POLLING_INTERVAL 100
#define DELAYED_INTERVAL 1000

#define PIR_PIN 25
#define DEBOUNCE 60

int32_t PirSensorModule::runOnce()
{
   
    // return disable();
    if (firstTime) {
        LOG_DEBUG("PIR init\n");
        // This is the first time the OSThread library has called this function, so do some setup
        firstTime = false;
        pinMode(PIR_PIN, INPUT);
        return DELAYED_INTERVAL;
    }

    if ((millis() - lastSentToMesh) >= Default::getConfiguredOrDefaultMs(DEBOUNCE)){
        if(hasDetectionEvent()) {
            moveDetected = true;
            sendDetectionMessage();
            return DELAYED_INTERVAL;
        }
        else if(moveDetected){
            sendNoDetectionMessage();
            moveDetected = false;
        }
    }
    
    return GPIO_POLLING_INTERVAL;
}

void PirSensorModule::sendDetectionMessage()
{
    LOG_DEBUG("PIR Detected event observed. Sending message\n");
    meshtastic_PirData m;
    m.value = true;
    meshtastic_MeshPacket *p = allocDataProtobuf(m);
    p->to = NODENUM_BROADCAST;
    p->decoded.want_response = false;
    p->want_ack = false;
    // p->priority = meshtastic_MeshPacket_Priority_RELIABLE;
    // p->which_payload_variant = meshtastic_MeshPacket_decoded_tag;
    // LOG_INFO("PIR Sending message id=%d, dest=%x, msg=%.*s\n", p->id, p->to, p->decoded.payload.size, p->decoded.payload.bytes);
    lastSentToMesh = millis();
    service->sendToMesh(p);
}

void PirSensorModule::sendNoDetectionMessage()
{
    LOG_DEBUG("PIR unetected event observed. Sending message\n");
    meshtastic_PirData m;
    m.value = false;
    meshtastic_MeshPacket *p = allocDataProtobuf(m);
    p->to = NODENUM_BROADCAST;
    p->decoded.want_response = false;
    p->want_ack = false;
    service->sendToMesh(p);
}

bool PirSensorModule::hasDetectionEvent()
{
    bool currentState = digitalRead(PIR_PIN);
    bool result = currentState != lastState;
    lastState = currentState;
    if(result){
        LOG_DEBUG("PIR Sensor Module: Current state: %i\n", currentState);
    }
    return result;
}

bool PirSensorModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_PirData *pptr)
{
    // auto p = *pptr;
    LOG_DEBUG("PIR did handleReceived\n");
    return false; // Let others look at this message also if they want
}
#endif // PIR_SENDER