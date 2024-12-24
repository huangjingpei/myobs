#pragma once
#include <memory>
class GBSAudioWriter {

public:
    static std::shared_ptr<GBSAudioWriter> Create();
    virtual ~GBSAudioWriter() {};

    virtual void write(const uint8_t *data, size_t size) = 0;
    virtual size_t getQueueSize() = 0;

    virtual void setSampleRate(uint32_t rate) = 0;

    virtual void setChannels(uint8_t ch) = 0;
    
};
