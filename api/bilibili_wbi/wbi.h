#include <nlohmann/json.hpp>

#include "../APIStatus.h"

#pragma once

/**
 * From <a href=https://socialsisteryi.github.io/bilibili-API-collect/docs/misc/sign/wbi.html#cplusplus>website</a>
 * */

/*
 * 注意，假定不会发生错误！
 */
class Wbi {
    API constexpr static std::array<uint8_t, 64> MIXIN_KEY_ENC_TAB_ = {
            46, 47, 18, 2, 53, 8, 23, 32, 15, 50, 10, 31, 58, 3, 45, 35,
            27, 43, 5, 49, 33, 9, 42, 19, 29, 28, 14, 39, 12, 38, 41, 13,
            37, 48, 7, 16, 24, 55, 40, 61, 26, 17, 0, 1, 60, 51, 30, 4,
            22, 25, 54, 21, 56, 59, 6, 63, 57, 62, 11, 36, 20, 34, 44, 52
    };

    /* 获取 md5 hex(lower) */
    API static std::string Get_md5_hex(const std::string &Input_str);

public:
    /* 将 json 转换为 url 编码字符串 */
    API static std::string Json_to_url_encode_str(const nlohmann::json &Json);

    /* 获取 wbi key */
    API static std::pair<std::string, std::string> Get_wbi_key();

    /* 获取 mixin key */
    API static std::string Get_mixin_key(const std::string &Img_key, const std::string &Sub_key);

    /* 计算签名(w_rid) */
    API static std::string Calc_sign(nlohmann::json &Params, const std::string &Mixin_key);
};

API std::string calc_w_rid(nlohmann::json params);