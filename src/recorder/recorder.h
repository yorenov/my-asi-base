#pragma once
#include "utils/sampapi-i.h"

class recorder {
public:
    static void recorderLoop();
    static bool is_file_exist(const std::string& filename);
    static void record_route(const std::string& param);
    static void render_route(const std::string& param);
};

struct RoutePoint {
    float fPosition[3]{};
    float fQuaternion[4]{};
    float fMoveSpeed[3]{};
    uint16_t m_nSpecialAction{};
    int16_t sKeys{};
    int16_t sUpDownKeys{};
    int16_t sLeftRightKeys{};
    unsigned long delay{};
};