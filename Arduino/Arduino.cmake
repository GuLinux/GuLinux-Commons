set(CACHE_DIR ${CMAKE_BINARY_DIR}/cache)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/output)

file(MAKE_DIRECTORY ${CACHE_DIR})
set(HOME $ENV{HOME})
set(GDB_EXECUTABLE gdb-multiarch CACHE STRING "GDB Executable for embedded architecture (default: gdb-multiarch)")


set(BOARD "${DEFAULT_BOARD}" CACHE STRING "Board type (currently supported: `nano`, `bluepill, stm32::bluepill, mapleMini`)")
set(ARDUINO_HOME $ENV{ARDUINO_HOME})

if("${ARDUINO_HOME}" STREQUAL "")
    message(FATAL_ERROR "Please set the ARDUINO_HOME environment variable")
endif()

set(DEFAULT_LEAFLAB_TTY /dev/serial/by-id/usb-LeafLabs_Maple-if00)
if(${BOARD} STREQUAL "bluepill")
    set(UPLOAD_METHOD DFUUploadMethod CACHE STRING "Options available: DFUUploadMethod, BMPMethod")
    set(DEVICE_FQBN Arduino_STM32:STM32F1:genericSTM32F103C:device_variant=STM32F103CB,upload_method=${UPLOAD_METHOD},cpu_speed=speed_72mhz,opt=osstd)
    set(TTY {${DEFAULT_LEAFLAB_TTY} CACHE STRING "Default tty port for picocom (${DEFAULT_LEAFLAB_TTY})")
    set(BOARD_EXTRA_OPTS -vid-pid=0X1EAF_0X0004)
elseif(${BOARD} STREQUAL "stm32::bluepill")
    set(UPLOAD_METHOD dfu2Method CACHE STRING "Options available: dfu2Method, bmpMethod")
    set(CPU_OPT oslto CACHE STRING "CPU Optimization (osstd, oslto, ogstd, etc)")
    set(XSERIAL none CACHE STRING "XSerial (generic, none, disabled)")
    set(USB CDCgen CACHE STRING "USB support (none, CDCgen, CDC, HID)")
    set(DEVICE_FQBN STM32:stm32:GenF1:pnum=BLUEPILL_F103C8B,upload_method=${UPLOAD_METHOD},xserial=${XSERIAL},usb=${USB},xusb=FS,opt=${CPU_OPT},rtlib=nano)
    set(TTY ${DEFAULT_LEAFLAB_TTY} CACHE STRING "Default tty port for picocom (${DEFAULT_LEAFLAB_TTY})")
    set(BOARD_EXTRA_OPTS -vid-pid=0X1EAF_0X0004)
elseif(${BOARD} STREQUAL "nano")
    set(TTY /dev/ttyUSB0 CACHE STRING "Default tty port for picocom (ttyUSB0)")
    set(DEVICE_CPU atmega328p)
    set(DEVICE_FQBN arduino:avr:nano:cpu=atmega328)
    set(ARDUINO_UPLOAD_BAUD 57600)
elseif(${BOARD} STREQUAL "mapleMini")
    set(DEVICE_FQBN Arduino_STM32:STM32F1:mapleMini:bootloader_version=original,cpu_speed=speed_72mhz,opt=osstd)
    set(TTY ${DEFAULT_LEAFLAB_TTY} CACHE STRING "Default tty port for picocom (${DEFAULT_LEAFLAB_TTY})")
else()
    message(FATAL_ERROR "Board ${BOARD} not supported. Please set one of the supported boards using the -DBOARD=`board-name` parameter")
endif()

set(TTY_BAUD 9600 CACHE STRING "Baud rate for picocom (9600)")
set(TTY_IMAP lfcrlf CACHE STRING "picocom tty imap (default: lfcrlf)")
set(TTY_OMAP crcrlf CACHE STRING "picocom tty omap (default: crcrlf)")

add_custom_target(
    ${PROJECT_NAME}
    ALL
        mkdir -p "${BUILD_DIR}" "${CACHE_DIR}" && 
        ${ARDUINO_HOME}/arduino-builder 
        -compile
#        -logger=machine
        -hardware ${ARDUINO_HOME}/hardware
        -hardware ${HOME}/.arduino15/packages
        -hardware ${HOME}/Arduino/hardware
        -tools ${ARDUINO_HOME}/tools-builder
        -tools ${HOME}/.arduino15/packages
        -libraries ${HOME}/Arduino/libraries
        -libraries ${CMAKE_SOURCE_DIR}/libraries
        -libraries ${ARDUINO_HOME}/libraries
        -fqbn=${DEVICE_FQBN}
        -ide-version=10808
        -build-path ${BUILD_DIR}
        -build-cache ${CACHE_DIR}
        -warnings=default
        -prefs=build.warn_data_percentage=75
        -verbose
        ${BOARD_EXTRA_OPTS}
        ${COMPILER_EXTRA_OPTS}
        ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.ino
)

add_custom_target(upload_maple
    ${HOME}/Arduino/hardware/Arduino_STM32/tools/linux/maple_upload ${TTY} 2 1EAF:0003 ${BUILD_DIR}/${PROJECT_NAME}.ino.bin
    DEPENDS ${PROJECT_NAME}
)

add_custom_target(upload_arduino
    /bin/avrdude -C/etc/avrdude.conf -v -p${DEVICE_CPU} -carduino -P/dev/${TTY} -b${ARDUINO_UPLOAD_BAUD} -D -Uflash:w:${BUILD_DIR}/${PROJECT_NAME}.ino.hex:i
    DEPENDS ${PROJECT_NAME}
)
add_custom_target(tty picocom ${TTY} --baud ${TTY_BAUD} --imap ${TTY_IMAP} --omap ${TTY_OMAP} --echo USES_TERMINAL)
add_custom_target(bmp_gdb
    ${GDB_EXECUTABLE}
    -ex "target extended-remote /dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe__STLINK____Firmware_v1.6.1-269-g7cb1858__C1DEA6F9-if00"
    -ex "monitor swdp_scan"
    -ex "attach 1"
    -ex "file ${BUILD_DIR}/${PROJECT_NAME}.ino.elf"
    USES_TERMINAL
)
