# STM32C0116-DK template project

 * Linux dev environment
 * C++ support
 * RTT debug support (set RELEASE to 0 in makefile)
 * auto RDP lock for release version
 * cube programmer supported
 * openocd patch included

## Usage

 * untar the STM32Cube to ../cube
 * untar the JLink to ../jlink and untar ../jlink/Samples/RTT/SEGGER_RTT_* to ./jlink/Samples/RTT/SEGGER_RTT
 * untar STM32CubeProgrammer to ../progcube
 * study Makefile & enjoy !

**N.B. When RDP locked MCU will hard fault with any USB access to onboard STLink so supply only power over USB in this case**
