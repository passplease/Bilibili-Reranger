#include <array>    // std::array
#include <locale>   // std::locale
#include <print>    // std::println

/// third party libraries
#include <cpr/cpr.h>
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>

#include <regex>
#include "wbi.h"

std::string Wbi::Get_md5_hex(const std::string &Input_str) {
    CryptoPP::Weak1::MD5 hash;
    std::string          md5_hex;

    CryptoPP::StringSource ss(Input_str, true,
                              new CryptoPP::HashFilter(hash,
                                                       new CryptoPP::HexEncoder(
                                                               new CryptoPP::StringSink(md5_hex)
                                                       )
                              )
    );

    std::ranges::for_each(md5_hex, [](char &x) { x = std::tolower(x); });
    return md5_hex;
}

std::string Wbi::Json_to_url_encode_str(const nlohmann::json &Json) {
    std::string encode_str;
    for (const auto &[key, value]: Json.items()) {
        encode_str.append(key).append("=").append(cpr::util::urlEncode(value.is_string() ? value.get<std::string>() : to_string(value))).append("&");
    }

    // remove the last '&'
    encode_str.resize(encode_str.size() - 1, '\0');
    return encode_str;
}

std::pair<std::string, std::string> Wbi::Get_wbi_key() {
    const auto url    = cpr::Url {"https://api.bilibili.com/x/web-interface/nav"};
    const auto cookie = cpr::Cookies {
            {"SESSDATA", "xxxxxxxxxxxx"},
    };
    const auto header = cpr::Header {
            {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"},
            {"Referer", "https://www.bilibili.com/"},
    };
    const auto response = cpr::Get(url, cookie, header);

    nlohmann::json json = nlohmann::json::parse(response.text);

    const std::string img_url = json["data"]["wbi_img"]["img_url"];
    const std::string sub_url = json["data"]["wbi_img"]["sub_url"];

    std::string img_key = img_url.substr(img_url.find("wbi/") + 4, img_url.find(".png") - img_url.find("wbi/") - 4);
    std::string sub_key = sub_url.substr(sub_url.find("wbi/") + 4, sub_url.find(".png") - sub_url.find("wbi/") - 4);
    return {img_key, sub_key};
}

std::string Wbi::Get_mixin_key(const std::string &Img_key, const std::string &Sub_key) {
    std::string raw_wbi_key_str = Img_key + Sub_key;
    std::string result;

    std::ranges::for_each(MIXIN_KEY_ENC_TAB_, [&result, &raw_wbi_key_str](const uint8_t x) {
        result.push_back(raw_wbi_key_str.at(x));
    });

    return result.substr(0, 32);
}

std::string Wbi::Calc_sign(nlohmann::json &Params, const std::string &Mixin_key) {
    Params["wts"] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    const std::string encode_str = Json_to_url_encode_str(Params).append(Mixin_key);
    return Get_md5_hex(encode_str);
}

std::string calc_w_rid(nlohmann::json params){
    auto [img_key, sub_key] = Wbi::Get_wbi_key();
    const auto mixin_key = Wbi::Get_mixin_key(img_key, sub_key);
    const std::string back = Wbi::Json_to_url_encode_str(params) + "&w_rid" + Wbi::Calc_sign(params, mixin_key);
    return back;
}