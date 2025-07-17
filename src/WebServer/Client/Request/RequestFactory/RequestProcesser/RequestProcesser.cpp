#include "RequestProcesser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestPctDecoder/RequestPctDecoder.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetNormalizer/RequestTargetNormalizer.hpp"

SimpleResult RequestProcesser::processRequestLinePctDecoding(RequestLine &requestLine)
{
    if (!RequestPctDecoder::isWellEncoded(requestLine.getTargetPath()))
        return SimpleResult::fail(BAD_REQUEST_ERR);
    if (!RequestPctDecoder::isWellEncoded(requestLine.getTargetQuery()))
        return SimpleResult::fail(BAD_REQUEST_ERR);

    requestLine.setTargetPath(RequestPctDecoder::decode(requestLine.getTargetPath()));
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

SimpleResult RequestProcesser::processRequestTargetNew(RequestLine &requestLine)
{
    const SimpleResult targetDecodingResult = processRequestLinePctDecoding(requestLine);
    if (targetDecodingResult.isFailure())
        return SimpleResult::fail(targetDecodingResult.getError());

    std::string newPath = requestLine.getTargetPath();
    RequestTargetNormalizer::normalizePath(newPath);
    requestLine.setTargetPath(newPath);

    return SimpleResult::ok();
}

SimpleResult RequestProcesser::processRequestTarget(Target_t &target)
{
    RequestLine rql;

    rql.setTargetUri(target.uri);

    const SimpleResult result = processRequestTargetNew(rql);
    if (result.isFailure())
        return result;

    target.uri = rql.getTargetUri();
    target.path = rql.getTargetPath();
    target.query = rql.getTargetQuery();

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
