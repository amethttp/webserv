#include "RequestTargetProcesser.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetDecoder/RequestTargetDecoder.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestTargetSeparator/RequestTargetSeparator.hpp"

Result<Target_t> RequestTargetProcesser::process(Target_t &target)
{
    RequestTargetSeparator::separateInComponents(target);

    const Result<Target_t> decodingTargetResult = RequestTargetDecoder::decodeTarget(target);

    if (decodingTargetResult.isFailure())
        return Result<Target_t>::fail(decodingTargetResult.getError());

    target = decodingTargetResult.getValue();

    return Result<Target_t>::ok(target);
}
