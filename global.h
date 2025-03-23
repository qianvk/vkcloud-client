#ifndef GLOBAL_H
#define GLOBAL_H

#include <functional>
#include <cassert>
#include <string_view>
#include <QString>
#include <qpixmap.h>

class QWidget;

extern std::function<void(QWidget*)> Repolish;

enum class RequestId {
    kGetVerifyCode  =   1001,
    kRegisterUser   =   1002,
    kLogin          =   1003,
    ID_CHAT_LOGIN_RSP= 1006, //登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007, //用户搜索请求
    ID_SEARCH_USER_RSP = 1008, //搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,  //添加好友申请
    ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息

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

struct ServerInfo{
    QString host;
    QString port;
    QString token;
    int uid;
};

enum class ChatRole
{

    Self,
    Other
};

struct MsgInfo{
    QString msgFlag;//"text,image,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

//聊天界面几种模式
enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode, //聊天模式
    ContactMode, //联系模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

const QString add_prefix = "添加标签 ";

const int  tip_offset = 5;


const std::vector<QString>  strs ={"hello world !",
                                   "nice to meet u",
                                   "New year，new life",
                                   "You have to love yourself",
                                   "My love is written in the wind ever since the whole world is you"};

const std::vector<QString> heads = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg"
};

const std::vector<QString> names = {
    "HanMeiMei",
    "Lily",
    "Ben",
    "Androw",
    "Max",
    "Summer",
    "Candy",
    "Hunter"
};

const int CHAT_COUNT_PER_PAGE = 13;

template <typename T>
int sign(T value) {
    return (T(0) < value) - (value < T(0));
}



#endif // GLOBAL_H
