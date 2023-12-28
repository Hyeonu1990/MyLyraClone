#include "MyLyraCloneAssetManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraCloneAssetManager) // UE_INLINE_GENERATED_CPP_BY_NAME : cpp 파일을 모듈 cpp 파일에 인라인-> 헤더 구문 분석이 덜 필요하므로 컴파일 시간이 향상

#pragma region static methods
PRAGMA_DISABLE_OPTIMIZATION
bool UMyLyraCloneAssetManager::TestClone()
{
	// naive functionality test
	static bool bResult = false;
	{
		bResult = ShouldLogAssetLoads();
	}
	return true;
}
PRAGMA_ENABLE_OPTIMIZATION

UMyLyraCloneAssetManager& UMyLyraCloneAssetManager::Get()
{
	check(GEngine);

	// 우리는 AssetManager를 UEngine의 GEngine의 AssetManager의 Class를 오버라이드했기 때문에, 이미 GEngine에 Asset Manager가 있음
	if (UMyLyraCloneAssetManager* Singleton = Cast<UMyLyraCloneAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	//UE_LOG(LogHak, Fatal, TEXT("invalid AssetManagerClassName in DefaultEngine.ini(project settings); it must be HakAssetManager"));

	// 위의 UE_LOG의 Fatal의 Verbose가 물론 Crash를 통해, 여기까지 도달하지는 않지만, Code Completness를 위해 더미로 리턴 진행
	return *NewObject<UMyLyraCloneAssetManager>();
}

bool UMyLyraCloneAssetManager::ShouldLogAssetLoads()
{
	// 여기서 여러분이 알아두면 좋은 두 가지 Unreal 코딩 패턴이있다:
	// 1. FParse::Param과 FCommandLine::Get()을 통해, 여러분들이 원하는 속성값은 CommandLine으로 가져올 수 있다
	//    - 한번 UHakAssetManager::Get()을 통해 실습해보자
	// 2. 함수 안에 static 변수 선언이다:
	//    - 언리얼에서 자주 보게될 패턴으로, 보통 글로벌 변수를 선언하고 싶을 경우, 여러분에게 두가지 옵션이 있다:
	//      1. .h 파일에 extern을 선언하여 헤더파일을 포함시키거나?
	//      2. static 함수를 선언하여, 해당 함수 안에 static 변수를 선언하여, 반환하는 방법 (마치 global 변수로 활용할 수 있음)
	//         - 대표적인 예시로 앞서 Singleton 패턴이 되겠다

	// 실습:
	// - UHakAssetManager::TestClone 구현
	// - bLogAssetLoads의 메모리 주소를 파악
	const TCHAR* commandLineContent = FCommandLine::Get(); // 프로그램 실행 시 입력했던 파라미터 가져오기
	static bool bLogAssetLoads = FParse::Param(commandLineContent, TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

/*
 동기 함수를 따로 만든 이유
 1. 비동기보다 확실히 빠름
 2. 불필요한 에셋을 로드하는게 있는지 확인하기 위해
 */
UObject* UMyLyraCloneAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath) // FSoftObjectPath : 경로 전용 구조체
{
	// 해당 함수를 만든 이유는 'synchronous load asset을 불필요하게 하는 것이 있는지 체크하기 위함!'
	if (AssetPath.IsValid())
	{
		// 잠깐 FScopeLogTime을 확인해보자:
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			// 단순히 로깅하면서, 초단위로 로깅 진행
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronously loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// 여기서 두가지 분기:
		// 1. AssetManager가 있으면, AssetManager의 StreamableManager를 통해 정적 로딩
		// 2. 아니면, FSoftObjectPath를 통해 바로 정적 로딩
		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// if asset manager is not ready, use LoadObject()
		// - 슥 보면, StaticLoadObject가 보인다: 
		// - 참고로, 항상 StaticLoadObject하기 전에 StaticFindObject를 통해 확인하고 실패하면 진짜 로딩함
		return AssetPath.TryLoad(); // LoadSynchronous보다 속도가 느림
	}

	return nullptr;
}
#pragma endregion

#pragma region member methods
UMyLyraCloneAssetManager::UMyLyraCloneAssetManager()
// 항상 Constructor를 까먹지 않도록 하자!
	: UAssetManager()
{
}

void UMyLyraCloneAssetManager::AddLoadedAsset(const UObject* Asset)
{
	// Asset Nullptr 확인, Init 동안에는 thread-safety가 보장되지 않음
	if (ensureAlways(Asset))
	{
		// thread-safety 보장을 위해 Lock
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}

/**
 * UAssetManager's interfaces
 */
void UMyLyraCloneAssetManager::StartInitialLoading()
{
	// 오버라이드할 경우, Super의 호출은 꼭 까먹지 말자
	Super::StartInitialLoading();
	UE_LOG(LogTemp, Warning, TEXT("StartInitialLoading"));
	// HakGameplayTags 초기화
	// - Lyra와의 차이점을 한번 보고 가자:
	//   - STARTUP_JOB() 매크로를 활용하고 있으나, 현재 우리는 로딩과 관계 없으므로 간단하게 구현
	//FHakGameplayTags::InitializeNativeTags();
}
#pragma endregion
