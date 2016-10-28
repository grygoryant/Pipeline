//
//  main.cpp
//  PipelineExmpl
//
//  Created by grigorii on 25/10/16.
//  Copyright © 2016 grigorii. All rights reserved.
//


#include <iostream>
#include <unistd.h>

#include "InStage.h"
#include "OutStage.h"
#include "InOutStage.h"
#include "timer.hpp"

const int IN_PACKAGE_SIZE = 1024*768;
const int IN_PACKAGE_NUM = 10000;

class ConcreteInStage : public InStage {
protected:
	
public:

	void StageTask( ) override {
		for( int i = 0; i < IN_PACKAGE_NUM; ++i ) {
			
			auto msg = std::make_unique<Message>( i, IN_PACKAGE_SIZE,
												 ( i == IN_PACKAGE_NUM - 1 ) ? MessageType::last : MessageType::notLast );
			//std::cout << "Push it to the limit " << i << "\n";
			bmq->Push( std::move( msg ) );
		}
	}
};

class ConcreteOutStage : public OutStage {
public:
	void StageTask( std::unique_ptr<Message> msg ) override {
		std::string m( "Read " );
		m += std::to_string( msg->data[0] ) + "\n";
		std::cerr << m;
	}
};

class FirstConcreteInOutStage : public InOutStage {
public:
	FirstConcreteInOutStage() : InOutStage() { }
	
	std::unique_ptr<Message> StageTask( std::unique_ptr<Message> msg ) override {
		std::for_each( std::begin( msg->data ) , std::end( msg->data ), [] ( int &n ) {
			n *= 10;
		});
		return std::move( msg );
	}
};
		
class SecondConcreteInOutStage : public InOutStage {
public:
	SecondConcreteInOutStage() : InOutStage() { }
	
	std::unique_ptr<Message> StageTask( std::unique_ptr<Message> msg ) override {
		std::for_each( std::begin( msg->data ) , std::end( msg->data ), [] ( int &n ) {
			n /= 10;
		});
		return std::move( msg );
	}
};

int main(int argc, const char * argv[]) {

	//is -> bmq1 -> ios1 -> bmq2 -> ios2 -> bmq3 -> os
	auto bmq1 = std::make_shared<BlockingMessageQueue>( );
	auto bmq2 = std::make_shared<BlockingMessageQueue>( );
	auto bmq3 = std::make_shared<BlockingMessageQueue>( );
	
	auto is = std::make_shared<ConcreteInStage>( );
	auto os = std::make_shared<ConcreteOutStage>( );
	auto ios1 = std::make_shared<FirstConcreteInOutStage>( );
	auto ios2 = std::make_shared<SecondConcreteInOutStage>( );
	
	is->SetOutBMQ( bmq1 );
	ios1->SetInBMQ( bmq1 );
	ios1->SetOutBMQ( bmq2 );
	ios2->SetInBMQ( bmq2 );
	ios2->SetOutBMQ( bmq3 );
	os->SetInBMQ( bmq3 );
	
	TimePoint t1 = currentTime();
	is->Run();
	ios1->Run();
	ios2->Run();
	os->Run();

	while( !os->TaskCompleted() );
	TimePoint t2 = currentTime();
	
	std::cout << "Elapsed time: " << elapsedTime( t1, t2 ) << "\n";
	
	t1 = currentTime();
	for( int i = 0; i < IN_PACKAGE_NUM; ++i ) {
		std::vector<int> v( IN_PACKAGE_SIZE, i );
		std::for_each( std::begin( v ) , std::end( v ), [] ( int &n ) {
			n *= 10;
			n /= 10;
		});
		std::for_each( std::begin( v ) , std::end( v ), [] ( int &n ) {
			n *= 10;
			n /= 10;
		});
		std::string m( "Read " );
		m += std::to_string( v[0] ) + "\n";
		std::cerr << m;
	}
	t2 = currentTime();
	
	std::cout << "Elapsed time: " << elapsedTime( t1, t2 ) << "\n";
	
	return 0;
}

