#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "MT3620_Grove_Shield_Library/Grove.h"
#include "MT3620_Grove_Shield_Library/Sensors/Grove4DigitDisplay.h"

#include "MT3620_Grove_Shield_Library/Sensors/GroveLightSensor.h"
#include "MT3620_Grove_Shield_Library/Sensors/GroveAD7992.h"

#include "MT3620_Grove_Shield_Library/Sensors/GroveTempHumiSHT31.h"

int main(void)
{
    // This minimal Azure Sphere app repeatedly toggles GPIO 9, which is the green channel of RGB
    // LED 1 on the MT3620 RDB.
    // If your device exposes different GPIOs, you might need to change this value. For example,
    // to run the app on a Seeed mini-dev kit, change the GPIO from 9 to 7 in the call to
    // GPIO_OpenAsOutput and in the app_manifest.json to blink its LED. Check with your hardware
    // manufacturer to determine which GPIOs are available.
    // Use this app to test that device and SDK installation succeeded that you can build,
    // deploy, and debug an app with Visual Studio, and that you can deploy an app over the air,
    // per the instructions here: https://docs.microsoft.com/azure-sphere/quickstarts/qs-overview
    //
    // It is NOT recommended to use this as a starting point for developing apps; instead use
    // the extensible samples here: https://github.com/Azure/azure-sphere-samples
    Log_Debug(
        "\nVisit https://github.com/Azure/azure-sphere-samples for extensible samples to use as a "
        "starting point for full applications.\n");

    // Change this GPIO number and the number in app_manifest.json if required by your hardware.
    int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fd < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return -1;
    }

    int fd1 = GPIO_OpenAsOutput(4, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    if (fd1 < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return -1;
    }

    int i2cFd;
    GroveShield_Initialize(&i2cFd, 115200);
    void* light = GroveLightSensor_Init(i2cFd, 0);
    void* sht31 = GroveTempHumiSHT31_Open(i2cFd);

    void* dev = Grove4DigitDisplay_Open(0, 1);
    Grove4DigitDisplay_DisplayClockPoint(false);

    int number = 0;

    GroveTempHumiSHT31_Read(sht31);
    float temp = GroveTempHumiSHT31_GetTemperature(sht31);
    float humi = GroveTempHumiSHT31_GetHumidity(sht31);

    Log_Debug("Temperature %f \n", temp);

    const struct timespec sleepTime = {1, 0};
    while (true) {
        GPIO_SetValue(fd, GPIO_Value_Low);
        GPIO_SetValue(fd1, GPIO_Value_High);

        usleep(1000);

        Grove4DigitDisplay_DisplayValue(dev, number++);

        GPIO_SetValue(fd, GPIO_Value_High);
        GPIO_SetValue(fd1, GPIO_Value_Low);

        usleep(1000);

        Grove4DigitDisplay_DisplayValue(dev, number++);

        float value = GroveLightSensor_Read(light);
        value = GroveAD7992_ConvertToMillisVolt(value);
        Log_Debug("Light value %dmV\n", (uint16_t)value);
        
    }
}
