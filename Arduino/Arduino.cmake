set(CACHE_DIR ${CMAKE_BINARY_DIR}/cache)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/output)

file(MAKE_DIRECTORY ${CACHE_DIR})
set(HOME $ENV{HOME})


set(BOARD "${DEFAULT_BOARD}" CACHE STRING "Board type (currently supported: `nano`, `bluepill, mapleMini`)")
set(ARDUINO_HOME $ENV{ARDUINO_HOME})

if("${ARDUINO_HOME}" STREQUAL "")
    message(FATAL_ERROR "Please set the ARDUINO_HOME environment variable")
endif()

if(${BOARD} STREQUAL "bluepill")
    set(DEVICE_FQBN Arduino_STM32:STM32F1:genericSTM32F103C:device_variant=STM32F103CB,upload_method=DFUUploadMethod,cpu_speed=speed_72mhz,opt=osstd)
    set(TTY ttyACM0 CACHE STRING "Default tty port for picocom (ttyACM0)")
    set(BOARD_EXTRA_OPTS -vid-pid=0X1EAF_0X0004)
elseif(${BOARD} STREQUAL "nano")
    set(TTY ttyUSB0 CACHE STRING "Default tty port for picocom (ttyUSB0)")
    set(DEVICE_CPU atmega328p)
    set(DEVICE_FQBN arduino:avr:nano:cpu=atmega328)
    set(ARDUINO_UPLOAD_BAUD 57600)
elseif(${BOARD} STREQUAL "mapleMini")
    set(DEVICE_FQBN Arduino_STM32:STM32F1:mapleMini:bootloader_version=original,cpu_speed=speed_72mhz,opt=osstd)
    set(TTY ttyACM0 CACHE STRING "Default tty port for picocom (ttyACM0)")
else()
    message(FATAL_ERROR "Board ${BOARD} not supported. Please set one of the supported boards using the -DBOARD=`board-name` parameter")
endif()

set(TTY_BAUD 9600 CACHE STRING "Baud rate for picocom (9600)")

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
add_custom_target(tty picocom /dev/${TTY} --baud ${TTY_BAUD} --echo USES_TERMINAL)
