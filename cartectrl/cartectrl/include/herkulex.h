/**
  ******************************************************************************
  * @file    herkulex.h
  * @author  Yoonseok Pyo
  * @author  modifications by Mathieu Drapeau (mtiudaeu)
  * @author  modifications by James-Adam Renquinha Henri (Jarhmander)
  * @version 0.1
  * @date    2013-10-24
  * @brief   Low level control of Herkulex servo-motors
  * @todo    More refactoring is needed
  * @note    Here is the original copyright information about this file:
  * @verbatim
  * herkulex servo library for mbed
  *
  * Copyright (c) 2012-2013 Yoonseok Pyo, MIT License
  *
  * Permission is hereby granted, free of charge, to any person obtaining a
  * copy of this software and associated documentation files (the "Software"),
  * to deal in the Software without restriction, including without limitation
  * the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the
  * Software is furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  * DEALINGS IN THE SOFTWARE.
  * @endverbatim
  ******************************************************************************
  */

//------------------------------------------------------------------------------
#ifndef HERKULEX_H
#define HERKULEX_H
//------------------------------------------------------------------------------
#include <cstdint>
#include <initializer_list>
#include <new>
#include "UARTinterface.hpp"
#include "bsp/TimedTasks.hpp"
//------------------------------------------------------------------------------
#include "herkulex/internals.hpp"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Herkulex ROM Register
#define ROM_MODEL_NO1                       0
#define ROM_MODEL_NO2                       1
#define ROM_VERSION1                        2
#define ROM_VERSION2                        3
#define ROM_BAUD_RATE                       4
#define ROM_RESERVED5                       5
#define ROM_ID                              6
#define ROM_ACK_POLICY                      7
#define ROM_ALARM_LED_POLICY                8
#define ROM_TORQUE_POLICY                   9
#define ROM_RESERVED10                      10
#define ROM_MAX_TEMPERATURE                 11
#define ROM_MIN_VOLTAGE                     12
#define ROM_MAX_VOLTAGE                     13
#define ROM_ACCELERATION_RATIO              14
#define ROM_MAX_ACCELERATION_TIME           15
#define ROM_DEAD_ZONE                       16
#define ROM_SATURATOR_OFFSET                17
#define ROM_SATURATOR_SLOPE                 18  // 2Byte
#define ROM_PWM_OFFSET                      20
#define ROM_MIN_PWM                         21
#define ROM_MAX_PWM                         22  // 2Byte
#define ROM_OVERLOAD_PWM_THRESHOLD          24  // 2Byte
#define ROM_MIN_POSITION                    26  // 2Byte
#define ROM_MAX_POSITION                    28  // 2Byte
#define ROM_POSITION_KP                     30  // 2Byte
#define ROM_POSITION_KD                     32  // 2Byte
#define ROM_POSITION_KI                     34  // 2Byte
#define ROM_POSITION_FEEDFORWARD_1ST_GAIN   36  // 2Byte
#define ROM_POSITION FEEDFORWARD_2ND_GAIN   38  // 2Byte
#define ROM_RESERVED40                      40  // 2Byte
#define ROM_RESERVED42                      42  // 2Byte
#define ROM_LED_BLINK_PERIOD                44
#define ROM_ADC_FAULT_CHECK_PERIOD          45
#define ROM_PACKET_GARBAGE_CHECK_PERIOD     46
#define ROM_STOP_DETECTION_PERIOD           47
#define ROM_OVERLOAD_DETECTION_PERIOD       48
#define ROM_STOP_THRESHOLD                  49
#define ROM_INPOSITION_MARGIN               50
#define ROM_RESERVED51                      51
#define ROM_RESERVED52                      52
#define ROM_CALIBRATION_DIFFERENCE          53

//------------------------------------------------------------------------------
// Herkulex RAM Register
#define RAM_ID                              0
#define RAM_ACK_POLICY                      1
#define RAM_ALARM_LED_POLICY                2
#define RAM_TORQUE_POLICY                   3
#define RAM_RESERVED4                       4
#define RAM_MAX_TEMPERATURE                 5
#define RAM_MIN_VOLTAGE                     6
#define RAM_MAX_VOLTAGE                     7
#define RAM_ACCELERATION_RATIO              8
#define RAM_MAX_ACCELERATION                9
#define RAM_DEAD_ZONE                       10
#define RAM_SATURATOR_OFFSET                11
#define RAM_SATURATOR_SLOPE                 12 // 2Byte
#define RAM_PWM_OFFSET                      14
#define RAM_MIN_PWM                         15
#define RAM_MAX_PWM                         16 // 2Byte
#define RAM_OVERLOAD_PWM_THRESHOLD          18 // 2Byte
#define RAM_MIN_POSITION                    20 // 2Byte
#define RAM_MAX_POSITION                    22 // 2Byte
#define RAM_POSITION_KP                     24 // 2Byte
#define RAM_POSITION_KD                     26 // 2Byte
#define RAM_POSITION_KI                     28 // 2Byte
#define RAM_POSITION_FEEDFORWARD_1ST_GAIN   30 // 2Byte
#define RAM_POSITION_FEEDFORWARD 2ND GAIN   32 // 2Byte
#define RAM_RESERVED34                      34 // 2Byte
#define RAM_RESERVED36                      36 // 2Byte
#define RAM_LED_BLINK_PERIOD                38
#define RAM_ADC_FAULT_DETECTION_PERIOD      39
#define RAM_PACKET_GARBAGE_DETECTION_PERIOD 40
#define RAM_STOP_DETECTION_PERIOD           41
#define RAM_OVERLOAD_DETECTION_PERIOD       42
#define RAM_STOP_THRESHOLD                  43
#define RAM_INPOSITION_MARGIN               44
#define RAM_RESERVED45                      45
#define RAM_RESERVED46                      46
#define RAM_CALIBRATION_DIFFERENCE          47
#define RAM_STATUS_ERROR                    48
#define RAM_STATUS_DETAIL                   49
#define RAM_RESERVED50                      50
#define RAM_RESERVED51                      51
#define RAM_TORQUE_CONTROL                  52
#define RAM_LED_CONTROL                     53
#define RAM_VOLTAGE                         54
#define RAM_TEMPERATURE                     55
#define RAM_CURRENT_CONTROL_MODE            56
#define RAM_TICK                            57
#define RAM_CALIBRATED_POSITION             58 // 2Byte
#define RAM_ABSOLUTE_POSITION               60 // 2Byte
#define RAM_DIFFERENTIAL_POSITION           62 // 2Byte
#define RAM_PWM                             64 // 2Byte
#define RAM_RESERVED66                      66 // 2Byte
#define RAM_ABSOLUTE_GOAL_POSITION          68 // 2Byte
#define RAM_ABSOLUTE_DESIRED_TRAJECTORY_POSITION    70 // 2Byte
#define RAM_DESIRED_VELOCITY                72 // 2Byte

//------------------------------------------------------------------------------
// ACK Packet [To Controller(ACK)]
#define CMD_ACK_MASK   0x40 // ACK Packet CMD is Request Packet CMD + 0x40
#define CMD_EEP_WRITE_ACK   (CMD_EEP_WRITE|CMD_ACK_MASK)
#define CMD_EEP_READ_ACK    (CMD_EEP_READ|CMD_ACK_MASK)
#define CMD_RAM_WRITE_ACK   (CMD_RAM_WRITE|CMD_ACK_MASK)
#define CMD_RAM_READ_ACK    (CMD_RAM_READ|CMD_ACK_MASK)
#define CMD_I_JOG_ACK       (CMD_I_JOG|CMD_ACK_MASK)
#define CMD_S_JOG_ACK       (CMD_S_JOG|CMD_ACK_MASK)
#define CMD_STAT_ACK        (CMD_STAT|CMD_ACK_MASK)
#define CMD_ROLLBACK_ACK    (CMD_ROLLBACK|CMD_ACK_MASK)
#define CMD_REBOOT_ACK      (CMD_REBOOT|CMD_ACK_MASK)

//------------------------------------------------------------------------------
// Status Error
#define STATUS_OK                       = 0x00;
#define ERROR_EXCEED_INPUT_VOLTAGE      = 0x01;
#define ERROR_EXCEED_POT_LIMIT          = 0x02;
#define ERROR_EXCEED_TEMPERATURE_LIMIT  = 0x04;
#define ERROR_INVALID_PACKET            = 0x08;
#define ERROR_OVERLOAD                  = 0x10;
#define ERROR_DRIVER_FAULT              = 0x20;
#define ERROR_EEP_REG_DISTORT           = 0x40;

//------------------------------------------------------------------------------
// Status Detail
#define MOVING_FLAG                     = 0x01;
#define INPOSITION_FLAG                 = 0x02;
#define CHECKSUM_ERROR                  = 0x04; // Invalid packet`s detailed information
#define UNKNOWN_COMMAND                 = 0x08; // Invalid packet`s detailed information
#define EXCEED_REG_RANGE                = 0x10; // Invalid packet`s detailed information
#define GARBAGE_DETECTED                = 0x20; // Invalid packet`s detailed information
#define MOTOR_ON_FLAG                   = 0x40;

//------------------------------------------------------------------------------


// ID
#define MAX_PID                             0xFD
#define DEFAULT_ID                          0xFD
#define MAX_ID                              0xFD
#define BROADCAST_ID                        0xFE

// Checksum
#define CHKSUM_MASK                         0xFE

// Torque CMD
#define TORQUE_FREE                         0x00
#define BREAK_ON                            0x40
#define TORQUE_ON                           0x60

// Jog Set CMD
#define STOP                                0x01
#define POS_MODE                            0x00
#define TURN_MODE                           0x02
#define GLED_ON                             0x04
#define BLED_ON                             0x08
#define RLED_ON                             0x10

//------------------------------------------------------------------------------
/** herkulex Servo control class, based packet protocol on a serial
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "herkulex.h"
 *
 * Herkulex sv(p9, p10, 115200);
 * DigitalOut led(LED1);
 *
 * int main()
 * {
 *     wait(1);
 *     sv.setTorque(0xFD, TORQUE_ON);
 *     while(1)
 *     {
 *         led=1;
 *         sv.movePos(0xFD, 1002, 100, SET_MODE_POS, SET_LED_GREEN_ON);
 *         wait(3);
 *
 *         led=0;
 *         sv.movePos(0xFD, 21, 100, SET_MODE_POS, SET_LED_BLUE_ON);
 *         wait(3);
 *     }
 * }
 * @endcode
 */
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
class Herkulex : private UART_Callback
{
public:
    /**@brief Create an Herkulex servo object
     *
     * @param com UART pointer object.
     */
    Herkulex(UARTInterface *com);

    /**
    * @brief Destroy an Herkulex servo object
     */
    ~Herkulex();



    /******** Very low level Herkulex manipulation *********/

    template <typename MsgType>
     struct wrapped_fifo_ptr : public fifo_ptr
    {
        wrapped_fifo_ptr(fifo_ptr &&fptr) : fifo_ptr(std::move(fptr)) {}

        MsgType &operator*()  { return *static_cast<MsgType *>(this->get()); }
        MsgType *operator->() { return &**this; };
    };
    template <servocmd scmd, typename ...Args>
     wrapped_fifo_ptr<msgtype<scmd>> newMsg(uint8_t id, uint8_t len,
                                                                Args ...args)
    {
        fifo_ptr p;
        do {
            p = com->alloc(msgtraits<scmd>::len(len));
        } while (!p);
        new (p.get()) msgtype<scmd>(id, len, args...);
        return {std::move(p)};
    }

    void sendPacket(fifo_ptr &);

    void sendPacket(fifo_ptr &&p)
    {
        return sendPacket(p);
    }






    /*********** Low level Herkulex manipulation ***********/

    /**@brief Write into EEPROM
     *
     * @param id The herkulex servo ID.
     */
    void send_eep_write(uint8_t id, uint8_t addr, const uint8_t *beg,
                                                  const uint8_t *end);

    /**@brief Write into EPPROM (initializer list overload)
     *
     * @param id The herkulex servo ID.
     */
    void send_epp_write(uint8_t id, uint8_t addr,
                                    std::initializer_list<uint8_t> il)
    {
        return send_eep_write(id, addr, il.begin(), il.end());
    }

    /**@brief Read from EEPROM
     *
     * @param id The herkulex servo ID.
     */
    void send_eep_read(uint8_t id, uint8_t addr, uint8_t len);

    /**@brief Write into RAM
     *
     * @param id The herkulex servo ID.
     */
    void send_ram_write(uint8_t id, uint8_t addr, const uint8_t *beg,
                                                  const uint8_t *end);

    /**@brief Write into RAM (initializer list overload)
     *
     * @param id The herkulex servo ID.
     */
    void send_ram_write(uint8_t id, uint8_t addr,
                                    std::initializer_list<uint8_t> il)
    {
        return send_ram_write(id, addr, il.begin(), il.end());
    }

    /**@brief Read from RAM
     *
     * @param id The herkulex servo ID.
     */
    void send_ram_read(uint8_t id, uint8_t addr, uint8_t len);

    /**@brief Independent control of several servos
     *
     * @param id The herkulex servo ID.
     */
    void send_i_jog(uint8_t id, const i_jog_part *beg, const i_jog_part *end);

    /**@brief Independent control of several servos (initializer list overload)
     *
     * @param id The herkulex servo ID.
     */
    void send_i_jog(uint8_t id, std::initializer_list<i_jog_part> il)
    {
        return send_i_jog(id, il.begin(), il.end());
    }

    /**@brief Synchronized control of several servos
     *
     * @param id The herkulex servo ID.
     */
    void send_s_jog(uint8_t id, uint8_t time,
                                const s_jog_part *beg, const s_jog_part *end);

    /**@brief Synchronized control of several servos (initializer list overload)
     *
     * @param id The herkulex servo ID.
     */
    void send_s_jog(uint8_t id, uint8_t time,
                                std::initializer_list<s_jog_part> il)
    {
        return send_s_jog(id, time, il.begin(), il.end());
    }

    /**@brief Get status
     *
     * @param id The herkulex servo ID.
     */
    void send_stat(uint8_t id);

    /**@brief Reset EEPROM settings
     *
     * @param id        The herkulex servo ID.
     * @param keepID    Preserve ID in EEPROM.
     * @param keepBaud  Preserve Baud rate in EEPROM.
     */
    void send_rollback(uint8_t id, bool keepID, bool keepBaud);

    /**@brief Reboot servos
     *
     * @param id The herkulex servo ID.
     */
    void send_reboot(uint8_t id);






    /*********** High level Herkulex manipulation **********/

    /**@brief Clear error status
     *
     * @param id The herkulex servo ID.
     */
     void clear(uint8_t id);

    /**@brief Set torque setting
     *
     * @param id The herkulex servo ID.
     * @param cmdTorue The Command for setting of torque (TORQUE_FREE 0x00, BREAK_ON 0x40, TORQUE_ON 0x60)
     */
    void setTorque(uint8_t id, uint8_t cmdTorque);

    /**@brief Position Control
     *
     * @param id The herkulex servo ID.
     * @param position The goal position of herkulex servo.
     * @param playtime Time to target position.
     * @param setLED Select LED and on/off controll (GLED_ON 0x00,BLED_ON 0x08, RLED_ON 0x10)
     */
    void positionControl(uint8_t id, uint16_t position, uint8_t playtime, uint8_t setLED);

    /**@brief Velocity Control
     *
     * @param id The herkulex servo ID.
     * @param speed The goal position of herkulex servo.
     * @param setLED Select LED and on/off control (GLED_ON 0x00,BLED_ON 0x08, RLED_ON 0x10)
     */
    void velocityControl(uint8_t id, int16_t speed,uint8_t setLED);

    /**@brief Get Status
     *
     * @param id The herkulex servo ID.
     * @return -1 is getStatus failed. other is servo's status error value.
     */
    int8_t getStatus(uint8_t id);

    /**@brief Get Position
     *
     * @param id The herkulex servo ID.
     * @return -1 is getPos failed. other is servo's current position.
     */
    int16_t getPos(uint8_t id);

    struct MsgHandler
    {
        virtual void err_header(msgPacket *)    {}
        virtual void err_checksum(msgPacket *)  {}

        virtual void unknownPacket(msgPacket *) {}

        virtual void stat    (msgStat *)     {}
        virtual void eep_read(msgEEP_read *) {}
        virtual void ram_read(msgRAM_read *) {}
    };

    class fwd_MsgHandler : public MsgHandler
    {
        MsgHandler *next;
    public:
        using base = MsgHandler;

        fwd_MsgHandler(base *b) : next(b) {}

        void err_header(msgPacket *m) override
        {
            return next->err_header(m);
        }
        void err_checksum(msgPacket *m) override
        {
            return next->err_checksum(m);
        }

        void unknownPacket(msgPacket *m) override
        {
            return next->unknownPacket(m);
        }

        void stat(msgStat *m) override
        {
            return next->stat(m);
        }
        void eep_read(msgEEP_read *m) override
        {
            return next->eep_read(m);
        }
        void ram_read(msgRAM_read *m) override
        {
            return next->ram_read(m);
        }
    };

    void        handler(MsgHandler *h);
    MsgHandler *handler() const        { return mhandler; }

    void        resetHandler(MsgHandler *h = nullptr)
    {
        return handler(h);
    }

private :
    bsp::CPUclockdiff cpuc;

    using fifo_ptr = ::fifo_ptr;
    UARTInterface *com;

    MsgHandler *mhandler;

    // UART_Callback:
    void rx(int) override;

    size_t idx = 0;
    uint8_t buffer[256];
};

//------------------------------------------------------------------------------
#endif  // HERKULEX_H


//------------------------------------------------------------------------------
