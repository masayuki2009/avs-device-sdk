/*
 * Copyright 2018 Sony Video & Sound Products Inc.
 *
 * based on SourceInterface.h
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

#ifndef ALEXA_CLIENT_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_SOURCEINTERFACE_H_
#define ALEXA_CLIENT_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_SOURCEINTERFACE_H_

#include <AVSCommon/Utils/RequiresShutdown.h>

namespace alexaClientSDK {
namespace mediaPlayer {

class SourceInterface : public avsCommon::utils::RequiresShutdown {
public:
    SourceInterface(const std::string& className) : RequiresShutdown(className) {
    }

    virtual ~SourceInterface() = default;

    virtual ssize_t readData(void *buffer, size_t size) = 0;

    virtual void installOnReadDataHandler() = 0;
};

}  // namespace mediaPlayer
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_SOURCEINTERFACE_H_
