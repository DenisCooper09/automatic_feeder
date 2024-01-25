# Automatic Feeder
Automatic feeder for cats.

## Features
- Web control panel.
- Anti-stuck system.
- ~1 litre tank for cat food.

## Future Features
- Feeding schedule.
- Feeding amount in grams.
- Worldwide control via MQTT protocol.
- Android/IOS app for controlling the feeder.

## Components
- 1x ATmega328P (arduino nano)
- 1x ESP8266
- 1x DVR8825 (driver)
- 1x NEMA17 (stepper motor)
- 1x RTC DS1307 (real time clock)
- 1x DC plug 2mm
- 2x 220Ω resistors
- 2x 3mm leds
- 1x v7805 (voltage regulator)
- 3x 470µF 16v capacitors
- 1x 10µF 6.3v capacitor
- 1x HDR-2.54 female 1x4

## Schematic
![Schematic](https://github.com/DenisCooper09/automatic_feeder/blob/main/schematic/Schematic_AutomaticFeeder_2023-12-08.svg)

## PCB
![PCB_top](https://github.com/DenisCooper09/automatic_feeder/blob/main/pcb/images/photo_view/PCB_AutomaticFeeder_Photo_View_Top.svg)
![PCB_bottom](https://github.com/DenisCooper09/automatic_feeder/blob/main/pcb/images/photo_view/PCB_AutomaticFeeder_Photo_View_Bottom.svg)

## Firmware settings

### Internet connection

```c++
#define WIFI_SSID "wifi_network_name"
#define WIFI_PASSWORD "wifi_password"
```

Replace **wifi_network_name** with your wifi network name and **wifi_password** with your password.

> [!NOTE]
> You can connect to web control panel by entering ESP's IP address into your browser.
> Here is an instruction how to find ESP's IP address:
> 1. Connect ESP to a computer via USB.
> 2. Open serial monitor.
> 3. Restart ESP and you should see IP address being printed in serial monitor.

### Motor configuration

```c
#define SM_IMPULSE_DELAY 30
#define SM_FORWARD_STEPS 100
#define SM_BACKWARD_STEPS 94
#define SM_MICRO_STEPS MICRO_STEPS_32
#define SM_DIRECTION CLOCKWISE
```

**SM_IMPULSE_DELAY** - delay in microseconds before sending signal to driver. (Speed of motor)

**SM_FORWARD_STEPS** and **SM_BACKWARD_STEPS** - these are used for anti-stuck system.
Intead of rotating all the time forward motor makes **SM_FORWARD_STEPS** steps forward and **SM_BACKWARD_STEPS** steps backward.
This makes motor vibrate (if frequency (AKA **SM_IMPULSE_DELAY**) is small enough and motor is in microstepping mode), so pieces of cat food can't be stuck. 
> [!CAUTION]
> Make sure that ***SM_FORWARD_STEPS* > *SM_BACKWARD_STEPS***

**SM_MICRO_STEPS** - number of microsteps per one full step. Enum values:

```c
enum MOTOR_MICRO_STEPS {
    FULL_STEP,
    MICRO_STEPS_2,
    MICRO_STEPS_4,
    MICRO_STEPS_8,
    MICRO_STEPS_16,
    MICRO_STEPS_32
};
```

**SM_DIRECTION** - direction of motor rotation. Enum values:

```c
enum MOTOR_DIRECTION {
    ANTI_CLOCKWISE,
    CLOCKWISE
};
```

> [!TIP]
> If motor rotates in wrong direction (screw moves cat food towards motor) then just reverse motor direction in code!

## Gallery

> [!NOTE]
> Photos were took while development so, circuit is assembled on a breadboard, not on PCB.

![Photo0](https://github.com/DenisCooper09/automatic_feeder/blob/main/images/AutomaticFeeder000.jpg)
![Photo2](https://github.com/DenisCooper09/automatic_feeder/blob/main/images/AutomaticFeeder002.jpg)
![Photo17](https://github.com/DenisCooper09/automatic_feeder/blob/main/images/AutomaticFeeder017.jpg)
