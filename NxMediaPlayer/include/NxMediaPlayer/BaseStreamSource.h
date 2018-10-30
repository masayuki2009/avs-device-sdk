/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on BaseStreamSource.h
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

#ifndef ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_BASESTREAMSOURCE_H_
#define ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_BASESTREAMSOURCE_H_

#include <AVSCommon/Utils/MediaPlayer/MediaPlayerInterface.h>
#include "NxMediaPlayer/SourceInterface.h"

namespace alexaClientSDK {
namespace mediaPlayer {

class BaseStreamSource : public SourceInterface {
public:
    BaseStreamSource(const std::string& className);
    ~BaseStreamSource() override;

protected:
    bool init(const avsCommon::utils::AudioFormat* audioFormat = nullptr);
    void installOnReadDataHandler() override;

private:

    static ssize_t onReadData(void *buffer, size_t size, void *data);
    virtual ssize_t readData(void *buffer, size_t size) = 0;

};

}  // namespace mediaPlayer
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_NXMEDIAPLAYER_INCLUDE_NXMEDIAPLAYER_BASESTREAMSOURCE_H_
