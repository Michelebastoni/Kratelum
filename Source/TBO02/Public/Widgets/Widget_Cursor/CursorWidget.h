// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CursorWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class TBO02_API UCursorWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Transient, Meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Transient, Meta = (BindWidget))
	TObjectPtr<class UImage> Cursor_Image;

	TObjectPtr<class UCanvasPanelSlot> Canvas_Panel_Slot;

protected:
	virtual void NativeConstruct() override;
public:
	void UpdateCursorLocation(FVector2D NewLocation);

};
