//
//  Messenger.hpp
//  Pockets
//
//  Created by David Wicks on 11/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Receiver.hpp"

/**
Simple message passing library

Message: What is sent from Messenger to Subject, a templated type.
Messenger<T>: Sends out messages of type T.
Receiver<T>: Receives messages of type T.

If the receiver expects multiple messengers, it should inherit from multiple Receiver<T>
types and implement receive( const T& ); for each type. This eliminates the need
for casting present in the previous, non-templated version of the library.

If you want to send multiple types of messages from a single object, you should
compose Messengers into your class rather than inheriting. Then provide your own
interface for adding/removing receivers to the different messengers.
Another option is to define a custom message type that is flexible enough for your
needs and inherit Messenger<AwesomeTypeMessage>.

The Messenger and Receiver automatically unregister themselves in their destructors,
so no messages will be sent to objects that are out of scope. To do this, the Messenger
and Receiver maintain a circular reference to each other.

When a Messenger or Receiver is copied, the network of Receiver/Messenger relationships
in the copied object is rebuilt in the assigned object.

Clang doesn't like the multiple append()/deliver()/remove() function declarations
that result from multiply-inheriting from Messenger<T>. I wouldn't mind if Clang
recognized the separate signatures of multiply inherited append()/remove()/deliver()
methods, but using composition is a good option and may help your code be more readable.

Typical use, with Message template:
typedef MessageT<class Whatevs> WhatevsMessage;
class Messenger : public Messenger<WhatevsMessage>
{ ...
  deliver( Message( *this, "change" ) );
}
class Receiver : public Receiver<WhatevsMessage>
{ ...
  void receive( const Message &msg )
  {
  	auto info = msg.getSubject().getWhatevaInformation();
  }
}

enum Event{ CHANGE, VANISH };
typedef MessageT<WhatevsMessage> Message;
Messenger<Message> messenger;
Receiver<Message> receiver;
shared_ptr<Receiver<Message>> r_ptr;
...
messenger.appendReceiver( &receiver );
messenger.appendReceiver( r_ptr.get() );
...
// assuming this is an instance of Whatevs
messenger.deliver( WhatevsMessage( *this, CHANGE ) );
Receiver<Message>::receive( const Message &msg )
{
	if( msg.getEvent() == CHANGE ){
  	... do something, maybe with msg.getSubject();
  }
}
...
messenger.removeReceiver( &receiver );

*/

namespace cascade
{
	/**
	Messenger delivers messages of type T.
	Registers and unregisters itself from receivers as they are appended and removed.
	*/
	template <class T>
	class Messenger
	{
	public:
		typedef T Message;
		typedef Receiver<T> Receiver;
		Messenger() = default;
		//! Copies the Messenger/Receiver relationship of \a other
		Messenger( const Messenger &other ):
		mReceivers()
		{
			for( auto r : other.mReceivers )
			{
				appendReceiver( r );
			}
		}
		//! Copies the Messenger/Receiver relationship of \a rhs
		Messenger& operator=( const Messenger &rhs )
		{
			for( auto r : mReceivers )
			{
				r->unregisterMessenger( this );
			}
			mReceivers.clear();
			for( auto r : rhs.mReceivers )
			{
				appendReceiver( r );
			}
			return *this;
		}
		//! Destructor unregisters self from all Receivers
		virtual ~Messenger()
		{
			for( auto r : mReceivers )
			{
				r->unregisterMessenger( this );
			}
		}
		//! Deliver \a message to all receivers.
		void deliver( const Message &message )
		{
			// deliver message
			mDelivering = true;
			for( auto receiver : mReceivers )
			{
				receiver->receive( message );
			}
			mDelivering = false;
			// remove anything that tried to be removed in loop
			for( auto receiver : mRemovalQueue )
			{
				removeReceiver( receiver );
			}
			mRemovalQueue.clear();
			for( auto receiver : mDestructedQueue )
			{
				removeDestructingReceiver( receiver );
			}
			mDestructedQueue.clear();
		}
		//! Start sending messages to \a receiver. Safe to call within deliver/receive loop.
		//! Will not receive events until the next deliver/receive loop.
		void appendReceiver( Receiver *receiver )
		{
			mReceivers.push_back( receiver );
			receiver->registerMessenger( this );
		}
		//! Stop sending messages to \a receiver. Safe to call within deliver/receive loop.
		void removeReceiver( Receiver *receiver )
		{
			if( ! mDelivering )
			{
				receiver->unregisterMessenger( this );
				mReceivers.erase( remove_if( mReceivers.begin()
											, mReceivers.end()
											, [=](Receiver *other)
											{ return other == receiver; } )
								 , mReceivers.end() );
			}
			else
			{
				mRemovalQueue.push_back( receiver );
			}
		}
	private:
		friend Receiver;
		//! Called by Receiver in its destructor
		void removeDestructingReceiver( const Receiver *receiver )
		{
			if( ! mDelivering )
			{
				mReceivers.erase( remove_if( mReceivers.begin()
											, mReceivers.end()
											, [=](Receiver *other)
											{ return other == receiver; } )
								 , mReceivers.end() );
			}
			else
			{
				mDestructedQueue.push_back( receiver );
			}
		}
		// All the active receivers
		std::vector<Receiver*> 			mReceivers;
		// Receivers marked for regular removal during deliver() loop
		std::vector<Receiver*>		 	mRemovalQueue;
		// Receivers marked for destructing remove during deliver() loop
		std::vector<const Receiver*> 	mDestructedQueue;
		// Are we currently delivering a message?
		bool							mDelivering = false;
	};
}
