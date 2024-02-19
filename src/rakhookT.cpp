#include "rakhookT.h"
#include <rakhook/rakhook.hpp>
#include "main.h"
#include "recorder/recorder.h"

void rakhookT::sendOnfoot(samp::Synchronization::OnfootData sync) {
    RakNet::BitStream bs;
    bs.ResetWritePointer();
    bs.Write(static_cast<uint8_t>(207));
    bs.Write(reinterpret_cast<const char*>(&sync), sizeof(samp::Synchronization::OnfootData));
    rakhook::send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
    samp::RefChat()->AddMessage(-1, "[{00c4ff}RouteREC{FFFFFF}] Send OnFoot");
}

void rakhookT::sendDialogResponse() {
    RakNet::BitStream bitStream;
    bitStream.ResetWritePointer();

    std::vector<unsigned char> bytes = {215, 2, 0, 0, 0, 0, 0, 16, 0, 0, 0, 79, 110, 68, 105, 97, 108, 111, 103, 82,
                                        101, 115, 112, 111, 110, 115, 101, 8, 0, 0, 0, 100, 0, 0, 0, 0, 100, 1, 0, 0, 0,
                                        100, 255, 255, 255, 255, 115, 0, 0, 0, 0};

    for (unsigned char byte : bytes)
        bitStream.Write(byte);

    rakhook::send(&bitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void rakhookT::sendAlt() {
    RakNet::BitStream bs;
    bs.ResetWritePointer();
    samp::Synchronization::OnfootData data;

    memcpy_s(&data, sizeof(samp::Synchronization::OnfootData),
        &samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer()->m_onfootData, sizeof(samp::Synchronization::OnfootData));

    data.m_controllerState.m_bPedWalk = true;

    bs.Write(static_cast<uint8_t>(207));
    bs.Write(reinterpret_cast<const char*>(&data), sizeof(samp::Synchronization::OnfootData));

    rakhook::send(&bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void rakhookT::installRakHook() {
    rakhook::on_send_rpc += [](int &id, RakNet::BitStream *bs, PacketPriority &priority, PacketReliability &reliability,
                               char &ord_channel, bool &sh_timestamp) -> bool {
        // id

        return true;
    };

    rakhook::on_receive_packet += [](Packet *packet) -> bool {
        // +(*packet->data)
        switch (+(*packet->data)) {
            case 215: {
                if (settings.flood_alt_enter) {
                    sendDialogResponse();
                    return false;
                }
                RakNet::BitStream bitStream(packet->data, packet->length, true);
                bitStream.ResetReadPointer();
                int32_t lenPacket;
                bitStream.IgnoreBits(64);
                bitStream.Read(lenPacket);
                if (lenPacket <= 0 || lenPacket >= 5000)
                    return true;
                char *dialogText = new char[lenPacket + 1]{0};
                bitStream.Read(dialogText, lenPacket);
                std::string str_dialogText(dialogText);
                if (settings.debugPackets)
                    samp::RefChat()->AddMessage(-1, dialogText);
                delete[] dialogText;
                if (str_dialogText.find("начать") != std::string::npos && settings.skipEnable) {
                    sendDialogResponse();
                    if (settings.cursorEnable)
                        settings.temp_cursorEnable = true;
                    return false;
                }
                if (lenPacket == 7 && (str_dialogText.find("Capture") != std::string::npos)) {
                    settings.temp_cursorEnable = true;
                    if (settings.cursorEnable)
                        settings.temp_cursorEnable = true;
                }
                return true;
            }
        }
        return true;
    };

    rakhook::on_send_packet += [](RakNet::BitStream *bitStream, PacketPriority &priority,
                                  PacketReliability &reliability, char &ord_channel) -> bool {
        // +(*bitStream->GetData())
        switch (+(*bitStream->GetData())) {
            case 207: {
                if (settings.record) {
                    bitStream->ResetReadPointer();
                    samp::Synchronization::OnfootData data;
                    memcpy_s(&data, sizeof(samp::Synchronization::OnfootData),
                    &samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer()->m_onfootData, sizeof(samp::Synchronization::OnfootData));

                    int8_t packet;
                    bitStream->Read(packet);
                    bitStream->Read((PCHAR)&data, sizeof(samp::Synchronization::OnfootData));

                    // Запись данных в файл
                    settings.wRoute_file << std::format("{}|{}|{}|{}|{}|{}|", data.m_position.x, data.m_position.y, data.m_position.z, data.m_speed.x, data.m_speed.y, data.m_speed.z);
                    settings.wRoute_file << std::format("{}|{}|{}|{}|", data.m_fQuaternion[0], data.m_fQuaternion[1], data.m_fQuaternion[2], data.m_fQuaternion[3]);
                    settings.wRoute_file << std::format("{}|{}|{}|", data.m_nSpecialAction, data.m_controllerState.m_value, GetTickCount() - settings.last_point_time);
                    settings.wRoute_file << std::format("{}|{}\n", data.m_controllerState.m_sLeftStickY, data.m_controllerState.m_sLeftStickX);
                    settings.last_point_time = GetTickCount();
                }
                if (settings.render)
                    return false;
                if (settings.enable_fakelag) {
                    samp::Synchronization::OnfootData data;
                    memset(&data, 0, sizeof(samp::Synchronization::OnfootData));
                    uint8_t packet;

                    bitStream->ResetReadPointer();
                    bitStream->Read(packet);
                    bitStream->Read((PCHAR)&data, sizeof(samp::Synchronization::OnfootData));

                    static ULONGLONG dwSyncTick = GetTickCount();
                    if (GetTickCount() - dwSyncTick > (ULONGLONG)settings.smooth_fakelag)
                        dwSyncTick = GetTickCount();
                    else if (!data.m_controllerState.m_bRightShoulder1 == 1)
                        return false;
                }

                return true;
            }
            default: {
                return true;
            }
        }

    };

    rakhook::on_receive_rpc += [](unsigned char &id, RakNet::BitStream *bitStream) -> bool {
        // +id
        switch (+id) {
            case 93: {
                int32_t color, lenPacket;
                bitStream->ResetReadPointer();
                bitStream->Read(color);
                bitStream->Read(lenPacket);
                char *chatText = new char[lenPacket + 1]{0};
                bitStream->Read(chatText, lenPacket);
                std::string str_chatText(chatText);
                if (settings.debugPackets)
                    samp::RefChat()->AddMessage(-1, chatText);
                delete[] chatText;
                if (((str_chatText.find("БАНКОВСКИЙ ЧЕК") != std::string::npos) ||
                     (str_chatText.find("захватить бизнес") != std::string::npos) ||
                     (str_chatText.find("захватить территорию") != std::string::npos) ||
                     (str_chatText.find("захватили бизнес") != std::string::npos) ||
                     (str_chatText.find("захватили территорию") != std::string::npos)) && settings.captureTable) {
                    settings.temp_captureTable = true;
                    if (settings.cursorEnable)
                        settings.temp_cursorEnable = true;
                    if (settings.debugPackets)
                        samp::RefChat()->AddMessage(-1, "I found the text and I will now send cmd");
                }
                return true;
            }
        }
        return true;
    };
}