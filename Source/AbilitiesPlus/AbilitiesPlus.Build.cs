using UnrealBuildTool;

public class AbilitiesPlus : ModuleRules
{
	public AbilitiesPlus(ReadOnlyTargetRules Target)
        : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			});

		PrivateIncludePaths.AddRange(
			new string[] {
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",

                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks"
            });

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			});
	}
}
