#include "RequestProcesser.hpp"
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

SimpleResult RequestProcesser::processRequestTarget(Target_t &target)
{
    RequestTargetSeparator::separateComponents(target);

    const SimpleResult targetDecodingResult = processTargetPctDecoding(target);
    if (targetDecodingResult.isFailure())
        return SimpleResult::fail(targetDecodingResult.getError());

    RequestTargetNormalizer::normalizePath(target.path);

    return SimpleResult::ok();
}
