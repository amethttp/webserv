#include "RequestProcesser.hpp"
#include "utils/string/string.hpp"
#include "WebServer/Client/Request/RequestFactory/RequestPctDecoder/RequestPctDecoder.hpp"

SimpleResult RequestProcesser::processHeaders(HeaderCollection &headers)
{
    const std::string hostValue = headers.getHeaderValue(HOST);

    headers.updateHeader(HOST, RequestPctDecoder::decode(hostValue));

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
