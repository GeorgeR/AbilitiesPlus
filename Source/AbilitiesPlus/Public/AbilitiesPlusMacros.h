#pragma once

#if __has_include("NoesisTypeClass.h")
#define POST_ATTRIBUTE_CHANGE(PropertyName) NoesisNotifyPropertyChanged(this, #PropertyName);
#else
#define POST_ATTRIBUTE_CHANGE(PropertyName)
#endif

// Uses macros from AttributeSet.h
#define ATTRIBUTE_SETTER(PropertyName)															\
	FORCEINLINE void Set##PropertyName(float NewVal)											\
	{																							\
		auto AbilityComp = GetOwningAbilitySystemComponent();									\
		if (ensure(AbilityComp))																\
		{																						\
			AbilityComp->SetNumericAttributeBase(Get##PropertyName##Attribute(), NewVal);		\
			POST_ATTRIBUTE_CHANGE(PropertyName)													\
		};																						\
	}

#define ATTRIBUTE_INIT(PropertyName)															\
	FORCEINLINE void Init##PropertyName(float NewVal)											\
	{																							\
		PropertyName.SetBaseValue(NewVal);														\
		PropertyName.SetCurrentValue(NewVal);													\
		POST_ATTRIBUTE_CHANGE(PropertyName)														\
	}

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)											\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)									\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)												\
	ATTRIBUTE_SETTER(PropertyName)																\
	ATTRIBUTE_INIT(PropertyName)
