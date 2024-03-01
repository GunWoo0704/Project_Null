// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionBlueprintLibrary.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Blueprint/BlueprintSupport.h"
#include "ActionPluginRuntimeModule.h"
#include "CollisionDebugDrawingPublic.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimSequence.h"
#include "Animation/BuiltInAttributeTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UActionBlueprintLibrary::UActionBlueprintLibrary(class FObjectInitializer const & ObjectInitializer)
	: Super(ObjectInitializer)
{
}


FDelegateHandleBP UActionBlueprintLibrary::BindEventToOnSkeletalMeshBoneTransformsFinalized(USkeletalMeshComponent* SkeletalMeshComponent, const FOnSkeletalMeshBoneTransformsFinalized& Event)
{
	if(SkeletalMeshComponent == nullptr)
	{
		UE_LOG(LogAnimation, Error, TEXT("BindEventToOnSkeletalMeshBoneTransformsFinalized : skeletal mesh component is null."));
		PrintScriptCallstack();
		return FDelegateHandleBP();
	}
	
	auto EventLambda = [Event]() {Event.ExecuteIfBound();};
	const FOnBoneTransformsFinalizedMultiCast::FDelegate Delegate = FOnBoneTransformsFinalizedMultiCast::FDelegate::CreateLambda(EventLambda);
	const FDelegateHandle DelegateHandle = SkeletalMeshComponent->RegisterOnBoneTransformsFinalizedDelegate(Delegate);
	return FDelegateHandleBP{DelegateHandle};
}

void UActionBlueprintLibrary::UnbindEventFromOnSkeletalMeshBoneTransformsFinalized(USkeletalMeshComponent* SkeletalMeshComponent, FDelegateHandleBP DelegateHandle)
{
	if(SkeletalMeshComponent == nullptr)
	{
		UE_LOG(LogAnimation, Error, TEXT("UnbindEventFromOnSkeletalMeshBoneTransformsFinalized : skeletal mesh component is null."));
		PrintScriptCallstack();
		return;
	}
	
	SkeletalMeshComponent->UnregisterOnBoneTransformsFinalizedDelegate(DelegateHandle.DelegateHandle);
}

#if 0
void UOSCServer::BindEventToOnOSCAddressPatternMatchesPath(const FOSCAddress& InOSCAddressPattern, const FOSCDispatchMessageEventBP& InEvent)
{
	if (InOSCAddressPattern.IsValidPattern())
	{
		FOSCDispatchMessageEvent& MessageEvent = AddressPatterns.FindOrAdd(InOSCAddressPattern);
		MessageEvent.AddUnique(InEvent);
	}
}

void UOSCServer::UnbindEventFromOnOSCAddressPatternMatchesPath(const FOSCAddress& InOSCAddressPattern, const FOSCDispatchMessageEventBP& InEvent)
{
	if (InOSCAddressPattern.IsValidPattern())
	{
		if (FOSCDispatchMessageEvent* AddressPatternEvent = AddressPatterns.Find(InOSCAddressPattern))
		{
			AddressPatternEvent->Remove(InEvent);
			if (!AddressPatternEvent->IsBound())
			{
				AddressPatterns.Remove(InOSCAddressPattern);
			}
		}
	}
}

void UOSCServer::UnbindAllEventsFromOnOSCAddressPatternMatchesPath(const FOSCAddress& InOSCAddressPattern)
{
	if (InOSCAddressPattern.IsValidPattern())
	{
		AddressPatterns.Remove(InOSCAddressPattern);
	}
}

void UOSCServer::UnbindAllEventsFromOnOSCAddressPatternMatching()
{
	AddressPatterns.Reset();
}

#endif

int UActionBlueprintLibrary::MoveCharacterToSafeSpotIfStuck(ACharacter* CharacterActor)
{
	check(CharacterActor != nullptr);
	UWorld* World = CharacterActor->GetWorld();
	check(World != nullptr);
	const UCapsuleComponent* CapsuleComponent = CharacterActor->GetCapsuleComponent();
	check(CapsuleComponent != nullptr);

	// if this character actor is not stuck, return 0
	FVector ActorLocation = CharacterActor->GetActorLocation();
	const FRotator ActorRotation = CharacterActor->GetActorRotation();

	const FCollisionQueryParams Params(SCENE_QUERY_STAT(ComponentEncroachesBlockingGeometry_NoAdjustment), false, CharacterActor);
	if(!World->OverlapBlockingTestByChannel(CharacterActor->GetActorLocation(), FQuat(ActorRotation), CapsuleComponent->GetCollisionObjectType(), FCollisionShape::MakeCapsule(CapsuleComponent->GetScaledCapsuleRadius(), CapsuleComponent->GetScaledCapsuleHalfHeight()), Params))
		return 0;

	// find safe spot, if not found, return -1
	if(!World->FindTeleportSpot(CharacterActor, ActorLocation, ActorRotation))
		return -1;
	
	CharacterActor->SetActorLocation(ActorLocation, false, nullptr, ETeleportType::TeleportPhysics);
	return 1;
}
