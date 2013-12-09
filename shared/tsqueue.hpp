#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template <class T>
class tsqueue{
	std::queue<T> queue;

	mutable std::mutex lock;
	std::condition_variable emptyCondition;
public:
	bool empty() const{ 
		std::lock_guard<std::mutex> lck(lock);
		return queue.empty();
	}

	std::shared_ptr<T> pop(){
		std::unique_lock<std::mutex> lck(lock);

		emptyCondition.wait(lck,[this]{return !queue.empty();});

		std::shared_ptr<T> pointer = std::shared_ptr<T>(std::make_shared<T>(queue.front()));
		queue.pop();

		return pointer;
	}

	std::shared_ptr<T> tryPop(){
		std::lock_guard<std::mutex> lck(lock);

		if(queue.empty())
			return std::shared_ptr<T>();

		std::shared_ptr<T> pointer = std::shared_ptr<T>(std::make_shared<T>(queue.front()));
		queue.pop();

		return pointer;
	};

	void push(T value){
		std::lock_guard<std::mutex> lck(lock);
		queue.push(value);
		emptyCondition.notify_one();
	}
};

#endif