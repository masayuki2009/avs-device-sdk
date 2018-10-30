/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on PortAudioMicrophoneWrapper.h
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

#ifndef ALEXA_CLIENT_SDK_SAMPLEAPP_INCLUDE_SAMPLEAPP_NXMICWRAPPER_H_
#define ALEXA_CLIENT_SDK_SAMPLEAPP_INCLUDE_SAMPLEAPP_NXMICWRAPPER_H_

#include <AVSCommon/AVS/AudioInputStream.h>
#include <SampleApp/ConsolePrinter.h>

namespace alexaClientSDK {
namespace sampleApp {

class NxMicWrapper {
public:
    class NxMicWrapperOutputDataSource {
    public:
        void setWriter(std::shared_ptr<avsCommon::avs::AudioInputStream::Writer> writer) {
            m_writer = writer;
        }

        ssize_t write(unsigned char* buf, size_t size) {
            ssize_t ret = m_writer->write(buf, size);
            if (ret <= 0) {
                ConsolePrinter::simplePrint("Failed to write to stream.");
                return 0;
            }

            return ret;
        }

    private:
        std::shared_ptr<avsCommon::avs::AudioInputStream::Writer> m_writer;
    };
public:

    static std::unique_ptr<NxMicWrapper> create(std::shared_ptr<avsCommon::avs::AudioInputStream> stream);

    bool startStreamingMicrophoneData();
    bool stopStreamingMicrophoneData();

    ~NxMicWrapper();

private:

    NxMicWrapper(std::shared_ptr<avsCommon::avs::AudioInputStream> stream);

    static int NxMicWrapperCallback(
        const void* inputBuffer,
        void* outputBuffer,
        unsigned long numSamples,
        void* userData);

    bool initialize();

    const std::shared_ptr<avsCommon::avs::AudioInputStream> m_audioInputStream;

    std::shared_ptr<avsCommon::avs::AudioInputStream::Writer> m_writer;
};

}  // namespace sampleApp
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_SAMPLEAPP_INCLUDE_SAMPLEAPP_NXMICWRAPPER_H_
