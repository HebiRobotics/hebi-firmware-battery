/*
 * @file info.h
 *
 * @brief Utility macros for versioning info
 *
 * Modified from addresses.h
 *
 * Created on: June 6, 2014
 * 
 * @author Pras Velagapudi (pkv@cs)
 * @author Yigit Bilgen (mbilgen@andrew)
 * @author Florian Enner (enner@cmu)
 */

#ifndef PROJECT_INFO_H_
#define PROJECT_INFO_H_

/* Macro for unused arguments. Makes the compiler shut up. */
#define UNUSED_ARG(arg) ((void)(arg))

/* Macros for converting macro arguments to strings */
#define STRINGIFY(A) #A
#define STRINGIFY_CONTENTS(A) STRINGIFY(A)

// Query compiler for module type information
#ifndef _FIRMWARE_TYPE
#warning Firmware type was not defined!
#define _FIRMWARE_TYPE unknown
#endif

// Query compiler for firmware revision information
#ifndef _FIRMWARE_REVISION
#warning Firmware revision was not defined!
#define _FIRMWARE_REVISION unknown
#endif

// Query compiler for module mode information
#ifndef _FIRMWARE_MODE
#warning Firmware mode was not defined!
#define _FIRMWARE_MODE unknown
#endif

// Query compiler for board type information
#ifndef _BOARD_TYPE
#warning Electrical board type was not defined!
#define _BOARD_TYPE unknown
#endif

// Query compiler for electrical type information
#ifndef _ELECTRICAL_TYPE
#warning Electrical type was not defined!
#define _ELECTRICAL_TYPE unknown
#endif

/**
 * This string defines the type of the firmware for compatibility purposes
 * (e.g., what hardware it will work on).
 */
#define FIRMWARE_TYPE (STRINGIFY_CONTENTS(_FIRMWARE_TYPE))

/**
 * This string is auto-filled by the Makefile to contain the git 
 * repository branch from which this firmware was built.
 */
#define FIRMWARE_REVISION (STRINGIFY_CONTENTS(_FIRMWARE_REVISION))

/**
 * This string describes the type of the firmware -- e.g., Application or Bootloader.
 */
#define FIRMWARE_MODE (STRINGIFY_CONTENTS(_FIRMWARE_MODE))

/**
 * This string is the type of the boards in the module.
 */
#define BOARD_TYPE (STRINGIFY_CONTENTS(_BOARD_TYPE))

/**
 * This string is the electrical type of the module (may be the same as BOARD_TYPE, but
 * not necessarily so if several projects use the same board).
 */
#define ELECTRICAL_TYPE (STRINGIFY_CONTENTS(_ELECTRICAL_TYPE))

#endif /* INFO_H_ */
