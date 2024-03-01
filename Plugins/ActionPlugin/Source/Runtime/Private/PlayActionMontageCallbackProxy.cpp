// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayActionMontageCallbackProxy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayActionMontageCallbackProxy)

//////////////////////////////////////////////////////////////////////////
// UPlayActionMontageCallbackProxy

UPlayActionMontageCallbackProxy::UPlayActionMontageCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageInstanceID(INDEX_NONE)
	, bInterruptedCalledBeforeBlendingOut(false)
{
}

UPlayActionMontageCallbackProxy* UPlayActionMontageCallbackProxy::CreateProxyObjectForPlayActionMontage(
	USkeletalMeshComponent* InSkeletalMeshComponent,
	UObject* ActionObject,
	UAnimMontage* MontageToPlay,
	const float PlayRate,
	const float StartingPosition,
	const FName StartingSection)
{
	UPlayActionMontageCallbackProxy* Proxy = NewObject<UPlayActionMontageCallbackProxy>();
	Proxy->ActionObjectPtr = ActionObject;
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->PlayMontage(InSkeletalMeshComponent, MontageToPlay, PlayRate, StartingPosition, StartingSection);
	return Proxy;
}


void UPlayActionMontageCallbackProxy::PlayMontage(const USkeletalMeshComponent* InSkeletalMeshComponent, 
	UAnimMontage* MontageToPlay, 
	const float PlayRate, 
	const float StartingPosition, 
	const FName StartingSection)
{
	bool bPlayedSuccessfully = false;
	if (InSkeletalMeshComponent)
	{
		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
			bPlayedSuccessfully = (MontageLength > 0.f);

			if (bPlayedSuccessfully)
			{
				AnimInstancePtr = AnimInstance;
				if (const FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}

				BlendingOutDelegate.BindUObject(this, &UPlayActionMontageCallbackProxy::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UPlayActionMontageCallbackProxy::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UPlayActionMontageCallbackProxy::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UPlayActionMontageCallbackProxy::OnNotifyEndReceived);
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(this->ActionObjectPtr, NAME_None);
	}
}

bool UPlayActionMontageCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}


void UPlayActionMontageCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(this->ActionObjectPtr, NotifyName);
	}
}


void UPlayActionMontageCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(this->ActionObjectPtr, NotifyName);
	}
}


void UPlayActionMontageCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast(this->ActionObjectPtr, NAME_None);
		bInterruptedCalledBeforeBlendingOut = true;
	}
	else
	{
		OnBlendOut.Broadcast(this->ActionObjectPtr, NAME_None);
	}
}

void UPlayActionMontageCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(this->ActionObjectPtr, NAME_None);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(this->ActionObjectPtr, NAME_None);
	}

	UnbindDelegates();
}

void UPlayActionMontageCallbackProxy::UnbindDelegates()
{
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UPlayActionMontageCallbackProxy::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UPlayActionMontageCallbackProxy::OnNotifyEndReceived);
	}
}

void UPlayActionMontageCallbackProxy::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}
