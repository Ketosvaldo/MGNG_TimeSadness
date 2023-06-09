// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/SphereComponent.h"
#include "MGNG_TimeSadnessNPC.generated.h"


UCLASS(config=Game)
class AMGNG_TimeSadnessNPC : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Wall Jump Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WallJumpAction;
  
	/** Slide Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlideAction;
  
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UCharacterMovementComponent* CharMove;

public:
	AMGNG_TimeSadnessNPC();
	
	bool bIsWall;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsWallJumping;
	bool bCanRoll;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSafeLand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSliding;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsRag;
	
	FVector DirectionToJump;
	FRotator DirectionToLook;
	UPROPERTY(EditAnywhere)
	float Magnitude;
	float Counter;
	float JumpCounter;
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void WallJump();

	void CheckJump();
	
	void Slide();
	
	void ResetBools();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};

