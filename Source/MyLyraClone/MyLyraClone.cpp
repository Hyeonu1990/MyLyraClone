// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyLyraClone.h"
#include "Modules/ModuleManager.h"

/**
 * FMyLyraCloneGameModule
 */
class FMyLyraCloneGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};

void FMyLyraCloneGameModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("StartupModule")); // 게임 시작 시 모듈 init 될 때 호출
}

void FMyLyraCloneGameModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("ShutdownModule")); // 게임 종료 시 모듈 deinit 될 때 호출
}

IMPLEMENT_PRIMARY_GAME_MODULE(FMyLyraCloneGameModule, MyLyraClone, "MyLyraClone");
