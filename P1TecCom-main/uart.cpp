#include "uart.hpp"

void UART_RX::put_samples(const unsigned int *buffer, unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        this->samples.push_front(buffer[i]);

        if (this->samples[0] == 0)  this->count_low++; // conta low bits

        if (this->samples[30] == 0) this->count_low--; // low bit sai da janela

        if (state == WAIT){
            if (count_low >= 25 && this->samples[93] == 0) { // Inicio dos dados
                this->count = 15; 
                this->x = 0;
                this->num_bit = 0;
                this->state = RECEIVE;
            }
        }

        else{
            if (state == RECEIVE){
                if (this->count == 159) {
                    this->x += this->samples[0] << this->num_bit;
                    this->num_bit++;
                    this->count = 0; 
                    if (this->num_bit == 8)    this->state = STOP;
                } 
                else  this->count++; 
            }

            else {
                if (state == STOP){ 
                    if (this->count == 159) { // Terminou de receber o byte
                        this->get_byte(this->x);
                        this->state = WAIT;
                    } 
                    else  this->count++;
                }
            }
        }

        this->samples.pop_back();
    }
}

void UART_TX::put_byte(uint8_t byte)
{
    samples_mutex.lock();
    put_bit(0);  // start bit
    for (int i = 0; i < 8; i++) {
        put_bit(byte & 1);
        byte >>= 1;
    }
    put_bit(1);  // stop bit
    samples_mutex.unlock();
}

void UART_TX::get_samples(unsigned int *buffer, unsigned int n)
{
    samples_mutex.lock();
    std::vector<unsigned int>::size_type i = 0;
    while (!samples.empty() && i < n) {
        buffer[i++] = samples.front();
        samples.pop_front();
    }
    samples_mutex.unlock();

    while (i < n) {
        // idle
        buffer[i++] = 1;
    }
}

void UART_TX::put_bit(unsigned int bit)
{
    for (int i = 0; i < SAMPLES_PER_SYMBOL; i++) {
        samples.push_back(bit);
    }
}
