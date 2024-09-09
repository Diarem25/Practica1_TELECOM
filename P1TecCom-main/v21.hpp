#ifndef V21_HPP
#define V21_HPP

#include <functional>
#include <deque>
#include "config.hpp"

class V21_RX
{
public:
    V21_RX(float omega_mark, float omega_space, std::function<void(const unsigned int *, unsigned int)> get_digital_samples)
        :omega_mark(omega_mark),omega_space(omega_space),get_digital_samples(get_digital_samples) {
            for (int i = 0; i < SAMPLES_PER_SYMBOL; i++)    s_buffer.push_front(0);
            vSr_buffer = 0;
            vSi_buffer = 0;
            vMr_buffer = 0;
            vMi_buffer = 0;
            unfiltered_buffer[0] = 0;
            unfiltered_buffer[1] = 0;
            filtered_buffer[0] = 0;
            filtered_buffer[1] = 0;
        };
    void demodulate(const float *in_analog_samples, unsigned int n);
private:
    float omega_mark, omega_space;
    float vSr_buffer, vSi_buffer, vMr_buffer, vMi_buffer;
    float unfiltered_buffer[2];
    float filtered_buffer[2];
    std::deque<float> s_buffer;
    enum {
        IDLE,
        CARRIER_DETECTED
    } state;
    std::function<void(const unsigned int *, unsigned int)> get_digital_samples;
};

class V21_TX
{
public:
    V21_TX(float omega_mark, float omega_space) :omega_mark(omega_mark),omega_space(omega_space),phase(0.f) {};
    void modulate(const unsigned int *in_digital_samples, float *out_analog_samples, unsigned int n);
private:
    float omega_mark, omega_space;
    float phase;
};

#endif
