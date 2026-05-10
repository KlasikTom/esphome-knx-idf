#include "esp_platform.h"

#ifdef ARDUINO_ARCH_ESP8266
#include <user_interface.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "bits.h"

#ifndef KNX_SERIAL
    #define KNX_SERIAL Serial
#endif

KnxEspPlatform::KnxEspPlatform()
#ifndef KNX_NO_DEFAULT_UART
    : ArduinoPlatform(&KNX_SERIAL)
#endif
{
}

KnxEspPlatform::KnxEspPlatform( HardwareSerial* s) : ArduinoPlatform(s)
{
}

uint32_t KnxEspPlatform::currentIpAddress()
{
    return WiFi.localIP();
}

uint32_t KnxEspPlatform::currentSubnetMask()
{
    return WiFi.subnetMask();
}

uint32_t KnxEspPlatform::currentDefaultGateway()
{
    return WiFi.gatewayIP();
}

void KnxEspPlatform::macAddress(uint8_t* addr)
{
    wifi_get_macaddr(STATION_IF, addr);
}

uint32_t KnxEspPlatform::uniqueSerialNumber()
{
    return ESP.getChipId();
}

void KnxEspPlatform::restart()
{
    println("restart");
    ESP.reset();
}

void KnxEspPlatform::setupMultiCast(uint32_t addr, uint16_t port)
{
    _multicastAddr = htonl(addr);
    _multicastPort = port;
    IPAddress mcastaddr(_multicastAddr);

    KNX_DEBUG_SERIAL.printf("setup multicast addr: %s port: %d ip: %s\n", mcastaddr.toString().c_str(), port,
                            WiFi.localIP().toString().c_str());
    uint8 result = _udp.beginMulticast(WiFi.localIP(), mcastaddr, port);
    KNX_DEBUG_SERIAL.printf("multicast setup result %d\n", result);
}

void KnxEspPlatform::closeMultiCast()
{
    _udp.stop();
}

bool KnxEspPlatform::sendBytesMultiCast(uint8_t* buffer, uint16_t len)
{
    //printHex("<- ",buffer, len);
    _udp.beginPacketMulticast(_multicastAddr, _multicastPort, WiFi.localIP());
    _udp.write(buffer, len);
    _udp.endPacket();
    return true;
}

int KnxEspPlatform::readBytesMultiCast(uint8_t* buffer, uint16_t maxLen)
{
    int len = _udp.parsePacket();

    if (len == 0)
        return 0;

    if (len > maxLen)
    {
        KNX_DEBUG_SERIAL.printf("udp buffer to small. was %d, needed %d\n", maxLen, len);
        fatalError();
    }

    _udp.read(buffer, len);
    //printHex("-> ", buffer, len);
    return len;
}

bool KnxEspPlatform::sendBytesUniCast(uint32_t addr, uint16_t port, uint8_t* buffer, uint16_t len)
{
    IPAddress ucastaddr(htonl(addr));
    println("sendBytesUniCast endPacket fail");

    if (_udp.beginPacket(ucastaddr, port) == 1)
    {
        _udp.write(buffer, len);

        if (_udp.endPacket() == 0)
            println("sendBytesUniCast endPacket fail");
    }
    else
        println("sendBytesUniCast beginPacket fail");

    return true;
}

uint8_t* KnxEspPlatform::getEepromBuffer(uint32_t size)
{
    uint8_t* eepromptr = EEPROM.getDataPtr();

    if (eepromptr == nullptr)
    {
        EEPROM.begin(size);
        eepromptr = EEPROM.getDataPtr();
    }

    return eepromptr;
}

void KnxEspPlatform::commitToEeprom()
{
    EEPROM.commit();
}
#endif
