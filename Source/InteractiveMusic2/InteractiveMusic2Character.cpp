// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractiveMusic2Character.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SpotLightComponent.h"

//////////////////////////////////////////////////////////////////////////
// AInteractiveMusic2Character

AInteractiveMusic2Character::AInteractiveMusic2Character() :
	isSpotlightOn(false), WalkSpeed(150.0f),
	WalkAcceleration(300.0f), RunSpeed(600.0f),	RunAcceleration(1000.0f),
	DashSpeed(900.0f), DashAcceleration(1500.0f),
	isDashTriggered(false), isWalkTriggered(false),
	movementState(MovementSate::Run), isDashCompleted(true),
	isWalkCompleted(true), isSwitchSpotlightCompleted(true)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	GetCharacterMovement()->MaxAcceleration = RunAcceleration;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a AkComponent
	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);

	akGameObjID = new AkGameObjectID(AkComponent->GetAkGameObjectID());
	
	AK::SoundEngine::RegisterGameObj(*akGameObjID, "Player");
	AK::SoundEngine::SetDefaultListeners(akGameObjID, 1);

	bShouldUpdateAkPosition = true;

	// Create a spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	SpotLight->SetupAttachment(GetCapsuleComponent());
	SpotLight->SetRelativeLocation(FVector(15.0f, 3.0f, 80.0f));

	SpotLight->SetIntensity(0.0f);
	TargetIntensity = 0.0f;
	MaxIntensity = 300000.0f;
	
	isSpotlightOn = false;
	bShouldUpdateIntensity = false;

	IntensityInputIncrement = 80000.0f;
	IntensityUpdateIncrement = 50000.0f;
	
	// Spotlight Zoom
	MaxOuterConeAngle = 50.0f;
	MinOuterConeAngle = 15.0f;
	
	OuterConeAngleInputIncrement = 10.0f;
	OuterConeAngleUpdateIncrement = 5.0f;

	TargetOuterConeAngle = 40.0f;
	SpotLight->SetOuterConeAngle(TargetOuterConeAngle);
	bShouldUpdateOuterConeAngle = false;

	// Camera Zoom
	bShouldUpdateTargetArmLength = false;
	MaxTargetArmLength = 450.0f;
	MinTargetArmLength = 200.0f;

	TargetArmLengthInputIncrement = 50.0f;
	TargetArmLengthUpdateIncrement = 8.0f;

	TargetTargetArmLength = 400.0f;
	CameraBoom->TargetArmLength = TargetTargetArmLength;

	// Mouse Wheel Press Event
	isMouseWheelPressed = false;

	// Enable Tick
	this->SetActorTickEnabled(true);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AInteractiveMusic2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AInteractiveMusic2Character::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	if (bShouldUpdateIntensity)
	{
		UpdateIntensityInTick();
	}

	if (bShouldUpdateOuterConeAngle)
	{
		UpdateOuterConeAngleInTick();
	}

	if (bShouldUpdateTargetArmLength)
	{
		UpdateTargetArmLengthInTick();
	}

	if (bShouldUpdateAkPosition)
	{
		bShouldUpdateAkPosition = false;
		const auto uePosition = GetActorLocation();
		akSoundPosition.SetPosition(uePosition.Y, uePosition.Z, uePosition.X);
		AK::SoundEngine::SetPosition(*akGameObjID, akSoundPosition);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AInteractiveMusic2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::Look);

		//Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::Dash);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AInteractiveMusic2Character::OnDashCompleted);

		//Walking
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::Walk);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &AInteractiveMusic2Character::OnWalkCompleted);

		//Switch Spotlight
		EnhancedInputComponent->BindAction(SwitchSpotlightAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnSwitchSpotlightTriggered);
		EnhancedInputComponent->BindAction(SwitchSpotlightAction, ETriggerEvent::Completed, this, &AInteractiveMusic2Character::OnSwitchSpotlightCompleted);

		//Spotlight Zoom
		// EnhancedInputComponent->BindAction(SpotlightZoomInAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnSpotlightZoomInTriggered);
		// EnhancedInputComponent->BindAction(SpotlightZoomOutAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnSpotlightZoomOutTriggered);

		//Camera Zoom
		EnhancedInputComponent->BindAction(CameraZoomInAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnCameraZoomInTriggered);
		EnhancedInputComponent->BindAction(CameraZoomOutAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnCameraZoomOutTriggered);

		//Mouse Wheel Pressed
		EnhancedInputComponent->BindAction(MouseWheelPressAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnMouseWheelPressTriggered);
		EnhancedInputComponent->BindAction(MouseWheelReleaseAction, ETriggerEvent::Triggered, this, &AInteractiveMusic2Character::OnMouseWheelReleaseTriggered);
	}

}

void AInteractiveMusic2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AInteractiveMusic2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AInteractiveMusic2Character::Dash(const FInputActionValue& Value)
{
	if (isDashCompleted)
	{
		isDashCompleted = false;
		isDashTriggered = true;
		UpdateMovementState(movementState);
	}
}

void AInteractiveMusic2Character::Walk(const FInputActionValue& Value)
{
	if (isWalkCompleted)
	{
		isWalkCompleted = false;
		isWalkTriggered = true;
		UpdateMovementState(movementState);
	}
}

void AInteractiveMusic2Character::OnWalkCompleted(const FInputActionValue& Value)
{
	isWalkCompleted = true;
}

void AInteractiveMusic2Character::OnSwitchSpotlightTriggered(const FInputActionValue& Value)
{
	if (isSwitchSpotlightCompleted)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Spotlight Triggered"))		
		isSwitchSpotlightCompleted = false;
		isSpotlightOn = !isSpotlightOn;
		TargetIntensity = isSpotlightOn ? CalcTargetIntensity() : 0.0f;
		bShouldUpdateIntensity = true;
		// UE_LOG(LogTemp, Warning, TEXT("TargetIntensity:%.2f"), TargetIntensity)	
	}
}

void AInteractiveMusic2Character::OnSwitchSpotlightCompleted(const FInputActionValue& Value)
{
	isSwitchSpotlightCompleted = true;
}

void AInteractiveMusic2Character::OnDashCompleted(const FInputActionValue& Value)
{
	isDashCompleted = true;
}

float AInteractiveMusic2Character::CalcTargetIntensity() const
{
	constexpr float minIntensityRatio = 0.1f;
	const float ratio = ((MaxOuterConeAngle - SpotLight->OuterConeAngle)/(MaxOuterConeAngle - MinOuterConeAngle))
	* (1.0f - minIntensityRatio) + minIntensityRatio;
	return MaxIntensity * ratio;
}

void AInteractiveMusic2Character::UpdateIntensityInTick()
{
	// UE_LOG(LogTemp, Warning, TEXT("Current: %.2f\tTarget: %.2f"), SpotLight->Intensity, TargetIntensity)
	if (fabs(SpotLight->Intensity - TargetIntensity) <= IntensityUpdateIncrement)
	{
		SpotLight->SetIntensity(TargetIntensity);
		bShouldUpdateIntensity = false;
	}
	else if (SpotLight->Intensity > TargetIntensity)
	{
		SpotLight->SetIntensity(SpotLight->Intensity - IntensityUpdateIncrement);
	}
	else if (SpotLight->Intensity < TargetIntensity)
	{
		SpotLight->SetIntensity(SpotLight->Intensity + IntensityUpdateIncrement);
	}
	SpotLight->SetAttenuationRadius(10*SpotLight->Intensity);
}

void AInteractiveMusic2Character::UpdateOuterConeAngleInTick()
{
	if (fabs(SpotLight->OuterConeAngle-TargetOuterConeAngle) <= OuterConeAngleUpdateIncrement)
	{
		SpotLight->SetOuterConeAngle(TargetOuterConeAngle);
		bShouldUpdateOuterConeAngle = false;
	}
	else if (SpotLight->OuterConeAngle > TargetOuterConeAngle)
	{
		SpotLight->SetOuterConeAngle(SpotLight->OuterConeAngle-OuterConeAngleUpdateIncrement);
	}
	else if (SpotLight->OuterConeAngle < TargetOuterConeAngle)
	{
		SpotLight->SetOuterConeAngle(SpotLight->OuterConeAngle+OuterConeAngleUpdateIncrement);
	}
}

void AInteractiveMusic2Character::OnSpotlightZoomInTriggered(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Spotlight zoom in Triggered"))
	// UE_LOG(LogTemp, Warning, TEXT("SpotLightAngle: %.2f"), SpotLight->OuterConeAngle)
	if (isSpotlightOn && SpotLight->OuterConeAngle < MaxOuterConeAngle)
	{
		TargetOuterConeAngle = std::min(TargetOuterConeAngle+OuterConeAngleInputIncrement, MaxOuterConeAngle);
		bShouldUpdateOuterConeAngle = true;

		TargetIntensity = CalcTargetIntensity();
		bShouldUpdateIntensity = true;
	}
}

void AInteractiveMusic2Character::OnSpotlightZoomOutTriggered(const FInputActionValue& Value)
{
	if (isSpotlightOn && SpotLight->OuterConeAngle > MinOuterConeAngle)
	{
		TargetOuterConeAngle = std::max(TargetOuterConeAngle-OuterConeAngleInputIncrement, MinOuterConeAngle);
		bShouldUpdateOuterConeAngle = true;

		TargetIntensity = CalcTargetIntensity();
		bShouldUpdateIntensity = true;
	}
}

void AInteractiveMusic2Character::UpdateTargetArmLengthInTick()
{
	if (fabs(CameraBoom->TargetArmLength-TargetTargetArmLength) <= TargetArmLengthUpdateIncrement)
	{
		bShouldUpdateTargetArmLength = false;
		CameraBoom->TargetArmLength = TargetTargetArmLength;
	}
	else if (CameraBoom->TargetArmLength < TargetTargetArmLength)
	{
		CameraBoom->TargetArmLength += TargetArmLengthUpdateIncrement;
	}
	else if (CameraBoom->TargetArmLength > TargetTargetArmLength)
	{
		CameraBoom->TargetArmLength -= TargetArmLengthUpdateIncrement;
	}
}

void AInteractiveMusic2Character::OnCameraZoomInTriggered(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Camera zoom in Triggered"))
	if (!isMouseWheelPressed) {
		// UE_LOG(LogTemp, Warning, TEXT("Camera zoom in Update"))
		if (CameraBoom->TargetArmLength > MinTargetArmLength)
		{
			TargetTargetArmLength = std::max(TargetTargetArmLength-TargetArmLengthInputIncrement, MinTargetArmLength);
			bShouldUpdateTargetArmLength = true;
		}
	}
	else
	{
		OnSpotlightZoomInTriggered(Value);
	}
}

void AInteractiveMusic2Character::OnCameraZoomOutTriggered(const FInputActionValue& Value)
{
	if (!isMouseWheelPressed) {
		if (CameraBoom->TargetArmLength < MaxTargetArmLength)
		{
			TargetTargetArmLength = std::min(TargetTargetArmLength+TargetArmLengthInputIncrement, MaxTargetArmLength);
			bShouldUpdateTargetArmLength = true;
		}
	}
	else
	{
		OnSpotlightZoomOutTriggered(Value);
	}
}

void AInteractiveMusic2Character::OnMouseWheelPressTriggered(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Pressed"))
	isMouseWheelPressed = true;
}

void AInteractiveMusic2Character::OnMouseWheelReleaseTriggered(const FInputActionValue& Value)
{
	// UE_LOG(LogTemp, Warning, TEXT("Released"))
	isMouseWheelPressed = false;
}

void AInteractiveMusic2Character::UpdateMovementState(MovementSate& currState)
{
	/*UE_LOG(LogTemp, Warning, TEXT("CurrState:%hs\tisWalkTriggered=%hs\tisDashTriggered=%hs\n"),
		(currState==MovementSate::Walk?"Walk":(currState==MovementSate::Run?"Run":"Dash")),
		isWalkTriggered?"True":"False",
		isDashTriggered?"True":"False")*/
	switch (currState)
	{
	case MovementSate::Walk:
		if (isWalkTriggered)
		{
			isWalkTriggered = false;
			currState = MovementSate::Run;
			UpdateMovementParam(currState);
		}
		else if (isDashTriggered)
		{
			isDashTriggered = false;
			currState = MovementSate::Dash;
			UpdateMovementParam(currState);
		}
		break;
	case MovementSate::Run:
		if (isWalkTriggered)
		{
			isWalkTriggered = false;
			currState = MovementSate::Walk;
			UpdateMovementParam(currState);
		}
		else if (isDashTriggered)
		{
			isDashTriggered = false;
			currState = MovementSate::Dash;
			UpdateMovementParam(currState);
		}
		break;
	case MovementSate::Dash:
		if (isWalkTriggered)
		{
			isWalkTriggered = false;
			currState = MovementSate::Walk;
			UpdateMovementParam(currState);
		}
		else if (isDashTriggered)
		{
			isDashTriggered = false;
			currState = MovementSate::Run;
			UpdateMovementParam(currState);
		}
		break;
	}
}

void AInteractiveMusic2Character::UpdateMovementParam(const MovementSate& currState) const
{
	switch (currState)
	{
	case MovementSate::Walk:
		this->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		this->GetCharacterMovement()->MaxAcceleration = WalkAcceleration;
		break;
	case MovementSate::Run:
		this->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		this->GetCharacterMovement()->MaxAcceleration = RunAcceleration;
		break;
	case MovementSate::Dash:
		this->GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
		this->GetCharacterMovement()->MaxAcceleration = DashAcceleration;
		break;
	}
}
