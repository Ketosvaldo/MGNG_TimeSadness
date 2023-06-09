// Copyright Epic Games, Inc. All Rights Reserved.

#include "MGNG_TimeSadnessNPC.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AExamen2MGNGCharacter

AMGNG_TimeSadnessNPC::AMGNG_TimeSadnessNPC()
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
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
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

	bIsWall = false;
	bCanRoll = false;
	bSafeLand = false;
	bIsSliding = false;
	
	CharMove = GetCharacterMovement();
	Magnitude = 1;
	Counter = 0;
	JumpCounter = 0;
}

void AMGNG_TimeSadnessNPC::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMGNG_TimeSadnessNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(bIsRag)
		return;
	JumpCounter += DeltaSeconds;
	if(!CharMove->IsFalling() && bCanRoll && bSafeLand)
	{
		Counter += DeltaSeconds;
		/*if(JumpCounter > 0.4f)
			GetCharacterMovement()->AddImpulse(DirectionToJump * Magnitude);*/
		if(Counter > 0.88f)
			bIsWallJumping = false;
		if(Counter > 1.17f)
			ResetBools();
		return;
	}
	//Condición para morir
	if(!CharMove->IsFalling() && bCanRoll && !bSafeLand)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Me muero")));
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true);
		bIsRag = true;
		return;
	}
	if(!CharMove->IsFalling())
	{
		bIsWallJumping = false;
		if(bIsSliding)
		{
			CharMove->MaxWalkSpeed = 1000;
			CharMove->Velocity = GetCapsuleComponent()->GetForwardVector() * 1000.f;
			Counter += DeltaSeconds;
		}
		if(Counter > 1.17f && bIsSliding)
		{
			bIsSliding = false;
			GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);
			USkeletalMeshComponent* MeshComp = GetMesh();
			UCapsuleComponent* Caps = GetCapsuleComponent();

			CharMove->MaxWalkSpeed = 700.f;
			float xLocation = Caps->GetComponentLocation().X;
			float yLocation = Caps->GetComponentLocation().Y;
			float zLocation = Caps->GetComponentLocation().Z;
			
			MeshComp->SetRelativeLocation(FVector(0,0,MeshComp->GetRelativeLocation().Z - 50.0f));
			Caps->SetWorldLocation(FVector(xLocation,yLocation,zLocation + 50.0f));
			return;
		}
	}
	if(CharMove->Velocity.Z < -900)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Ruedo")));
		bCanRoll = true;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMGNG_TimeSadnessNPC::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMGNG_TimeSadnessNPC::CheckJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMGNG_TimeSadnessNPC::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMGNG_TimeSadnessNPC::Look);

		//WallJump
		EnhancedInputComponent->BindAction(WallJumpAction, ETriggerEvent::Started, this, &AMGNG_TimeSadnessNPC::WallJump);

		//Slide
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AMGNG_TimeSadnessNPC::Slide);
	}

}

void AMGNG_TimeSadnessNPC::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr && !bIsSliding)
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

void AMGNG_TimeSadnessNPC::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMGNG_TimeSadnessNPC::WallJump()
{
	if(bIsWall)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString(TEXT("Si funciona")));
		GetCharacterMovement()->AddImpulse(DirectionToJump * Magnitude);
		SetActorRotation(FRotator(GetActorRotation().Pitch,GetActorRotation().Yaw + 180,GetActorRotation().Roll));
		bIsWallJumping = true;
		JumpCounter = 0;
	}

	if(bCanRoll)
	{
		Counter = 0;
		bSafeLand = true;
	}
}

void AMGNG_TimeSadnessNPC::CheckJump()
{
	if(!bIsSliding)
	{
		Cast<ACharacter>(this)->Jump();
	}
}

void AMGNG_TimeSadnessNPC::Slide()
{
	if(!bIsSliding && !CharMove->IsFalling())
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
	
		UCapsuleComponent* caps = GetCapsuleComponent();
	
		float xLocation = caps->GetComponentLocation().X;
		float yLocation = caps->GetComponentLocation().Y;
		float zLocation = caps->GetComponentLocation().Z;
	
		GetCapsuleComponent()->SetCapsuleHalfHeight(42.f);
		MeshComp->SetRelativeLocation(FVector(0,0,MeshComp->GetRelativeLocation().Z + 50.0f));
		GetCapsuleComponent()->SetWorldLocation(FVector(xLocation,yLocation,zLocation - 50.0f));
		Counter = 0;
		bIsSliding = true;
	}
}

void AMGNG_TimeSadnessNPC::ResetBools()
{
	bCanRoll = false;
	bSafeLand = false;
}
