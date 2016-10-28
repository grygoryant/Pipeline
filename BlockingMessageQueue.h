//
//  BlockingQueue.h
//  PipelineExmpl
//
//  Created by grigorii on 27/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//

#ifndef BlockingMessageQueue_h
#define BlockingMessageQueue_h

#include <iostream>
#include <thread>
#include <queue>

#include "Message.h"

class BlockingMessageQueue {
    std::mutex qMutex;
    std::condition_variable qCv;
    std::queue<std::unique_ptr<Message>> queue;
    
public:
    
    void Push( std::unique_ptr<Message> msg ) {
        std::unique_lock<std::mutex> lock( qMutex );
        queue.push( std::move( msg ) );
        qCv.notify_one();
    }
    
    std::unique_ptr<Message> Pop() {
        std::unique_lock<std::mutex> lock( qMutex );
        qCv.wait( lock, [&] {
            return !queue.empty();
        });
        std::unique_ptr<Message> retMsg( std::move( queue.front() ) );
        queue.pop();
        return retMsg;
    }
};

#endif /* BlockingQueue_h */
