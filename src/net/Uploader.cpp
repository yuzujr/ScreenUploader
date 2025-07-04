#include "net/Uploader.h"

#include <cpr/cpr.h>

#include <format>
#include <string>
#include <vector>

#include "core/Logger.h"

bool Uploader::uploadImage(const std::vector<uint8_t>& frame,
                           const std::string& url) {
    return uploadImageWithSSL(frame, url, false);
}

bool Uploader::uploadImageWithSSL(const std::vector<uint8_t>& frame,
                                  const std::string& url,
                                  bool skip_ssl_verification) {
    if (frame.empty()) {
        Logger::error("frame is empty!");
        return false;
    }

    // 生成时间戳文件名
    std::string filename = std::format("{}.jpg", generateTimestampFilename());

    // 构造 POST 请求
    cpr::Buffer buffer{
        frame.begin(),  // 首地址
        frame.end(),    // 尾地址
        filename        // 文件名
    };

    cpr::Multipart form = {cpr::Part{"file", buffer}};

    cpr::Response r;

    // 根据是否需要跳过SSL验证选择不同的请求方式
    if (url.starts_with("https://") && skip_ssl_verification) {
        Logger::warn(
            "HTTPS SSL certificate verification disabled - unsafe for "
            "production");
        cpr::SslOptions sslOptions =
            cpr::Ssl(cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyPeer{false});
        r = cpr::Post(
            cpr::Url{url}, form,
            cpr::Header{{"User-Agent", "cpr/1.11.0"}, {"Accept", "*/*"}},
            cpr::Timeout{3000}, sslOptions);
    } else {
        if (url.starts_with("https://")) {
            Logger::info("Using HTTPS with SSL certificate verification");
        }
        r = cpr::Post(
            cpr::Url{url}, form,
            cpr::Header{{"User-Agent", "cpr/1.11.0"}, {"Accept", "*/*"}},
            cpr::Timeout{3000});
    }

    return handleUploadResponse(r, "Image upload");
}

bool Uploader::uploadConfig(const nlohmann::json& config,
                            const std::string& url) {
    return uploadConfigWithSSL(config, url, false);
}

bool Uploader::uploadConfigWithSSL(const nlohmann::json& config,
                                   const std::string& url,
                                   bool skip_ssl_verification) {
    cpr::Response r;

    // 根据是否需要跳过SSL验证选择不同的请求方式
    if (url.starts_with("https://") && skip_ssl_verification) {
        Logger::warn(
            "HTTPS SSL certificate verification disabled - unsafe for "
            "production");
        cpr::SslOptions sslOptions =
            cpr::Ssl(cpr::ssl::VerifyHost{false}, cpr::ssl::VerifyPeer{false});
        r = cpr::Post(cpr::Url{url},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Body{config.dump()}, cpr::Timeout{3000}, sslOptions);
    } else {
        if (url.starts_with("https://")) {
            Logger::info("Using HTTPS with SSL certificate verification");
        }
        r = cpr::Post(cpr::Url{url},
                      cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Body{config.dump()}, cpr::Timeout{3000});
    }

    return handleUploadResponse(r, "Config upload");
}

std::string Uploader::generateTimestampFilename() {
    std::ostringstream fileName;
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};

#if defined(_MSC_VER)
    localtime_s(&localTime, &nowTime);  // MSVC
#else
    localtime_r(&nowTime, &localTime);  // POSIX
#endif

    fileName << "screen_" << std::put_time(&localTime, "%Y%m%d_%H%M%S");
    return fileName.str();
}

bool Uploader::handleUploadResponse(const cpr::Response& r,
                                    const std::string& context) {
    if (r.error) {
        Logger::error(std::format("{} failed: {}", context, r.error.message));
        return false;
    }
    if (r.status_code < 200 || r.status_code >= 300) {
        std::string errorMsg =
            std::format("{} failed: HTTP {}", context, r.status_code);
        if (!r.text.empty()) {
            errorMsg += std::format(" - Message: {}", r.text);
        }
        Logger::error(errorMsg);
        return false;
    }
    if (!r.text.empty()) {
        Logger::info(r.text, LogTarget::Server);
    }
    return true;
}