#include "RequestProcesser.hpp"
#include "utils/string/string.hpp"
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

SimpleResult RequestProcesser::processHostHeaderPctDecoding(HeaderCollection &headers)
{
    const std::string hostValue = headers.getHeaderValue("Host");
    const Result<std::string> decodingResult = RequestPctDecoder::decode(hostValue);
    if (decodingResult.isFailure())
        return SimpleResult::fail(decodingResult.getError());

    headers.updateHeader("Host", decodingResult.getValue());
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

SimpleResult RequestProcesser::processHeaders(HeaderCollection &headers)
{
    const SimpleResult headerDecodingResult = processHostHeaderPctDecoding(headers);
    if (headerDecodingResult.isFailure())
        return SimpleResult::fail(headerDecodingResult.getError());

    if (headers.contains("Transfer-Encoding"))
    {
        const std::string transferEncodingNewValue = toLower(headers.getHeaderValue("Transfer-Encoding"));
        headers.updateHeader("Transfer-Encoding", transferEncodingNewValue);
    }

    if (headers.contains("Connection"))
    {
        const std::string connectionNewValue = toLower(headers.getHeaderValue("Connection"));
        headers.updateHeader("Connection", connectionNewValue);
    }

    return SimpleResult::ok();
}
