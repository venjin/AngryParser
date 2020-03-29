#ifndef _HANDLER_HPP
#define _HANDLER_HPP

#include "../Core/compilerdetection.h"
#include "../Core/string.h"

NS_BEGINE
inline namespace XML
{

class AngryParser_API XMLHandlerBase
{
public:
    void startDocument() {}
    void endDocument() {}
    void startElement(StringView /*name*/) {}
    void endElement(StringView /*name*/) {}
    void endAttributes(bool /*empty*/) {}
    void doctype() {}
    void attribute(StringView /*name*/, StringView /*value*/) {}
    void text(StringView /*value*/) {}
    void cdata(StringView /*value*/) {}
    void comment(StringView /*value*/) {}
    void processingInstruction(StringView /*name*/, StringView /*value*/) {}
};

} // namespace XML
NS_END

#endif
