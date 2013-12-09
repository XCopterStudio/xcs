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
	std::condition_variable waitEmpty;
public:
	bool empty() const{ 
		std::unique_lock<std::mutex> lck(lock);
		return queue.empty();
	}

	std::shared_ptr<T> pop(){
		std::unique_lock<std::mutex> lck(lock);

		waitEmpty.wait(lck,[this]{return !empty();});

		std::shared_ptr<T> pointer = std::make_shared<T>(queue.front());
		queue.pop();

		return pointer;
	}

	std::shared_ptr<T> tryPop(){
		std::unique_lock<std::mutex> lck(lock);

		std::shared_ptr<T> pointer = std::make_shared<T>(queue.front());
		queue.pop();

		return pointer;
	};

	void push(T value){
		std::unique_lock<std::mutex> lck(lock);
		queue.push(value);
		waitEmpty.notify_one();
	}
};

#endif