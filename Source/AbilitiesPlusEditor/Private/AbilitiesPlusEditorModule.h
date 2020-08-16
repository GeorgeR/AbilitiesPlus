#pragma once

#include "IAbilitiesPlusEditorModuleInterface.h"

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAbilitiesPlusEditorModule final
    : public IAbilitiesPlusEditorModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;
};