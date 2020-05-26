#ifndef __LED_TRANSLATE_HPP__
#define __LED_TRANSLATE_HPP__

#include <Arduino.h>
#include "../struct/packet.hpp"
#include "../struct/translate.hpp"

class LedTranslate : protected Translate {
    protected:
    void broadcast(const device_communication_message_t message){
        Broadcast<device_communication_message_t>::getInstance()->broadcast(message);
    }

    public:
    LedTranslate(HardwareSerial* _master, HardwareSerial* _right, HardwareSerial* _bottom) : 
        Translate(_master, _right, _bottom) {
    }

    LedTranslate() : Translate() {
    }

    void recv(){
        if(this->master->available() >= sizeof(device_communication_message_t)){
            auto message = new device_communication_message_t;
            this->master->readBytes(message->bytes, sizeof(device_communication_message_t)); 
            this->broadcast(*message);
            ESP_LOGI(typename(this), "Packet : %s", bytesToHex(message->bytes, sizeof(device_communication_message_t)).c_str());
            switch(message->type){
                case MESSAGE_TYPE::RUN_MOTOR : {
                    this->bottom->write(message->bytes, sizeof(device_communication_message_t));
                    this->right->write(message->bytes, sizeof(device_communication_message_t));
                    break;
                }
                case MESSAGE_TYPE::SET_COLOR : {
                    auto led = reinterpret_cast<led_message_t*>(&(message->message));
                    if(led->col > 0){
                        led->col -= 1;
                        this->right->write(message->bytes, sizeof(device_communication_message_t));
                        led->col += 1;
                    }
                    if(led->row > 0){
                        led->row -= 1;
                        this->bottom->write(message->bytes, sizeof(device_communication_message_t));
                    }
                    break;
                }
                default : break;
            }
            delete message;
        }
    }

    void send(){
        
    }
};

#endif