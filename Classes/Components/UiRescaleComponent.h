#ifndef __H_UIRESCALECOMPONENT__
#define __H_UIRESCALECOMPONENT__

#include "axmol.h"
#include "Helper/short_types.h"
#include "shared_scenes/GameUtils.h"

enum BorderLayout {
    TOP = 0,
    TOP_RIGHT = 1,
    RIGHT = 2,
    BOTTOM_RIGHT = 3,
    BOTTOM = 4,
    BOTTOM_LEFT = 5,
    LEFT = 6,
    TOP_LEFT = 7,
    CENTER = 8
};

#define RESIZE_UI_ELEMENTS auto list = GameUtils::findComponentsByName(this, "UiRescaleComponent"); \
for (auto i : list) dynamic_cast<UiRescaleComponent*>(i)->windowSizeChange(visibleSize);

class UiRescaleComponent : public ax::Component {
public:
    bool _setLayerColor = false;
    bool _ignore = false;
    bool _resizeHints = false;
    bool _fitting = false;
    Size _fittingSize;
    Rect _resizeHintsRect;
    Size _visibleSize;
    bool _recreateLayer = false;
    bool _isUiElemDirty = false;

    UiRescaleComponent(Size _visibleSize);

    UiRescaleComponent();

    void onAdd();

    void onRemove();

    // enableDesignScaleIgnoring and setVisibleSizeHints will be ignored if this is enabled
    // owner will be casted to LayerColor* object, so be careful using this function
    UiRescaleComponent* enableLayerResizing();

    UiRescaleComponent* enableDesignScaleIgnoring();

    UiRescaleComponent* setVisibleSizeHints(f32 widthDiv = 2, f32 widthOffset = 0, f32 heightDiv = 2, f32 heightOffset = 0);

    UiRescaleComponent* setBorderLayout(BorderLayout border = BorderLayout::CENTER);

    UiRescaleComponent* enableSizeFitting(Size _sizeInPixels);

    void windowSizeChange(Size newVisibleSize);
};

#endif
