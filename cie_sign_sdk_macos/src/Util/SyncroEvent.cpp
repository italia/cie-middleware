#include "SyncroEvent.h"

void auto_reset_event::set()
{
	{
		std::unique_lock<std::mutex> lock(m_);
		signaled_ = true;
	}

	cv_.notify_one();
}

void auto_reset_event::wait()
{
	std::unique_lock<std::mutex> lock(m_);
	while (!signaled_)
		cv_.wait(lock);
	signaled_ = false;
}
