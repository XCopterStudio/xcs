#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

class DataReceiver{
    virtual template <class type> void notify(unsigned int id, type data) = 0;
};

#endif