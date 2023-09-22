/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include "Wwise/API/Platforms/IOS/IOSAPI.h"

#if defined(PLATFORM_TVOS) && PLATFORM_TVOS
#include "Wwise/API_2022_1/Platforms/TVOS/TVOSAPI_2022_1.h"
#elif defined(PLATFORM_IOS) && PLATFORM_IOS

class FWwisePlatformAPI_2022_1_IOS : public IWwisePlatformAPI
{
public:
	UE_NONCOPYABLE(FWwisePlatformAPI_2022_1_IOS);
	FWwisePlatformAPI_2022_1_IOS() = default;
	virtual ~FWwisePlatformAPI_2022_1_IOS() override {}

	/// Change the category and options of the app's AVAudioSession without restarting the entire Sound Engine.
	/// \remark
	/// As per Apple recommendations, the AVAudioSession will be deactivated and then reactivated. Therefore, 
	/// the primary output device must be reinitialized, which causes all audio to stop for a short period of time.
	/// For a seamless transition, call this API during moments of complete silence in your game.
	///
	/// \sa
	/// - \ref AkAudioSessionProperties
	virtual void ChangeAudioSessionProperties(
		const AkAudioSessionProperties &in_properties                 ///< New properties to apply to the app's AVAudioSession shared instance.
		) override;
};

using FWwisePlatformAPI = FWwisePlatformAPI_2022_1_IOS;

#endif
