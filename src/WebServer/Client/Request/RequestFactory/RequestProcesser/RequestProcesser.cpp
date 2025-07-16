#include "RequestProcesser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestPctDecoder/RequestPctDecoder.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetSeparator/RequestTargetSeparator.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetNormalizer/RequestTargetNormalizer.hpp"

SimpleResult RequestProcesser::processTargetPctDecoding(Target_t &target)
{
    if (!RequestPctDecoder::isWellEncoded(target.path))
        return SimpleResult::fail(BAD_REQUEST_ERR);
    if (!RequestPctDecoder::isWellEncoded(target.query))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    target.path = RequestPctDecoder::decode(target.path);
    return SimpleResult::ok();
}

SimpleResult RequestProcesser::processHostHeaderPctDecoding(HeaderCollection &headers)
{
    const std::string hostValue = headers.getHeaderValue(HOST);

    if (!RequestPctDecoder::isWellEncoded(hostValue))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    headers.updateHeader(HOST, RequestPctDecoder::decode(hostValue));
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

    if (headers.contains(TRANSFER_ENCODING))
    {
        const std::string transferEncodingNewValue = toLower(headers.getHeaderValue(TRANSFER_ENCODING));
        headers.updateHeader(TRANSFER_ENCODING, transferEncodingNewValue);
    }

    if (headers.contains(CONNECTION))
    {
        const std::string connectionNewValue = toLower(headers.getHeaderValue(CONNECTION));
        headers.updateHeader(CONNECTION, connectionNewValue);
    }

    return SimpleResult::ok();
}
