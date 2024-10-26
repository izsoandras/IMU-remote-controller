# MPU-9250 based remote vehicle controller
This application is based on the Node32s development board of ESP32. It streams data over WebSocket to control a vehicle, according to [this project](https://github.com/izsoandras/Wemos-Tank-Websockets).
Rotating the unit around the Y axis (pitch) defines the longitudinal velocity and rotating around the X axis (roll) defines the angular velocity.

## Usage
1. After turning on, the device should look for the WiFi network `TigerTank`
2. Upon connecting, the control demand is streamed. 
3. For easy handling hold it horizontally with X axis pointing forward, and Y axis to the left. This way
  1. Rotating front and back controls the speed of the vehicle
  2. Rotating left and right steers the vehicle

Button functionalities are not (yet) implemented.

## Details
### About tilt estimation
Only the accelerometer of the MPU-9250 is used to determine the tilt of the sensor. Assumed reference frame orientation:

- **X**: forward
- **Y**: left
- **Z**: up

Euler angles are used to represent the tilt. This results in singularities when the Y axis is vertical. This is not handled in any way, keep this in mind on the vehicle side. The rotation order:
1. Y axis (pitch)
2. X axis (roll)

Rotation around Z axis (yaw) is not needed for vehicle control, thus it is not included in orientation estimation.

The software assumes, that the measured gravity vector is pointing down.

### Pin connection

| GPIO | Functionality |
|------|---------------|
| 21   | I2C SDA       |
| 22   | I2C SCL       |
