using UnrealBuildTool;

public class AbilitiesPlusEditor : ModuleRules
{
	public AbilitiesPlusEditor(ReadOnlyTargetRules Target)
        : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "BlueprintGraph",
				"Core",
                "CoreUObject",

                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks"
            });

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "GraphEditor",
                "KismetCompiler",
                "MainFrame",
                "Slate",
                "SlateCore",
                "UnrealEd",
			});
    }
}
