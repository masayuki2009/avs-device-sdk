/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on PortAudioMicrophoneWrapper.cpp
 *  Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <AVSCommon/Utils/Logger/Logger.h>

#include "SampleApp/NxMicWrapper.h"

extern "C"
{
  extern int g_tap_count;

  typedef int tinycap_callback(
    const void *input, void *output,
    unsigned long frameCount,
    void *userData );

  void tinycap_register_callback(tinycap_callback *f, void *data);
}

namespace alexaClientSDK {
namespace sampleApp {

using avsCommon::avs::AudioInputStream;


static const std::string TAG("NxMicWrapper");
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)


std::unique_ptr<NxMicWrapper> NxMicWrapper::create(
    std::shared_ptr<AudioInputStream> stream) {
    if (!stream) {
        ConsolePrinter::simplePrint("Invalid stream passed to NxMicWrapper");
        return nullptr;
    }
    std::unique_ptr<NxMicWrapper> voiceRecorder(new NxMicWrapper(stream));
    if (!voiceRecorder->initialize()) {
        ConsolePrinter::simplePrint("Failed to initialize NxMicWrapper");
        return nullptr;
    }
    return voiceRecorder;
}

NxMicWrapper::NxMicWrapper(std::shared_ptr<AudioInputStream> stream) :
    m_audioInputStream{stream} {
}

NxMicWrapper::~NxMicWrapper() {

}

bool NxMicWrapper::initialize() {
    m_writer = m_audioInputStream->createWriter(AudioInputStream::Writer::Policy::BLOCKING);
    if (!m_writer) {
        ConsolePrinter::simplePrint("Failed to create stream writer");
        return false;
    }

    tinycap_register_callback(NxMicWrapperCallback, this);
    return true;
}

bool NxMicWrapper::startStreamingMicrophoneData() {
    ACSDK_DEBUG9(LX("startStreamingMicCalled"));
    return true;
}

bool NxMicWrapper::stopStreamingMicrophoneData() {
    ACSDK_DEBUG9(LX("stopStreamingMicCalled"));
    return true;
}


#define DUMMY_VOICE "/mnt/sd1/recognize_weather_test1.wav"

int NxMicWrapper::NxMicWrapperCallback(
    const void* inputBuffer,
    void* outputBuffer,
    unsigned long numSamples,
    void* userData) {

    NxMicWrapper* wrapper = static_cast<NxMicWrapper*>(userData);

#ifdef USE_DUMMY_VOICE
    static int g_fd;
    int length = numSamples * 2;
    void *tmp = ::malloc(length);
    assert(tmp);
    memset(tmp, 0, length);

    if (1 == (g_tap_count % 2))
      {
        if (0 == g_fd)
          {
            g_fd = ::open(DUMMY_VOICE, O_RDONLY);
            assert(g_fd);
            ::lseek(g_fd, 16 + 4044, SEEK_SET);
          }

        ssize_t ret = ::read(g_fd, tmp, length);
        ACSDK_DEBUG9(LX("callback ******").d("numSamples", numSamples));

        if (ret == 0)
          {
            ::close(g_fd);
            g_fd = 0;
            g_tap_count++;
          }
      }

    (void)wrapper->m_writer->write(tmp, numSamples);
    ::free(tmp);
#else
    (void)wrapper->m_writer->write(inputBuffer, numSamples);
#endif
    return 0;
}

}  // namespace sampleApp
}  // namespace alexaClientSDK
