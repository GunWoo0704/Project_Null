// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimGraphNode_FootFixing.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DetailLayoutBuilder.h"

/////////////////////////////////////////////////////
// UAnimGraphNode_FootPlacement

#define LOCTEXT_NAMESPACE "AnimGraphNode_FootFixing"

UAnimGraphNode_FootFixing::UAnimGraphNode_FootFixing(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_FootFixing::GetControllerDescription() const
{
	return LOCTEXT("FootFixing", "Foot Fixing");
}

FText UAnimGraphNode_FootFixing::GetTooltipText() const
{
	return LOCTEXT("FootFixingTooltip", "Foot Fixing.");
}

FLinearColor UAnimGraphNode_FootFixing::GetNodeTitleColor() const
{
	return FLinearColor(FColor(153.f, 0.f, 0.f));
}

FText UAnimGraphNode_FootFixing::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

void UAnimGraphNode_FootFixing::GetInputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

void UAnimGraphNode_FootFixing::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

#undef LOCTEXT_NAMESPACE
