// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

class FToolBarBuilder;
class FMenuBuilder;
class FWorkspaceItem;
class SWidget;

class FMinesweeperPluginModule : public IModuleInterface, public TSharedFromThis<FMinesweeperPluginModule>
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& TabSpawnArgs);
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	/**
	 * Registers a tab spawner for the Minsweeper Plugin.
	 *
	 * @param WorkspaceGroup The workspace group to insert the tab into.
	 */
	void RegisterTabSpawner(const TSharedPtr<FWorkspaceItem>& WorkspaceGroup);

	/** Unregisters the tab spawner for the Minesweeper Plugin. */
	void UnregisterTabSpawner();

	/** True if the tab spawners have been registered for this module */
	bool bHasRegisteredTabSpawners;

private:

	void RegisterMenus();

private:
	TSharedPtr<class FUICommandList> PluginCommands;

protected:
	FName TabName;
	FText TabDisplayName;
	FText ToolTipText;
};
