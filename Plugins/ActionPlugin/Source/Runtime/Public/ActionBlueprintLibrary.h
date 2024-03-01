// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "CoreMinimal.h"
#include "UObject/Script.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "UObject/UnrealType.h"
#include "UObject/Stack.h"
#include "UObject/ScriptMacros.h"

#include "ActionBlueprintLibrary.generated.h"

// Delegates
USTRUCT(BlueprintType)
struct FDelegateHandleBP
{
	GENERATED_BODY()
	FDelegateHandle DelegateHandle;
};

DECLARE_DYNAMIC_DELEGATE(FOnSkeletalMeshBoneTransformsFinalized);


UCLASS(meta=(BlueprintThreadSafe, ScriptName = "ActionGameLibrary"))
class ACTIONPLUGINRUNTIME_API UActionBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Adds event to dispatch after Skeletal FinalizeBoneTransform */
	UFUNCTION(BlueprintCallable, Category = "SkeletelMesh")
	static FDelegateHandleBP BindEventToOnSkeletalMeshBoneTransformsFinalized(USkeletalMeshComponent* SkeletalMeshComponent, const FOnSkeletalMeshBoneTransformsFinalized& Event);

	/** Unbinds specific event from Skeletal FinalizeBoneTransform. */
	UFUNCTION(BlueprintCallable, Category = "SkeletelMesh")
	static void UnbindEventFromOnSkeletalMeshBoneTransformsFinalized(USkeletalMeshComponent* SkeletalMeshComponent, FDelegateHandleBP DelegateHandle);

	/** Actor FindSafeSpot */
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	static int MoveCharacterToSafeSpotIfStuck(ACharacter* CharacterActor);
};