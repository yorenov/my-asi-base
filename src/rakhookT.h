#pragma once
#include "utils/sampapi-i.h"

class rakhookT {
public:
    static void sendDialogResponse();
    static void installRakHook();
    static void sendAlt();
    static void sendOnfoot(samp::Synchronization::OnfootData sync);
};