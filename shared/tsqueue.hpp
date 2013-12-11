#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>


//! Queue with support for the thread safe operations.
template <class T>
class tsqueue{
	std::queue<T> queue; /*!< STL queue for storing a data. */

	mutable std::mutex lock; /*! Lock which is used for synchronization between access to the STL queue. */
	std::condition_variable emptyCondition; /*! Wait condition for pop function. */
public:
	//! Return true when queue is empty and false otherwise.
	bool empty() const{ 
		std::lock_guard<std::mutex> lck(lock); //! Acquire lock for safe access to the STL queue
		return queue.empty();
	}

	//! Returns pointer to the first element of queue.
	/*! Function call is blocking. 
		\return shared pointer to the first element of queue.
	*/
	std::shared_ptr<T> pop(){
		std::unique_lock<std::mutex> lck(lock); //! Create unique_lock which we use for waiting on the empty condition.

		emptyCondition.wait(lck,[this]{return !queue.empty();}); // Wait until queue is not empty.

		std::shared_ptr<T> pointer = std::shared_ptr<T>(std::make_shared<T>(queue.front())); // Creates shared pointer to the first element of the queue.
		queue.pop(); // Pop first element of the queue.

		return pointer;
	}

	//! Returns pointer to the first element from queue or null pointer if is queue is empty.
	/*! Function call is not blocking. 
		\return shared pointer to the first element of queue or null pointer if is queue is empty.
	*/
	std::shared_ptr<T> tryPop(){
		std::lock_guard<std::mutex> lck(lock); //! Acquire lock for safe access to the STL queue

		if(queue.empty()) // Test emptiness of the queue.
			return std::shared_ptr<T>(); // Return null shared pointer.

		std::shared_ptr<T> pointer = std::shared_ptr<T>(std::make_shared<T>(queue.front())); // Creates shared pointer to the first element of the queue.
		queue.pop(); // Pop first element of the queue.

		return pointer;
	};

	//! Push new element to the queue.
	void push(T value){
		std::lock_guard<std::mutex> lck(lock); //! Acquire lock for safe access to the STL queue
		queue.push(value); // Push new element to the queue.
		emptyCondition.notify_one(); // Wakes up one waiting process on the empty condition.
	}
};

#endif