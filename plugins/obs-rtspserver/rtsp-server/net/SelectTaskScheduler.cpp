// PHZ
// 2018-5-15

#include "SelectTaskScheduler.h"
#include "Timer.h"
#include <cstring>
#include <forward_list>

using namespace xop;

#define SELECT_CTL_ADD 0
#define SELECT_CTL_MOD 1
#define SELECT_CTL_DEL 2

SelectTaskScheduler::SelectTaskScheduler(const int id) : TaskScheduler(id)
{
	FD_ZERO(&fd_read_backup_);
	FD_ZERO(&fd_write_backup_);
	FD_ZERO(&fd_exp_backup_);

	this->SelectTaskScheduler::UpdateChannel(wakeup_channel_);
}

SelectTaskScheduler::~SelectTaskScheduler() = default;

void SelectTaskScheduler::UpdateChannel(const ChannelPtr &channel)
{
	std::lock_guard lock(mutex_);

	if (SOCKET socket = channel->GetSocket();
	    channels_.find(socket) != channels_.end()) {
		if (channel->IsNoneEvent()) {
			is_fd_read_reset_ = true;
			is_fd_write_reset_ = true;
			is_fd_exp_reset_ = true;
			channels_.erase(socket);
		} else {
			//is_fd_read_reset_ = true;
			is_fd_write_reset_ = true;
		}
	} else {
		if (!channel->IsNoneEvent()) {
			channels_.emplace(socket, channel);
			is_fd_read_reset_ = true;
			is_fd_write_reset_ = true;
			is_fd_exp_reset_ = true;
		}
	}
}

void SelectTaskScheduler::RemoveChannel(const ChannelPtr &channel)
{
	std::lock_guard lock(mutex_);

	if (const SOCKET fd = channel->GetSocket();
	    channels_.find(fd) != channels_.end()) {
		is_fd_read_reset_ = true;
		is_fd_write_reset_ = true;
		is_fd_exp_reset_ = true;
		channels_.erase(fd);
	}
}

bool SelectTaskScheduler::HandleEvent(int timeout)
{
	if (channels_.empty()) {
		if (timeout <= 0) {
			timeout = 10;
		}

		Timer::Sleep(timeout);
		return true;
	}

	fd_set fd_read{};
	fd_set fd_write{};
	fd_set fd_exp{};
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_exp);
	bool fd_read_reset = false;
	bool fd_write_reset = false;
	bool fd_exp_reset = false;

	if (is_fd_read_reset_ || is_fd_write_reset_ || is_fd_exp_reset_) {
		if (is_fd_exp_reset_) {
			maxfd_ = 0;
		}

		std::lock_guard lock(mutex_);
		for (const auto & [fst, snd] : channels_) {
			const uint32_t events = snd->GetEvents();
			const SOCKET fd = snd->GetSocket();

			if (is_fd_read_reset_ && (events & EVENT_IN)) {
				FD_SET(fd, &fd_read);
			}

			if (is_fd_write_reset_ && (events & EVENT_OUT)) {
				FD_SET(fd, &fd_write);
			}

			if (is_fd_exp_reset_) {
				FD_SET(fd, &fd_exp);
				if (fd > maxfd_) {
					maxfd_ = fd;
				}
			}
		}

		fd_read_reset = is_fd_read_reset_;
		fd_write_reset = is_fd_write_reset_;
		fd_exp_reset = is_fd_exp_reset_;
		is_fd_read_reset_ = false;
		is_fd_write_reset_ = false;
		is_fd_exp_reset_ = false;
	}

	if (fd_read_reset) {
		FD_ZERO(&fd_read_backup_);
		memcpy(&fd_read_backup_, &fd_read, sizeof(fd_set));
	} else {
		memcpy(&fd_read, &fd_read_backup_, sizeof(fd_set));
	}

	if (fd_write_reset) {
		FD_ZERO(&fd_write_backup_);
		memcpy(&fd_write_backup_, &fd_write, sizeof(fd_set));
	} else {
		memcpy(&fd_write, &fd_write_backup_, sizeof(fd_set));
	}

	if (fd_exp_reset) {
		FD_ZERO(&fd_exp_backup_);
		memcpy(&fd_exp_backup_, &fd_exp, sizeof(fd_set));
	} else {
		memcpy(&fd_exp, &fd_exp_backup_, sizeof(fd_set));
	}

	if (timeout <= 0) {
		timeout = 10;
	}

#if defined(WIN32) || defined(_WIN32)
    const timeval tv = {timeout / 1000, timeout % 1000 * 1000};
#else
    timeval tv = {timeout / 1000, timeout % 1000 * 1000};
#endif
    const int ret = select(static_cast<int>(maxfd_) + 1, &fd_read,
			       &fd_write, &fd_exp, &tv);

	if (ret < 0) {
#if defined(WIN32) || defined(_WIN32)
#else
		if (errno == EINTR) {
			return true;
		}
#endif
		return false;
	}

	std::forward_list<std::pair<ChannelPtr, uint32_t>> event_list;
	if (ret > 0) {
		std::lock_guard lock(mutex_);
		for (const auto & [fst, snd] : channels_) {
			uint32_t events = 0;
			const SOCKET socket = snd->GetSocket();

			if (FD_ISSET(socket, &fd_read)) {
				events |= EVENT_IN;
			}

			if (FD_ISSET(socket, &fd_write)) {
				events |= EVENT_OUT;
			}

			if (FD_ISSET(socket, &fd_exp)) {
				events |= EVENT_HUP; // close
			}

			if (events != 0) {
				event_list.emplace_front(snd, events);
			}
		}
	}

	for (const auto & [fst, snd] : event_list) {
		fst->HandleEvent(snd);
	}

	return true;
}
