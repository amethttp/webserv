#include "RequestProcesser.hpp"
#include "utils/string/string.hpp"
#include "utils/headers/headers.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestPctDecoder/RequestPctDecoder.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetSeparator/RequestTargetSeparator.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetNormalizer/RequestTargetNormalizer.hpp"

SimpleResult RequestProcesser::processTargetPctDecoding(Target_t &target)
{
    const Result<std::string> decodingPathResult = RequestPctDecoder::decode(target.path);

    if (decodingPathResult.isFailure())
        return SimpleResult::fail(decodingPathResult.getError());
    if (!RequestPctDecoder::isWellEncoded(target.query))
        return SimpleResult::fail("400 Bad Request");

    target.path = decodingPathResult.getValue();
    return SimpleResult::ok();
}

SimpleResult RequestProcesser::processHostHeaderPctDecoding(headers_t &headers)
{
    const Result<std::string> decodingResult = RequestPctDecoder::decode(headers.at("Host").back());
    if (decodingResult.isFailure())
        return SimpleResult::fail(decodingResult.getError());

    headers["Host"][0] = decodingResult.getValue();
    return SimpleResult::ok();
}

SimpleResult RequestProcesser::processRequestTarget(Target_t &target)
{
    RequestTargetSeparator::separateComponents(target);

    const SimpleResult targetDecodingResult = processTargetPctDecoding(target);
    if (targetDecodingResult.isFailure())
        return SimpleResult::fail(targetDecodingResult.getError());

    RequestTargetNormalizer::normalizePath(target.path);

    return SimpleResult::ok();
}

SimpleResult RequestProcesser::processHeaders(headers_t &headers)
{
    const SimpleResult headerDecodingResult = processHostHeaderPctDecoding(headers);
    if (headerDecodingResult.isFailure())
        return SimpleResult::fail(headerDecodingResult.getError());

    if (headers.find("Transfer-Encoding") != headers.end())
        headers["Transfer-Encoding"][0] = toLower(headers["Transfer-Encoding"][0]);

    if (headers.find("Connection") != headers.end())
        headers["Connection"][0] = toLower(headers["Connection"][0]);

    return SimpleResult::ok();
}
