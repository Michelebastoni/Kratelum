#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GenericPlayerController.generated.h"


class UMeleeComponent;
enum class EInteractionMethod : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInputReceivedSignaure, const FKey&, Key, bool, isGamepadInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinishSetUpSiganture, const AGenericPlayerController*, Player_Controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkipCinematic);
/**
 */
UCLASS(NotBlueprintable)
class TBO02_API AGenericPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FInputReceivedSignaure OnInputReceived;

	UPROPERTY(BlueprintAssignable)
	FOnFinishSetUpSiganture OnFinishSetUp;

	UPROPERTY(BlueprintAssignable)
	FSkipCinematic OnSkipCinematic;

	AGenericPlayerController();

	/******* TEAM ID ******/
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	FGenericTeamId TeamID;
	/******* END TEAM ID ******/

private:
	// Value that indicate tha imput mapping context where all the inputs will be triggered
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> GameContext;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> UIContext;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> HUBContext;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> PauseContext;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<class UInputMappingContext> CinematicContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UInputAction> SkipAction;

	UPROPERTY(EditDefaultsOnly,Category = "Enhanced Input|Actions")
	TArray<class UActionBase*> Actions;

	// Indicate the param that have to change into outline material instance
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Outline")
	FName OutlineThicknessParamName = "Outline";

	// Indicate the new param define by "OutlineThicknessParamName" variable of the outline material after being pointed
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Outline")
	float OutlineTargetThickness = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|laser")
	float LaserThickness = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|laser")
	float LaserDistance = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|laser")
	TArray <TEnumAsByte<EObjectTypeQuery>> laserObjectDetection;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Widget|Reload")
	TSubclassOf<class UQTEReload> reloadWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Widget|Cursor")
	TSubclassOf<class UCursorWidget> Cursor_Widget_Class;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Widget|HUD")
	TSubclassOf<class UHUDWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Widget|Loading")
	TSubclassOf<class ULoadingScreen> Loading_Screen_Class;

	UPROPERTY(EditAnywhere, Category = "Enhanced Input|Widget|Pause")
	TSubclassOf<class UGameScreen> PauseClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Debug")
	bool showMessage;

	UPROPERTY(Transient)
	TObjectPtr<class UEnhancedInputComponent> enhancedInputComponent;

	// Ref to subSystem to manage input
	UPROPERTY(Transient)
	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem> subSystem;

	// Ref to character controlled, generally will be "Generic Character", useful to trigger all the methods
	UPROPERTY(Transient)
	TObjectPtr<class APlayerCharacter> Current_Character;

	UPROPERTY(Transient)
	TObjectPtr<class UQTEReload> Reload_Screen;

	UPROPERTY(Transient)
	TObjectPtr<class UGameScreen> Pause_Screen;

	UPROPERTY(Transient)
	TObjectPtr<class UHUDWidget> HUDWidget;

	UPROPERTY(Transient)
	TObjectPtr<class UCursorWidget> Cursor_Widget;

	UPROPERTY(Transient)
	TObjectPtr<class ULoadingScreen> Loading_Screen;

	UPROPERTY(Transient)
	TObjectPtr<class AGenericCharacter> targetCharacter;

	UPROPERTY(Transient)
	TArray<FHitResult> HightLightTargetHits;

	uint8 bIsLastInputAPad : 1;

public:
	void Tick(float deltaSeconds) override;

	UFUNCTION(BlueprintPure)
	bool GetIsLastInputAPad() const { return bIsLastInputAPad; }

	UFUNCTION(BlueprintCallable)
	void SwitchIntoHUBContext() { SwitchContext(HUBContext); }
	
	UFUNCTION(BlueprintCallable)
	void SwitchIntoUIContext() { SwitchContext(UIContext); }

	UFUNCTION(BlueprintCallable)
	void SwitchIntoGameContext() { SwitchContext(GameContext); }

	UFUNCTION(BlueprintCallable)
	void SwitchIntoCinematicContext() { SwitchContext(CinematicContext); }

	UFUNCTION(BlueprintCallable)
	void SwitchIntoPauseContext() { SwitchContext(PauseContext); }
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool InputKey(const FInputKeyParams& Params) override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	UFUNCTION()
	void HandleCharacterDeath(class AGenericCharacter* controlledCharacter, class AWeapon* currentWeapon, const AActor* harmer, class UIntegrity* integrity);

public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
	TArray<class UActionBase*> GetActions() const { return Actions; }

	template <typename T>
	T* GetAction() const;

	class UEnhancedInputLocalPlayerSubsystem* GetSubSystem() const { return subSystem; }

	UFUNCTION(BlueprintPure, Category = "Actions", meta = (DeterminesOutputType = "ActionClass"))
	class UActionBase* GetActionByClass(TSubclassOf<class UActionBase> ActionClass) const;

	UFUNCTION(BlueprintCallable)
	class UHUDWidget* GetHUDScreen() const { return HUDWidget; }

	UFUNCTION(BlueprintCallable)
	class ULoadingScreen* GetLoadingScreen() const { return Loading_Screen; }
	
	UFUNCTION(BlueprintCallable)
	class UGameScreen* GetPauseScreen() const { return Pause_Screen; }

	UFUNCTION(BlueprintCallable)
	class APlayerCharacter* GetCurrentCharacter() const { return Current_Character; }
	
	UFUNCTION(BlueprintCallable)
	class UQTEReload* GetCurrentReloadWidget() const { return Reload_Screen; }

	UFUNCTION(BlueprintCallable)
	class UGameScreen* GetCurrentPauseWidget() const { return Pause_Screen; }
	
	UFUNCTION(BlueprintCallable)
	UEnhancedInputComponent* GetEnhancedInputComponent() const { return enhancedInputComponent; }
	
private:
	void SwitchContext(class UInputMappingContext* newInputMap);
	UFUNCTION()
	void HandleSkipCinematic();
};
