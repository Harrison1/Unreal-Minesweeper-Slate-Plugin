// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperPlugin.h"
#include "MinesweeperPluginStyle.h"
#include "MinesweeperPluginCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockableTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWidget.h"
#include "SMineSweep.h"

static const FName MinesweeperPluginTabName("MinesweeperPlugin");

#define LOCTEXT_NAMESPACE "FMinesweeperPluginModule"

void FMinesweeperPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMinesweeperPluginStyle::Initialize();
	FMinesweeperPluginStyle::ReloadTextures();

	FMinesweeperPluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperPluginCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMinesweeperPluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperPluginModule::RegisterMenus));
}

void FMinesweeperPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperPluginStyle::Shutdown();

	FMinesweeperPluginCommands::Unregister();
}

void FMinesweeperPluginModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	RegisterTabSpawner(nullptr);
}

void FMinesweeperPluginModule::RegisterTabSpawner(const TSharedPtr<FWorkspaceItem>& WorkspaceGroup)
{
	if (bHasRegisteredTabSpawners)
	{
		UnregisterTabSpawner();
	}

	bHasRegisteredTabSpawners = true;

	{
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner("MinesweeperPlugin", FOnSpawnTab::CreateRaw(this, &FMinesweeperPluginModule::SpawnTab))
			.SetDisplayName(LOCTEXT("MinesweeperPluginTitle", "Minesweeper UI"))
			.SetTooltipText(LOCTEXT("MinesweeperPluginTooltipText", "Open the Minesweeper tab."));

		FGlobalTabmanager::Get()->TryInvokeTab(FTabId("MinesweeperPlugin"));
	}
}

void FMinesweeperPluginModule::UnregisterTabSpawner()
{
	bHasRegisteredTabSpawners = false;

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("MinesweeperPlugin");
}

void FMinesweeperPluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperPluginCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperPluginCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

TSharedRef<SDockTab> FMinesweeperPluginModule::SpawnTab(const FSpawnTabArgs& TabSpawnArgs)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SMineSweep)
		];

	return SpawnedTab;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMinesweeperPluginModule, MinesweeperPlugin)