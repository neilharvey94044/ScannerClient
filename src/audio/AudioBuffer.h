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
#include <samplerate.h>

namespace sc {


constexpr int SC_SHORT_BUF_SIZE{320};
constexpr int SC_FLOAT_BUF_SIZE{1764};
constexpr float SC_SRC_RATIO = 44100.0/8000.0;

// The AudioBuffer recieves 320 samples per datagram from the scanner.  These have been
// decompanded into short values from mu-law byte values.  Mu-law is an 8000 samples per second standard.  
// Unfortunately not all platforms (e.g. Apple) have audio devices that support this sample rate, thus we resample up by
// a factor of 44,100/8000 using "libsamplerate".
// 	320 frames, 1 channel
// 	44100/8000 = 5.5125
// 	5.5125 * 320 = 1764 samples
typedef std::array<unsigned short, SC_SHORT_BUF_SIZE> rtpbuf;
typedef std::array<float, SC_FLOAT_BUF_SIZE> float_buf;


class AudioBuffer {

	public:
	int audio_buf_pool_max{0};
	int audio_pipe_max{0};

	AudioBuffer(int starting_buffers = 5);

	// provides a buffer for populating with audio
	std::shared_ptr<rtpbuf> getBuf();

	// puts a buffer containing audio into the pipeline for playing
	void pushAudio(std::shared_ptr<rtpbuf> buf_ptr);

	// writes the next audio buffer to buf_out for playing and recycles the buffer
	void getAudio(float_buf& buf_out);

	// flag to tell the audio callback loop to stop
	void setStopped(bool stopped);
	bool getStopped() const;

	void setChannels(int channels );
	int getChannels() const;
	~AudioBuffer();

	private:
	//std::deque<std::shared_ptr<rtpbuf>> buf_pipe{};
	std::vector<std::shared_ptr<rtpbuf>> rtp_buf_pool{};
	std::deque<std::shared_ptr<float_buf>> audio_pipe{};
	std::vector<std::shared_ptr<float_buf>> audio_buf_pool{};
	std::mutex rtp_buf_pool_mutex;
	std::mutex audio_pipe_mutex;
	std::mutex audio_buf_pool_mutex;
	std::atomic_bool m_stopped{false};
	std::atomic_int m_channels{0};

	SRC_STATE* m_samplerate; // handle to the libsamplerate object

};

} //namespace
