//E.I D00244320, E.T D00245315
#include "TextNode.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

TextNode::TextNode(const FontHolder& fonts, std::string& text)
	:m_text(text, fonts.Get(Font::kMain), 20)
{
}

void TextNode::SetString(const std::string& text)
{

	m_text.setString(text);
	Utility::CentreOrigin(m_text);
}

void TextNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_text, states);
}
