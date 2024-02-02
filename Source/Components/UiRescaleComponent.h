#ifndef __H_UIRESCALECOMPONENT__
#define __H_UIRESCALECOMPONENT__

#include "axmol.h"
#include "Helper/short_types.h"
#include "shared_scenes/GameUtils.h"

enum BorderLayout : U8 {
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

enum BorderContext : U8 {
    SCREEN_SPACE = 0,
    PARENT = 1,
};

class UiRescaleComponent : public ax::Component {
public:
    bool _setLayerColor = false;
    bool _ignore = false;
    bool _resizeHints = false;
    bool _fitting = false;
    S2D _fittingSize;
    R2D _resizeHintsRect;
    S2D _visibleSize;
    bool _recreateLayer = false;
    bool _isUiElemDirty = false;
    V2D _identityScale = V2D::ONE;

    UiRescaleComponent(Size _visibleSize);

    UiRescaleComponent();

    void onAdd();

    void onRemove();

    // enableDesignScaleIgnoring and setVisibleSizeHints will be ignored if this is enabled
    // owner will be casted to LayerColor* object, so be careful using this function
    UiRescaleComponent* enableLayerResizing();

    UiRescaleComponent* enableDesignScaleIgnoring(V2D identity = V2D::ONE);

    UiRescaleComponent* setVisibleSizeHints(F32 widthDiv = 2, F32 widthOffset = 0, F32 heightDiv = 2, F32 heightOffset = 0);

    UiRescaleComponent* setBorderLayout(BorderLayout border = BorderLayout::CENTER);

    UiRescaleComponent* enableSizeFitting(Size _sizeInPixels);

    void windowSizeChange(Size newVisibleSize);
};

#endif
