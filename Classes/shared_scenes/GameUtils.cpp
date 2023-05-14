#include "GameUtils.h"

void GameUtils::GLFW_ClipCursor(bool unset)
{
    return;

    auto mWindow = glfwGetWin32Window(Darkness::getInstance()->gameWindow.window);

    RECT rect;
    GetClientRect(mWindow, &rect);

    POINT ul;
    ul.x = rect.left;
    ul.y = rect.top;

    POINT lr;
    lr.x = rect.right;
    lr.y = rect.bottom;

    MapWindowPoints(mWindow, nullptr, &ul, 1);
    MapWindowPoints(mWindow, nullptr, &lr, 1);

    rect.left = ul.x;
    rect.top = ul.y;

    rect.right = lr.x;
    rect.bottom = lr.y;

    ClipCursor(unset ? nullptr : (Darkness::getInstance()->gameWindow.isCursorLockedToWindow ? &rect : nullptr));
}

void GameUtils::GLFW_SetCursorNormal()
{
    if (Darkness::getInstance()->gameWindow.currentWindowCursor == 0)
        return;

    Image* img = new Image();
    img->initWithImageFile("cursor.png"sv);
    GLFWimage* icon = new GLFWimage();
    icon->width = img->getWidth();
    icon->height = img->getHeight();
    icon->pixels = img->getData();
    GLFWcursor* cursor = glfwCreateCursor(icon, 0, 0);
    glfwSetCursor(Darkness::getInstance()->gameWindow.window, cursor);
    Darkness::getInstance()->gameWindow.currentWindowCursor = 0;
}

void GameUtils::GLFW_SetCursorSelected()
{
    if (Darkness::getInstance()->gameWindow.currentWindowCursor == 1)
        return;

    Image* img = new Image();
    img->initWithImageFile("cursor_selected.png"sv);
    GLFWimage* icon = new GLFWimage();
    icon->width = img->getWidth();
    icon->height = img->getHeight();
    icon->pixels = img->getData();
    GLFWcursor* cursor = glfwCreateCursor(icon, 0, 0);
    glfwSetCursor(Darkness::getInstance()->gameWindow.window, cursor);
    Darkness::getInstance()->gameWindow.currentWindowCursor = 1;
}

void GameUtils::GLFW_SetBorder(HWND window, int on)
{
    return;

    RECT rect;
    DWORD style;

    style = GetWindowLong(window, GWL_STYLE);

    if (on)
    {
        if (!GetWindowRect(window, &rect)) return;
        style &= ~WS_OVERLAPPEDWINDOW;
        style |= WS_POPUP;
        AdjustWindowRect(&rect, style, TRUE);
        SetWindowLong(window, GWL_STYLE, style);
    }
    else
    {
        if (!GetWindowRect(window, &rect)) return;

        style |= WS_CAPTION;
        style |= WS_OVERLAPPEDWINDOW;
        style &= ~WS_POPUP;

        AdjustWindowRect(&rect, style, TRUE);

        SetWindowLong(window, GWL_STYLE, style);
    }

    SetWindowPos(window, HWND_TOPMOST,
        rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOMOVE | SWP_FRAMECHANGED);
}

Vec2 GameUtils::parseVector2D(std::string position)
{
    try {
        std::string value = position;
        std::string::iterator end_pos = std::remove(value.begin(), value.end(), ' ');
        value.erase(end_pos, value.end());
        if (value.at(0) == '[')
            value.erase(0, 1);
        if (value.at(value.length() - 1) == ']')
            value.erase(value.length() - 1, value.length());
        std::string token, strX, strY;
        strX = value.substr(0, value.find(","));
        value.erase(0, value.substr(0, value.find(",")).length());
        if (value.length() != 0)
            strY = value.substr(1, value.length());
        if (strX.length() == 0) strX = "0";
        if (strY.length() == 0) strY = strX;
        return Vec2(std::stof(strX), std::stof(strY));
    }
    catch (std::exception& e) {
        return { 0,0 };
    };
}

backend::ProgramState* GameUtils::createGPUProgram(std::string resources_frag_shader_path, std::string resources_vertex_shader_path)
{
    auto fileUtiles = FileUtils::getInstance();
    auto fragmentFullPath = fileUtiles->fullPathForFilename("shaders/" + resources_frag_shader_path);
    auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
    auto vertexFullPath = fileUtiles->fullPathForFilename("shaders/" + resources_vertex_shader_path);
    auto vertexSource = fileUtiles->getStringFromFile(vertexFullPath);
    auto program = backend::Device::getInstance()->newProgram(vertexSource.length() == 0 ? positionTextureColor_vert : vertexSource.c_str(), fragSource.length() == 0 ? positionTextureColor_frag : fragSource.c_str());
    auto theFuckingProgramState = new backend::ProgramState(program);
    return theFuckingProgramState;
}

void GameUtils::addSpriteFramesFromJson(const std::string_view texture_path, const std::string_view json_path)
{
    Texture2D* tex = Director::getInstance()->getTextureCache()->addImage(texture_path);
    //SpriteFrameCache::getInstance()->addSpriteFramesWithFile(dat_path + ".dat", tex);

    auto cache = SpriteFrameCache::getInstance();

    if (FileUtils::getInstance()->fullPathForFilename(json_path).empty())
        return;

    Document d;
    d.Parse(FileUtils::getInstance()->getStringFromFile(json_path).c_str());

    const rapidjson::Value& meta = d["meta"];

    if (meta.IsObject() && meta.FindMember("alias")->value.GetBool())
        tex->setAliasTexParameters();

    const rapidjson::Value& frames = d["frames"];
    if (frames.IsArray())
    {
        for (auto const& it : frames.GetArray())
        {
            if (it.IsObject())
            {
                auto const& _name = it.FindMember("filename")->name;
                auto const& _value = it.FindMember("filename")->value;
                auto const& _valuerot = it.FindMember("rotated")->value;
                auto const& frame = it.FindMember("frame")->value;
                auto const& sourceSize = it.FindMember("sourceSize")->value;
                if (frame.IsObject() && sourceSize.IsObject()) {
                    const char* _filename = _value.GetString();
                    i32 _x = frame.FindMember("x")->value.GetInt();
                    i32 _y = frame.FindMember("y")->value.GetInt();
                    i32 _w = frame.FindMember("w")->value.GetInt();
                    i32 _h = frame.FindMember("h")->value.GetInt();
                    i32 _sw = sourceSize.FindMember("w")->value.GetInt();
                    i32 _sh = sourceSize.FindMember("h")->value.GetInt();
                    bool rotated = _valuerot.GetBool();

                    //std::cout << _name.GetString() << ": " << _filename << "\n"
                    //    "x: " << _x <<
                    //    ", y: " << _y <<
                    //    ", w: " << _w <<
                    //    ", h: " << _h << "\n"
                    //    "rotated: " << rotated << "\n\n";

                    auto spFrame = SpriteFrame::createWithTexture(tex, Rect(_x, _y, _w, _h), rotated, Vec2::ZERO, Size(_sw, _sh));
                    cache->addSpriteFrame(spFrame, _filename);
                }
            }
        }
    }
}

Vec2 GameUtils::convertFromScreenToSpace(const Vec2& locationInView, Node* cam, bool reverseY)
{
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    auto loc = director->convertToGL(Vec2(locationInView.x,
        (!reverseY ? visibleSize.y - locationInView.y : locationInView.y)));
    auto screenSize = director->getOpenGLView()->getFrameSize();
    return Vec2((((loc.x - visibleSize.x / 2) * cam->getScale()) + cam->getPositionX()),
        (((loc.y - visibleSize.y / 2) * cam->getScale()) + cam->getPositionY()));
        //.rotateByAngle(cam->getPosition(), -AX_DEGREES_TO_RADIANS(cam->getRotation()));
}

void GameUtils::setNodeIgnoreDesignScale(cocos2d::Node* node, bool ignoreScaling, float nestedScale) {
    Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
    Size actualWinSize = Director::getInstance()->getWinSizeInPixels();
    float x = actualWinSize.width / actualFrameSize.width * (ignoreScaling ? 1 : Darkness::getInstance()->gameWindow.guiScale);
    float y = actualWinSize.height / actualFrameSize.height * (ignoreScaling ? 1 : Darkness::getInstance()->gameWindow.guiScale);

    // If resolution policy is other than SHOW_ALL then we set
    // the scale to x and y value. and there will be no stretching.
    if (Darkness::getInstance()->gameWindow.windowPolicy != ResolutionPolicy::SHOW_ALL) {
        node->setScaleX(x * nestedScale);
        node->setScaleY(y * nestedScale);
    }
    // If the scale dimensions are the same, then we just
    // set the scale to the x or y value, any will suffice.
    else if (x == y)
        node->setScale(x * nestedScale);
    // If somehow the scale dimensions are different, then we just
    // see which dimension is bigger and set the scale to that
    // value so that any ui node doesn't stretch and deform.
    else node->setScale((x < y ? y : x) * nestedScale);
}

Size GameUtils::getWinDiff() {
    Size actualFrameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
    Size actualWinSize = Director::getInstance()->getWinSizeInPixels();
    return Size(actualFrameSize.width - actualWinSize.width, actualFrameSize.height - actualWinSize.height);
}

void GameUtils::setNodeScaleFHD(cocos2d::Node* node)
{
    f32 res = Director::getInstance()->getOpenGLView()->getFrameSize().width +
        Director::getInstance()->getOpenGLView()->getFrameSize().height;
}

std::vector<Component*> GameUtils::findComponentsByName(Node* parent, std::string_view name, bool containParent, std::vector<Component*> list, bool recursive)
{
    std::vector<Component*> comps;

    if (recursive)
        comps = list;

    if (parent->getChildrenCount() == 0 && containParent)
    {
        auto comp = parent->getComponent(name);
        if (comp)
        {
            std::vector<Component*> vect;
            vect.push_back(comp);
            return vect;
        }
    }

    if (!recursive)
    {
        auto comp = parent->getComponent(name);
        if (comp) comps.push_back(comp);
    }

    bool hasFoundSomething = false;

    for (auto i : parent->getChildren())
    {
        auto comp = parent->getComponent(name);
        if (comp)
        {
            comps.push_back(comp);
            hasFoundSomething = true;
        }

        if (i->getChildrenCount() > 0)
            comps = findComponentsByName(i, name, containParent, comps, true);
    }

    if (!recursive)
    {
        if (hasFoundSomething)
            return comps;
    }

    return comps;
}

std::vector<Node*> GameUtils::findNodesByTag(Node* parent, int tag, bool containParent, std::vector<Node*> list, bool recursive)
{
    std::vector<Node*> nodes;

    if (recursive)
        nodes = list;

    if (parent->getChildrenCount() == 0 && containParent)
    {
        if (parent->getTag() == tag)
        {
            std::vector<Node*> vect;
            vect.push_back(parent);
            return vect;
        }
    }

    if (!recursive)
    {
        if (parent->getTag() == tag)
            nodes.push_back(parent);
    }

    bool hasFoundSomething = false;

    for (auto i : parent->getChildren())
    {
        if (i->getTag() == tag)
        {
            nodes.push_back(i);
            hasFoundSomething = true;
        }

        if (i->getChildrenCount() > 0)
            nodes = findNodesByTag(i, tag, containParent, nodes, true);
    }

    if (!recursive)
    {
        if (hasFoundSomething)
            return nodes;
    }

    return nodes;
}

void GameUtils::Editor::UndoRedoState::applyUndoState()
{
    switch (action) {
    case UNDOREDO_TILEMAP:
        applyUndoStateTilemapEdit();
        break;
    }
}

void GameUtils::Editor::UndoRedoState::applyRedoState()
{
    switch (action) {
    case UNDOREDO_TILEMAP:
        applyRedoStateTilemapEdit();
        break;
    }
}

void GameUtils::Editor::UndoRedoState::applyUndoStateTilemapEdit()
{
    for (auto& _ : affected.prev_tiles)
        affected.map->setTileAt({ _.first.x, _.first.y }, _.second);
}

void GameUtils::Editor::UndoRedoState::applyRedoStateTilemapEdit()
{
    for (auto& _ : affected.next_tiles)
        affected.map->setTileAt({ _.first.x, _.first.y }, _.second);
}

void GameUtils::Editor::UndoRedoAffectedTiles::addOrIgnoreTilePrev(ax::Vec2 pos, u32 gid)
{
    if (prev_tiles.find({ pos.x, pos.y }) == prev_tiles.end())
        prev_tiles.emplace(pos, gid);
}

void GameUtils::Editor::UndoRedoAffectedTiles::addOrIgnoreTileNext(ax::Vec2 pos, u32 gid)
{
    if (next_tiles.find({ pos.x, pos.y }) == next_tiles.end())
        next_tiles.emplace(pos, gid);
}
