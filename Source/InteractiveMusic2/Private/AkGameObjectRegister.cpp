// Fill out your copyright notice in the Description page of Project Settings.


#include "AkGameObjectRegister.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>

#include "AkAmbientSound.h"
#include "AkComponent.h"

// Sets default values for this component's properties
UAkGameObjectRegister::UAkGameObjectRegister()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAkGameObjectRegister::BeginPlay()
{
	Super::BeginPlay();

	// ...
	const auto owner = GetOwner();
	if (auto AkAmbientSound = Cast<AAkAmbientSound>(owner))
	{
		if (auto AkComponent = Cast<UAkComponent>(owner->GetComponentByClass(UAkComponent::StaticClass())))
		{
			AK::SoundEngine::RegisterGameObj(AkComponent->GetAkGameObjectID());	
		}
	}
}


// Called every frame
void UAkGameObjectRegister::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

