#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>


//! Queue with support for the thread safe operations.
template <class T>
class Tsqueue{
	std::queue<T> queue_; /*!< STL queue for storing a data. */

	mutable std::mutex lock_; /*! Lock which is used for synchronization between access to the STL queue. */
	std::condition_variable emptyCondition_; /*! Wait condition for pop function. */
public:
	//! Return true when queue is empty and false otherwise.
	bool empty() const{ 
		std::lock_guard<std::mutex> lck(lock_); //! Acquire lock for safe access to the STL queue
		return queue_.empty();
	}

	//! Returns first element of queue.
	/*! Function call is blocking. 
		\return first element of queue.
	*/
	T pop(){
		std::unique_lock<std::mutex> lck(lock_); //! Create unique_lock which we use for waiting on the empty condition.

		emptyCondition_.wait(lck,[this]{return !queue_.empty();}); // Wait until queue is not empty.

		T element = queue_.front();
		queue_.pop();

		return element; // Pop first element of the queue.
	}

	//! Returns first element of queue.
	/*! Function call is blocking. 
		\return first element of queue.
	*/
	bool tryPop(T& element){
		std::lock_guard<std::mutex> lck(lock_); //! Acquire lock for safe access to the STL queue

		if(queue_.empty()) // ! Test emptiness of queue
			return false;

		element = std::move(queue_.front()); // Add first element from queue to the reference parameter
		queue_.pop();

		return true;
	}

	//! Push new element to the queue.
	void push(T value){
		std::lock_guard<std::mutex> lck(lock_); //! Acquire lock for safe access to the STL queue
		queue_.push(value); // Push new element to the queue.
		emptyCondition_.notify_one(); // Wakes up one waiting process on the empty condition.
	}
};

#endif