//
//  InStage.h
//  PipelineExmpl
//
//  Created by grigorii on 28/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//

#ifndef InStage_h
#define InStage_h


#include "Runnable.h"
#include "BlockingMessageQueue.h"
#include "Message.h"

//const int IN_PACKAGE_SIZE = 512 * 384;
//const int IN_PACKAGE_NUM = 10000;

class InStage {
protected:
    Runnable rnbl;
    std::shared_ptr<BlockingMessageQueue> bmq;
    std::atomic<bool> completed;
    
public:
    
    void SetOutBMQ( std::shared_ptr<BlockingMessageQueue> bmq ) {
        this->bmq = bmq;
    }
    
    bool TaskCompleted() {
        return completed.load();
    }
    
    virtual void StageTask() = 0;
    
    void Run() {
        completed = false;
        rnbl.Execute( [&] {
            StageTask();
            completed = true;
        } );
    }
};

#endif /* InStage_h */
