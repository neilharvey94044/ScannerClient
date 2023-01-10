// Copyright (c) Neil D. Harvey

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>


namespace sc{
class MediaDescription {
    public:
        // create MediaDescription objects with this method
        static std::unique_ptr<MediaDescription> parse(std::stringstream& sresponse);

        // RTP media types (all we need is audio)
        enum media_type {
            audio,
            video,
            text,
            application,
            message
        };

        // RTP code values for media formats (all we need is PCMU as supported by the scanner)
        enum media_format {
            PCMU = 0,             // Pulse Code Modulation (U law)
            PCMA = 8              // PUlse Code Modulation (A law)

        };
        enum MediaDescription::media_type getMediaType() const;
        enum MediaDescription::media_format getMediaFormat() const;
        std::string getAudioChannel() const;

    private:
        media_type m_media_type;
        media_format m_media_format;
        std::string m_transport_protocol{""};
        std::string m_audio_channel{""};
        std::unordered_multimap<char, std::string> m_md_value;

};




}