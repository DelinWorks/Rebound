#include "MapEditor.h"

USING_NS_CC;
using namespace backend;

using namespace GameUtils;

inline std::wstring getResolutionRecommendation(V2D resulotion) {
    if (resulotion.x == 1920 && resulotion.y == 1080) {
        return L"for FHD 1080p a 1.0 scaling is recommended.";
    }
    else if (resulotion.x == 2560 && resulotion.y == 1440) {
        return L"for 2K 1440p a 1.5 scaling is recommended.";
    }
    else if (resulotion.x == 3840 && resulotion.y == 2160) {
        return L"for 4K 2160p a 2.0 scaling is recommended.";
    }
    else {
        return L"Resolution: " + std::to_wstring(int(resulotion.x)) + L" by " + std::to_wstring(int(resulotion.y));
    }
}

void MapEditor::handleSignal(std::string signal)
{
    if (signal == "tooltip_hsv_reset")
        _editorToolTip->showToolTip(L"HSV color reset.", 0.5);
    else if (signal == "tooltip_gui_scale_advice")
        _editorToolTip->showToolTip(WFMT(L"%sx%.2f\n%s\n\n%s\n%s\n%s",
            L"Current GUI Scaling: ", Rebound::getInstance()->gameWindow.guiScale,
            getResolutionRecommendation(Director::getInstance()->getOpenGLView()->getFrameSize()).c_str(),
            L"It is recommended that you restart the Map Editor,",
            L"whenever you change the GUI Scaling or Window Size.",
            L"Doing so will prevent bugs or glitches in the GUI."), 10);
}
