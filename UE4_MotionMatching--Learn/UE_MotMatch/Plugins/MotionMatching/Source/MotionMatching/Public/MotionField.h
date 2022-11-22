

#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "MotionKey.h"

#include "Animation/Skeleton.h"

#include "MotionField.generated.h"


class USkeleton;
struct FMotionExtractionContext;

// ��������Ƕ���WITH_EDITOR�Ĳ���
#if WITH_EDITOR
USTRUCT()
struct FTagRange
{
	// ����洢��TArray<float> RangeKeys
	// ��ע��Tag�ķ�Χ
	// which is call FTagRange
	GENERATED_BODY()

	UPROPERTY()
	TArray <float> RangeKeys;

	FTagRange()
	{
		RangeKeys.Empty();
	}
};


// This is the Helper for tag
// One TagHelper -> Equa -> One AnimName -> FTagRange
USTRUCT()
struct FTagHelper
{
	GENERATED_BODY()

	UPROPERTY()
	FName AnimName = NAME_None;

	// One Per Tag
	UPROPERTY()
	TArray <FTagRange> Ranges;

	FTagHelper()
	{
	    AnimName = NAME_None;
		Ranges.Empty();
	}
};

#endif //WITH_EDITOR


// ���ڶ����ʲ��༭������������ڵ�Personaģ�飬��󲿷����ݴ���˽��Ŀ¼�£����Һܶ�������Ѿ���AnimSequence��AnimMontage��AnimBlendSpace�ȷ���д����
// �ڲ��Ķ�Դ�������£��ܲ�������������չ����������ֱ�Ӽ̳�UObject�������
// �����������ֱ��ȥ�̳�UObject

// Ҳ���Գ���ȥ�̳�UAnimationAsset�����Ը��õ����뵽UE��Animation��ϵͳ��
// ��Ȼ��Personaģ��Ҳ�����ṩ���������BoneTree֮������ýӿڣ����Ǻ�����ᵽ
// Personaģ��������BoneTree�Ľӿڣ�����ֱ�ӻ�ȡ������Ĺ�����


/*Data asset class that holds the List of Candidate Motion Keys to transition to*/
UCLASS(BlueprintType)
class MOTIONMATCHING_API UMotionField : public UObject
{
	GENERATED_BODY()

private:
	// ����ʱ��
	UPROPERTY()
	float TimeStep;
	// ѡ���Ķ�Ӧ�Ĺ���
	UPROPERTY()
	USkeleton* Skeleton;
	// TArray Tags
	UPROPERTY()
	TArray <FString> Tags;
	
#if WITH_EDITOR
	// One Per SrcAnim
	UPROPERTY()
	TArray <FTagHelper> TagHelpers;

	// Reset the All the TagHelper
	void ResetTagHelperRanges();
	// 
	void ProcessTagHelpers();
#endif //WITH_EDITOR

public:

	UMotionField(const FObjectInitializer& ObjectInitializer);

	// This here is pretty much the List of candidate poses we search through every frame.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionKeys")
	TArray <FMotionKey> MotionKeys;

	// Motion Bones are the Bones I store the Joint Data from for the Matching process
	UPROPERTY(BlueprintReadOnly, Category = "BoneData")
	TArray <FName> MotionBones;
		
	UPROPERTY(BlueprintReadOnly, Category = "Animations")
	TArray <UAnimSequence*> SourceAnimations;

	// Gotta Take this and put it elswhere and make it static at some point
	UFUNCTION()
	int GetLowestCostMotionKey
	(
		const float Responsiveness, 
		const float VelocityStrength, 
		const float PoseStrength,
		const FTrajectoryData DesiredTrajectory, 
		const TArray<FJointData> PresentJointData, 
		const FVector PresentVel, 
		const TArray <uint8> TagsToLookFor,
		const TArray <uint8> TagsToIgnore,
		float & OutLowestCost
	);

	bool CheckMotionKeyAgainstTags(const int32 MotionKeyIdx, const TArray <uint8> TagsIdx);

#if WITH_EDITOR
	void AddRangeToTagHelper(const float RangeTime, const uint8 AtTagIdx, const int32 AtAnimIdx);
	void RemoveRangeFromTagHelper(const float RangeTime, const uint8 AtTagIdx, const int32 AtAnimIdx);
	bool IsTimeTagged(const float RangeTime, const uint8 AtTagIdx, const int32 AtAnimIdx);
#endif //WITH_EDITOR

	void ResetTagsInAnim(const int32 AnimIdx);
	void ApplyTagsToKeysInAnimRange(const int32 AnimIdx, const float Start, const float End, const uint8 TagIdx);


	//---------------------------------------------------------------------------------------------------------------
	void GetMotionFieldProperties(float & OutTimeStep, TArray<FString> & OutTags) const;
	void SetProperties(const float InTimeStep, const TArray <FString> InTags);
		
	float GetTimeStep() const
	{
		return TimeStep;
	}
	
	//----------------------------------------------------------------------------------------------------------------
	FString GetTagAtIndex( const int32 TagIdx) const;
	

	int32 GetNumOfTags()
	{
		return Tags.Num();
	}

	bool IsValidMotionKeyIndex(const int Index)
	{
		return MotionKeys.IsValidIndex(Index);
	}
	int GetNumMotionKey()
	{
		return MotionKeys.Num();
	}
	const FMotionKey& GetKeyFrameChecked(const int FrameIndex) const
	{
		return MotionKeys[FrameIndex];
	}
		
	UAnimSequence* GetMotionKeyAnimSequence(const int AtMotionKeyIndex)
	{
		return SourceAnimations[MotionKeys[AtMotionKeyIndex].SrcAnimIndex];
	}
	

	void ClearAnimMotionKeysUtil(const int AtSourceAnimIndex);

	void ClearAllMotionKeys();

	void ClearAnimMotionKeys(const int AtSourceAnimIndex);
		
	void RebakeMotionKeysInAnim(const int FromSourceAnimation);
	void RebakeAllAnim();


	int GetSrcAnimNum()
	{
		return SourceAnimations.Num();
	}

	void AddSrcAnim(UAnimSequence* NewAnim);
		

	void DeleteSrcAnim(const int AtIndex);

	bool IsValidSrcAnimIndex(const int AtIndex)
	{
		return SourceAnimations.IsValidIndex(AtIndex);
	}

	UAnimSequence* GetSrcAnimAtIndex(const int Index)
	{
		return SourceAnimations[Index];
	}

	bool IsExtractedFrame(const FName AnimName, const float Time);

	// Get the MotionField Skeleton
	USkeleton* GetMotionFieldSkeleton()
	{
		return Skeleton;
	}

	void SetMotionFieldSkeleton(USkeleton* InSkeleton)
	{
		Skeleton = InSkeleton;
	}

	void PopulateFromSkeleton(USkeleton * SourceSkeleton, const TArray<FName> InMotionBones)
	{
		Skeleton = SourceSkeleton;
		MotionBones = InMotionBones;
	}

//#endif //WITH_EDITOR
};
