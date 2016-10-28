//
//  Message.h
//  PipelineExmpl
//
//  Created by grigorii on 27/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//

#ifndef Message_h
#define Message_h

#include <iostream>


enum class MessageType {
    notLast,
    last
};

struct Message {
    std::vector<int> data;
    MessageType type;
    Message( int value = 0, int n = 500, MessageType type = MessageType::notLast ) : data( n, value ), type( type ){ }
};


#endif /* Message_h */
