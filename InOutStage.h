//
//  InOutStage.h
//  PipelineExmpl
//
//  Created by grigorii on 28/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//

#ifndef InOutStage_h
#define InOutStage_h

#include "Runnable.h"
#include "BlockingMessageQueue.h"
#include "Message.h"

class InOutStage {
protected:
    Runnable rnbl;
    std::shared_ptr<BlockingMessageQueue> inBmq;
    std::shared_ptr<BlockingMessageQueue> outBmq;
    std::atomic<bool> completed;
public:
    InOutStage() : completed(false) { }
    
    virtual ~InOutStage() { }
    
    void SetInBMQ( std::shared_ptr<BlockingMessageQueue> bmq ) {
        this->inBmq = bmq;
    }
    void SetOutBMQ( std::shared_ptr<BlockingMessageQueue> bmq ) {
        this->outBmq = bmq;
    }
    
    bool TaskCompleted() {
        return completed.load();
    }
    
    virtual std::unique_ptr<Message> StageTask( std::unique_ptr<Message> msg ) = 0;
    
    void Run() {
        completed = false;
        rnbl.Execute( [&] {
            while( !completed ) {
                auto msg = inBmq->Pop();
                
                MessageType t = msg->type;

                auto procMsg = StageTask( std::move( msg ) );
                
                outBmq->Push( std::move( procMsg ) );
                
                if( t == MessageType::last )
                    completed = true;
            }
        } );
    }
};

#endif /* InOutStage_h */
