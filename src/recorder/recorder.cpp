#include "recorder.h"

#include <RakHook/rakhook.hpp>

#include "main.h"
#include "rakhookT.h"

#include <filesystem>

std::string last_filename;
std::vector<RoutePoint> route_points;

// Запись маршрута
const std::filesystem::path routes_main_dir = "C:/routes/";

void recorder::record_route(const std::string &param) {
    if (param.empty()) return;
    if (settings.record) {
        settings.wRoute_file.close();
        //SF->getSAMP()->getChat()->AddChatMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Маршрут '%s' сохранен", last_filename.c_str());
        samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Saved");
        return;
    }

    const std::filesystem::path filePath(routes_main_dir / (param + ".txt"));
    if (exists(filePath) && last_filename != param) {
        //SF->getSAMP()->getChat()->AddChatMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Файл уже существует. Отправь еще раз чтобы перезаписать");
        samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] The file already exists. Send again to re-record");
        last_filename = param;
        return;
    }

    //SF->getSAMP()->getChat()->AddChatMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Запись маршрута '%s'", param.c_str());
    samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Recording");
    settings.wRoute_file.open(filePath);
    settings.last_point_time = GetTickCount();
    settings.record = true;
}

// Воспроизведение маршрута

void recorder::render_route(const std::string &param) {
    if (settings.render) {
        settings.render = false;
        //SF->getSAMP()->getChat()->AddChatMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Воспроизведение остановлено");
        samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Stop");
        return;
    }

    const std::filesystem::path filePath(routes_main_dir / (param + ".txt"));
    if (!exists(filePath)) {
        //SF->getSAMP()->getChat()->AddChatMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Маршрут '%s' не найден", param.c_str());
        samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Route not found");
        return;
    }

    settings.rRoute_file.open(filePath);
    route_points.clear();
    std::string line;
    RoutePoint point;
    while (std::getline(settings.rRoute_file, line)) {
        samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] япидор");

        sscanf_s(line.c_str(), "%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%hd|%hd|%lu|%hd|%hd",
                 &point.fPosition[0], &point.fPosition[1], &point.fPosition[2],
                 &point.fMoveSpeed[0], &point.fMoveSpeed[1], &point.fMoveSpeed[2],
                 &point.fQuaternion[0], &point.fQuaternion[1], &point.fQuaternion[2], &point.fQuaternion[3],
                 &point.m_nSpecialAction, &point.sKeys, &point.delay, &point.sUpDownKeys, &point.sLeftRightKeys);
#ifdef _DEBUG
        std::printf("get line: %f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%hd|%hd|%lu|%hd|%hd\n",
                    point.fPosition[0], point.fPosition[1], point.fPosition[2],
                    point.fMoveSpeed[0], point.fMoveSpeed[1], point.fMoveSpeed[2],
                    point.fQuaternion[0], point.fQuaternion[1], point.fQuaternion[2], point.fQuaternion[3],
                    point.m_nSpecialAction, point.sKeys, point.delay, point.sUpDownKeys, point.sLeftRightKeys);
#endif
        route_points.push_back(point);
    }
    samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Route");
    settings.rRoute_file.close();
    settings.last_point_time = GetTickCount();
    settings.last_point = 0;
    settings.render = true;
}


void recorder::recorderLoop() {
    if ((GetTickCount() - settings.last_point_time) > route_points[settings.last_point].delay) {
        samp::Synchronization::OnfootData sync;
        memcpy_s(&sync, sizeof(samp::Synchronization::OnfootData),
                 &samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer()->m_onfootData,
                 sizeof(samp::Synchronization::OnfootData));
        RoutePoint &current_point = route_points[settings.last_point];
        sync.m_position.x = current_point.fPosition[0];
        sync.m_position.y = current_point.fPosition[1];
        sync.m_position.z = current_point.fPosition[2];
        sync.m_speed.x = current_point.fMoveSpeed[0];
        sync.m_speed.y = current_point.fMoveSpeed[1];
        sync.m_speed.z = current_point.fMoveSpeed[2];
        sync.m_fQuaternion[0] = current_point.fQuaternion[0];
        sync.m_fQuaternion[1] = current_point.fQuaternion[1];
        sync.m_fQuaternion[2] = current_point.fQuaternion[2];
        sync.m_fQuaternion[3] = current_point.fQuaternion[3];
        sync.m_nSpecialAction = current_point.m_nSpecialAction;
        sync.m_controllerState.m_value = current_point.sKeys;
        sync.m_controllerState.m_sLeftStickY = current_point.sUpDownKeys;
        sync.m_controllerState.m_sLeftStickX = current_point.sLeftRightKeys;

        rakhookT::sendOnfoot(sync);

        if (settings.tp_point)
            samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer()->GetPed()->Teleport({
                current_point.fPosition[0], current_point.fPosition[1], current_point.fPosition[2]
            });

        settings.last_point_time = GetTickCount();
        settings.last_point++;
        if (settings.last_point == route_points.size()) {
            if (settings.cycle)
                settings.last_point = 0;
            else {
                samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Stoped");
                settings.render = false;
            }
        }
    }
}
