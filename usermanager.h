#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <memory>
#include <singleton.h>
#include "userdata.h"
#include <vector>
class UserMgr:public QObject,public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void SetToken(QString token);
    int GetUid();
    QString GetName();
    QString GetIcon();
    std::shared_ptr<UserInfo> GetUserInfo();
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    bool AlreadyApply(int uid);
    std::vector<std::shared_ptr<UserInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<UserInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<UserInfo> GetFriendById(int uid);
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);
    std::shared_ptr<UserInfo> GetFriendByRelateId(uint32_t relate_id);
private:
    UserMgr();
    std::shared_ptr<UserInfo> _user_info;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::vector<std::shared_ptr<UserInfo>> _friend_list;
    std::unordered_map<uint32_t, std::shared_ptr<UserInfo>> friend_map_;
    QMap<int, std::shared_ptr<UserInfo>> _friend_map;
    QString _token;
    int _chat_loaded;
    int _contact_loaded;

public slots:
    void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};

#endif // USERMANAGER_H
