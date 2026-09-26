/******************************************************************************
 * @file    definitions.h
 * @brief   Hardware definitions for the IMPASS system (ESP32-S3)
 *
 * @project IMPASS - Master's Thesis
 * @company University of Málaga, Master's Degree in Electronic Systems for Intelligent Environments
 * @author  Luis Miguel Carmona Cortés
 * @date    2026
 * @version 1.0
 *
 * @details
 * This file contains the pin assignments, I2C addresses, and
 * hardware-specific configurations for the ESP32-S3-based PCB.
 *
 * @note    Any hardware modification should be reflected in this file.
 ******************************************************************************/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* ===== CH32v006 pin definitions ===== */

// Overtemp alert pins
#define OVERTEMP_REG1
#define OVERTEMP_REG2
#define OVERTEMP_REG3
#define OVERTEMP_BOARD

// Power supply control pins
#define BOOST_REG3
#define BUCK_REG3
#define BOOST_REG2
#define BUCK_REG2
#define BOOST_REG1
#define BUCK_REG1

// Mosfet control pins
#define SHIFT_LATCH
#define SHIFT_CLOCK
#define SHIFT_DATA

// Generic pins
#define BUTTON1
#define BUTTON2
#define BUTTON3

// I2C pins
#define I2C_SDA_GPIO
#define I2C_SCL_GPIO

// SPI pins
#define CS_DISPLAY
#define MOSI
#define SCK
#define MISO

/* ===== I2C address definitions ===== */

// Temperature sensors
#define LM75_SELECTOR_BOARD     0x48
#define LM75_REG1_BOARD
#define LM75_REG2_BOARD
#define LM75_REG3_BOARD

// Current sensors
#define INA3221_BASIC_VOLTAGES  0x40
#define INA3221_CUSTOM_VOLTAGES 0x41

// Digital potentiometer
#define DS1803_REG1
#define DS1803_REG2
#define DS1803_REG3

// Eeprom
#define AT24C128_EEPROM         0x50

#endif