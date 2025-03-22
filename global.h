#ifndef GLOBAL_H
#define GLOBAL_H

#include <functional>
#include <cassert>

class QWidget;

extern std::function<void(QWidget*)> Repolish;

enum class RequestId {
    kGetVerifyCode  =   1001,
    kRegisterUser   =   1002,
    kLogin          =   1003,
};

enum class StatusCode {
    kSuccess    =   0,
    kErrJson    =   1,
    kErrNetwork =   2,
    kServerError=   3,
    kCount,
};

constexpr std::array<const char*, static_cast<size_t>(StatusCode::kCount)> kStatusMessages
    {"ok", "parse json error", "network error", "server internal error"};

enum class Module {
    kRegister   =   0,
    kLogin      =   1,
};

enum class InputTip {
    kSuccess =0,
    kEmailError=1,
    kPasswordError=2,
    kConfirmError=3,
    kVerifyError=4,
    kUserError=5,
};

constexpr std::string_view EnumToString(StatusCode status_code) {
    assert(status_code < StatusCode::kCount);
    return kStatusMessages[static_cast<size_t>(status_code)];
}



#endif // GLOBAL_H
