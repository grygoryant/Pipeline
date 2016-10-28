//
//  OutStage.h
//  PipelineExmpl
//
//  Created by grigorii on 28/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//

#ifndef OutStage_h
#define OutStage_h

#include "Runnable.h"
#include "BlockingMessageQueue.h"
#include "Message.h"

class OutStage {
protected:
    Runnable rnbl;
    std::shared_ptr<BlockingMessageQueue> bmq;
    std::atomic<bool> completed;
    
public:
    void SetInBMQ( std::shared_ptr<BlockingMessageQueue> bmq ) {
        this->bmq = bmq;
    }
    
    bool TaskCompleted() {
        return completed.load();
    }
    
    virtual void StageTask( std::unique_ptr<Message> msg ) = 0;
    
    void Run() {
        completed = false;
        rnbl.Execute( [&] {
            while( !completed ) {
                auto msg = bmq->Pop();
                MessageType t = msg->type;
                StageTask( std::move( msg ) );
                if( t == MessageType::last )
                    completed = true;
            }
        } );
    }
};

#endif /* OutStage_h */
