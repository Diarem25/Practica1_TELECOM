#include <math.h>
#include <numbers>
#include "v21.hpp"

void V21_RX::demodulate(const float *in_analog_samples, unsigned int n)
{
    unsigned int digital_samples[n];
    unsigned int consec_low = 0;
    float vSr, vSi, vMr, vMi, decision, after_filter;
    
    for (int i = 0; i < n; i++) {
        s_buffer.push_front(in_analog_samples[i]);
        
        vSr = s_buffer[0] - pow(0.99f,SAMPLES_PER_SYMBOL) * cos(omega_space*SAMPLES_PER_SYMBOL*SAMPLING_PERIOD) * s_buffer[SAMPLES_PER_SYMBOL] 
            + 0.99f * cos(omega_space*SAMPLING_PERIOD) * vSr_buffer - 0.99f * sin(omega_space*SAMPLING_PERIOD) * vSi_buffer ;
        vSi = -pow(0.99f, SAMPLES_PER_SYMBOL) * sin(omega_space*SAMPLES_PER_SYMBOL*SAMPLING_PERIOD) * s_buffer[SAMPLES_PER_SYMBOL] 
            + 0.99f * cos(omega_space * SAMPLING_PERIOD) * vSi_buffer + 0.99f * sin(omega_space * SAMPLING_PERIOD) * vSr_buffer;
        vMr = s_buffer[0] - pow(0.99f, SAMPLES_PER_SYMBOL) * cos(omega_mark*SAMPLES_PER_SYMBOL*SAMPLING_PERIOD) * s_buffer[SAMPLES_PER_SYMBOL] 
            + 0.99f * cos(omega_mark * SAMPLING_PERIOD) * vMr_buffer - 0.99f * sin(omega_mark * SAMPLING_PERIOD) * vMi_buffer;
        vMi = -pow(0.99f, SAMPLES_PER_SYMBOL) * sin(omega_mark*SAMPLES_PER_SYMBOL*SAMPLING_PERIOD) * s_buffer[SAMPLES_PER_SYMBOL]
            + 0.99f * cos(omega_mark * SAMPLING_PERIOD) * vMi_buffer + 0.99f * sin(omega_mark * SAMPLING_PERIOD) * vMr_buffer;

        decision = vMr * vMr + vMi * vMi - vSr * vSr - vSi * vSi;

        after_filter = 0.00037507f * decision + 0.00075014f * unfiltered_buffer[0] + 0.00037507f * unfiltered_buffer[1]
                     - (-1.94447766f) * filtered_buffer[0] - 0.94597794f * filtered_buffer[1];

        after_filter /= 1.0f;
    
        if(abs(after_filter) > 120) state = CARRIER_DETECTED;
        else    {if(consec_low >= 50)    state = IDLE;}

        if(state == CARRIER_DETECTED){
            if(abs(after_filter) < 60)  consec_low++;
            else {
                consec_low = 0;
                if(after_filter > 0)    digital_samples[i] = 1;
                else    digital_samples[i] = 0;
            }
        }    
        else    {if(state == IDLE) digital_samples[i] = 1;}

        s_buffer.pop_back();
        vSr = vSr_buffer;
        vSi = vSi_buffer;
        vMr = vMr_buffer;
        vMi = vMi_buffer;
        unfiltered_buffer[1] = unfiltered_buffer[0];
        unfiltered_buffer[0] = decision;
        filtered_buffer[1] = filtered_buffer[0];
        filtered_buffer[0] = after_filter;
    }

    get_digital_samples(digital_samples, n);
}

void V21_TX::modulate(const unsigned int *in_digital_samples, float *out_analog_samples, unsigned int n)
{
    while (n--) {
        *out_analog_samples++ = sin(phase);
        phase += (*in_digital_samples++ ? omega_mark : omega_space) * SAMPLING_PERIOD;

        // evita que phase cresça indefinidamente, o que causaria perda de precisão
        phase = remainder(phase, 2*std::numbers::pi);
    }
}
