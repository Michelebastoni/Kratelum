#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashesWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashEndSignature, const int, dashIndex);

UCLASS(Abstract)
class TBO02_API UDashesWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FDashEndSignature OnDashCooldownEnd;

private:
	UPROPERTY(EditAnywhere, Category = "Dash")
	TSubclassOf<class UDashBar> DashBarClass;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox;

	UPROPERTY()
	TObjectPtr<class AGenericCharacter> genericCharacter;
	
	UPROPERTY()
	TObjectPtr<class AGenericPlayerController> genericController;
	
	UPROPERTY()
	TObjectPtr<class UDashAction> DashAction;

	UPROPERTY(Transient)
	TArray<class UDashBar*> Dashes;

	UPROPERTY(Transient)
	int32 CurrentDashIndex = 0;
	float DashCoolDown = 0.0f;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	TArray<class UDashBar*> GetDashes() const { return Dashes; }

private:
	UFUNCTION(Category = "Dash")
	void CreateDashesBar();
	
	UFUNCTION(Category = "Dash")
	void OnChangeDash(int DashIndex);

	UFUNCTION(Category = "Dash")
	void OnChangeDashCooldown(const class UDashAction* currentDashAction);
	
	UFUNCTION()
	void Notify_DashEnd(int DashIndex) { OnDashCooldownEnd.Broadcast(DashIndex - 1); };
};

