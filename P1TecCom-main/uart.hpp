#ifndef UART_HPP
#define UART_HPP

#include <functional>
#include <deque>
#include <mutex>
#include <stdint.h>
#include "config.hpp"

class UART_RX
{
public:
    UART_RX(std::function<void(uint8_t)> get_byte) :get_byte(get_byte), x(0), count(0), count_low(0), num_bit(0),
        state(WAIT) {
            for (int i = 0; i < 93; i++) this->samples.push_front(1);
        }
    void put_samples(const unsigned int *buffer, unsigned int n);
private:
    std::function<void(uint8_t)> get_byte;
   uint8_t x;
    unsigned int count;
    unsigned int count_low;
    unsigned int num_bit;
    std::deque<unsigned int> samples;
    enum {
        WAIT,
        RECEIVE,
        STOP
    } state;
};

class UART_TX
{
public:
    void put_byte(uint8_t byte);
    void get_samples(unsigned int *buffer, unsigned int n);
private:
    std::deque<unsigned int> samples;
    std::mutex samples_mutex;
    void put_bit(unsigned int bit);
};

#endif
