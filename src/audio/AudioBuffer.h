// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

// Provides buffering between receiving audio and playing audio.
// Create on the heap and provide a pointer to both the thread
// reading from the network and the audio callback thread.


#pragma once

#include <string>
#include <memory>
#include <array>
#include <deque>
#include <vector>
#include <mutex>
#include <atomic>

namespace sc {

typedef std::array<unsigned short, 320> rtpbuf;

class AudioBuffer {

	public:
	int pool_max{0};
	int pipe_max{0};

	AudioBuffer(int starting_buffers = 5);

	// provides a buffer for populating with audio
	std::shared_ptr<rtpbuf> getBuf();

	// puts a buffer containing audio into the pipeline for playing
	void pushAudio(std::shared_ptr<rtpbuf> buf_ptr);

	// writes the next audio buffer to buf_out for playing and recycles the buffer
	void getAudio(rtpbuf& buf_out);

	// flag to tell the audio callback loop to stop
	void setStopped(bool stopped);
	bool getStopped() const;

	void setChannels(int channels );
	int getChannels() const;

	private:
	std::deque<std::shared_ptr<rtpbuf>> buf_pipe{};
	std::vector<std::shared_ptr<rtpbuf>> buf_pool{};
	std::mutex pool_mutex;
	std::mutex pipe_mutex;
	std::atomic_bool m_stopped{false};
	std::atomic_int m_channels{0};

};

} //namespace
