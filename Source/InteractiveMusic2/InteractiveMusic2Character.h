// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AkComponent.h"
#include "InteractiveMusic2Character.generated.h"


UCLASS(config=Game)
class AInteractiveMusic2Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Spotlight */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* SpotLight;
	
	/** Switch Spotlight Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchSpotlightAction;

	bool isSpotlightOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float MaxIntensity;

	float TargetIntensity;

	bool bShouldUpdateIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float IntensityInputIncrement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float IntensityUpdateIncrement;

	float CalcTargetIntensity() const;

	void UpdateIntensityInTick();

	/** Spotlight Zoom Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// class UInputAction* SpotlightZoomInAction;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	// class UInputAction* SpotlightZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float MaxOuterConeAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float MinOuterConeAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float OuterConeAngleInputIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light", meta = (AllowPrivateAccess = "true"))
	float OuterConeAngleUpdateIncrement;

	float TargetOuterConeAngle;

	bool bShouldUpdateOuterConeAngle;

	void UpdateOuterConeAngleInTick();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Walk Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float WalkAcceleration;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float RunAcceleration;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking", meta = (AllowPrivateAccess = "true"))
	float DashAcceleration;

	// MovementState DFA Begin
	bool isDashTriggered;
	bool isWalkTriggered;

	enum class MovementSate
	{
		Walk,
		Run,
		Dash,
	};

	MovementSate movementState;

	void UpdateMovementState(MovementSate& currState);

	void UpdateMovementParam(const MovementSate& currState) const;

	// MovementState DFA End

	// Camera Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CameraZoomInAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CameraZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MaxTargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MinTargetArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float TargetArmLengthInputIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float TargetArmLengthUpdateIncrement;

	float TargetTargetArmLength;

	bool bShouldUpdateTargetArmLength;

	void UpdateTargetArmLengthInTick();

	/** Mouse Wheel Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseWheelPressAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseWheelReleaseAction;

	bool isMouseWheelPressed;

	// Wwise
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audiokinetic", meta = (AllowPrivateAccess = "true"))
	class UAkComponent* AkComponent;

	const AkGameObjectID* akGameObjID;

	bool bShouldUpdateAkPosition;

	AkSoundPosition akSoundPosition;

public:
	AInteractiveMusic2Character();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for dashing input */
	void Dash(const FInputActionValue& Value);

	void OnDashCompleted(const FInputActionValue& Value);

	/** Called for walking input */
	void Walk(const FInputActionValue& Value);

	void OnWalkCompleted(const FInputActionValue& Value);

	// 防止触发过快
	bool isDashCompleted, isWalkCompleted;

	/** Called for switching spotlight action */
	bool isSwitchSpotlightCompleted;
	
	void OnSwitchSpotlightTriggered(const FInputActionValue& Value);

	void OnSwitchSpotlightCompleted(const FInputActionValue& Value);

	/** Called for spotlight zoom in input */
	void OnSpotlightZoomInTriggered(const FInputActionValue& Value);
	
	/** Called for spotlight zoom out input */
	void OnSpotlightZoomOutTriggered(const FInputActionValue& Value);

	/** Called for camera zoom in input */
	void OnCameraZoomInTriggered(const FInputActionValue& Value);
	
	/** Called for camera zoom out input */
	void OnCameraZoomOutTriggered(const FInputActionValue& Value);

	/** Called for mouse wheel pressed input */
	void OnMouseWheelPressTriggered(const FInputActionValue& Value);
	void OnMouseWheelReleaseTriggered(const FInputActionValue& Value);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context

protected:
	virtual void BeginPlay() override;

public:
	
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
