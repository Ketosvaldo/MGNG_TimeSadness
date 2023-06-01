// Fill out your copyright notice in the Description page of Project Settings.


#include "WallToJump.h"
#include "MGNG_TimeSadnessNPC.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AWallToJump::AWallToJump()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OverlapEnd);
	RootComponent = BoxCollider;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	MeshComponent->SetupAttachment(BoxCollider);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	ArrowComponent->SetupAttachment(MeshComponent);

	velocityZ = 10.0f;
}

// Called when the game starts or when spawned
void AWallToJump::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWallToJump::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMGNG_TimeSadnessNPC* Character = Cast<AMGNG_TimeSadnessNPC>(OtherActor);

	if(Character != nullptr)
	{
		UCharacterMovementComponent* CharMove = Character->GetCharacterMovement();
		CharMove->GravityScale = 0;
		CharMove->Velocity = FVector(CharMove->Velocity.X,CharMove->Velocity.Y,-velocityZ);
		Character->bIsWall = true;
		Character->DirectionToJump = ArrowComponent->GetComponentRotation().Vector();
		Character->DirectionToJump.Normalize();
	}
}

void AWallToJump::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMGNG_TimeSadnessNPC* Character = Cast<AMGNG_TimeSadnessNPC>(OtherActor);
	if(Character != nullptr)
	{
		Character->bIsWall = false;
		Character->GetCharacterMovement()->GravityScale = 1;
	}
}

// Called every frame
void AWallToJump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

