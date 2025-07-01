#include "RequestTargetProcesser.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetDecoder/RequestTargetDecoder.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetSeparator/RequestTargetSeparator.hpp"

SimpleResult RequestTargetProcesser::decodeTargetComponents(Target_t &target)
{
    const Result<std::string> decodingPathResult = RequestTargetDecoder::decodePath(target.path);

    if (decodingPathResult.isFailure())
        return SimpleResult::fail(decodingPathResult.getError());
    if (!RequestTargetDecoder::isEncodedQueryValid(target.query))
        return SimpleResult::fail("400 Bad Request");

    target.path = decodingPathResult.getValue();
    return SimpleResult::ok();
}

SimpleResult RequestTargetProcesser::process(Target_t &target)
{
    RequestTargetSeparator::separateComponents(target);

    const SimpleResult targetDecodingResult = decodeTargetComponents(target);
    if (targetDecodingResult.isFailure())
        return SimpleResult::fail(targetDecodingResult.getError());

    return SimpleResult::ok();
}
