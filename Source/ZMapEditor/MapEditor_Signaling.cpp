#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

void MapEditor::handleSignal(std::string signal)
{
    if (signal == "tooltip_hsv_reset")
        _editorToolTip->showToolTip(L"HSV color reset.", 0.5);
    else if (signal == "tooltip_gui_scale_advice")
        _editorToolTip->showToolTip(WFMT(L"%sx%.2f\n\n%s\n%s\n%s",
            L"Current GUI Scaling: ", Rebound::getInstance()->gameWindow.guiScale,
            L"It is recommended that you restart the Map Editor,",
            L"whenever you change the GUI Scaling or Window Size.",
            L"Doing so will prevent bugs or glitches in the GUI."), 10);
}
