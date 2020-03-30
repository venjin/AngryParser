#include "document.h"

NS_BEGINE
inline namespace XML
{
	void XMLDocument::print(std::ostream& stream)
	{
		if (hasChildNodes())
		{

			XMLNode* cur = &getFirstChild();
			while (true)
			{

				switch (cur->getType())
				{

				case XMLNodeType::Element:
				{

					auto& element = static_cast<XMLElement&>(*cur);
					auto name = element.getName();
					stream.write("<", 1);
					stream.write(name.getData(), name.getLength());
					for (auto& attr : element.attribute())
					{
						auto attrName = attr.getName();
						auto attrValue = attr.getValue();
						stream.write(" ", 1);
						stream.write(attrName.getData(), attrName.getLength());
						stream.write("=\"", 2);
						stream.write(attrValue.getData(), attrValue.getLength());
						stream.write("\"", 1);
					}
					bool empty = !cur->hasChildNodes();
					{
						if (empty)
							stream.write("/>", 2);
						else
							stream.write(">", 1);
					}
					if (!empty)
					{
						cur = &cur->getFirstChild();
						continue;
					}
					break;
				}
				case XMLNodeType::Text:
				{

					auto& text = static_cast<XMLText&>(*cur);
					auto value = text.getValue();
					stream.write(value.getData(), value.getLength());
					break;
				}
				case XMLNodeType::CDATA:
				{

					auto& cdata = static_cast<XMLCDATA&>(*cur);
					auto value = cdata.getValue();
					stream.write("<![CDATA[", 9);
					stream.write(value.getData(), value.getLength());
					stream.write("]]>", 3);
					break;
				}
				case XMLNodeType::Comment:
				{

					auto& comment = static_cast<XMLComment&>(*cur);
					auto value = comment.getValue();
					stream.write("<!--", 4);
					stream.write(value.getData(), value.getLength());
					stream.write("-->", 3);
					break;
				}
				case XMLNodeType::ProcessingInstruction:
				{

					auto& pi = static_cast<XMLProcessingInstruction&>(*cur);
					auto name = pi.getName();
					auto value = pi.getValue();

					stream.write("<?", 2);
					stream.write(name.getData(), name.getLength());
					stream.write(" ", 1);
					stream.write(value.getData(), value.getLength());
					stream.write("?>", 2);
					break;
				}
				default:
					throw XMLDOMException("Invalid node type");
				}
				while (!cur->next)
				{

					cur = cur->parent;
					if (cur == this)
						break;
					auto name = static_cast<XMLElement*>(cur)->getName();

					stream.write("</", 2);
					stream.write(name.getData(), name.getLength());
					stream.write(">", 1);
				}
				if (cur == this)
					break;
				cur = cur->next;
			}
			stream.flush();
		}
	}

}
NS_END