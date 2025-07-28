#include "Widgets/Widget_Cursor/CursorWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"


void UCursorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Canvas_Panel_Slot = Cast<UCanvasPanelSlot>(Cursor_Image->Slot);

	checkf(Canvas_Panel_Slot, TEXT("Cursor Image parent is not a canvas panel. Source: [%s]"), *GetName());

}

void UCursorWidget::UpdateCursorLocation(FVector2D NewLocation)
{
	Canvas_Panel_Slot->SetPosition(NewLocation);
}
