// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioEqualizer.h"

#include <irrlicht.h>

namespace irr
{
namespace gui
{
//! constructor
CGUIAudioEqualizer::CGUIAudioEqualizer(
	IGUIEnvironment* environment,
	IGUIElement* parent,
	s32 id,
	core::rect<s32> rectangle,
	f32 volume, f32 pitch, f32 pan )
: IGUIElement(EGUIET_ELEMENT, environment, parent, id, rectangle)
{
	setName("CGUIAudioEqualizer");
	setTabStop(true);
	setTabOrder(-1);
//	setNotClipped(noclip);

	video::IVideoDriver* driver = Environment->getVideoDriver();

	IGUISkin* skin = Environment->getSkin();

    IGUIFont* font = skin->getFont( EGDF_DEFAULT );

	/// get coords of this widget
    const core::recti r0 = AbsoluteRect;
        const core::recti r1 = AbsoluteRect;

	const s32 w = AbsoluteRect.getWidth();
	const s32 h = AbsoluteRect.getHeight();

	/// some padding
    const s32 Border = 2;
	const s32 line_height = 24;

}

//! destructor
CGUIAudioEqualizer::~CGUIAudioEqualizer()
{
	// empty
}

//! called if an event happened.
bool CGUIAudioEqualizer::OnEvent(const SEvent& event)
{
	if (!isEnabled())
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
		case EET_KEY_INPUT_EVENT:
			break;

		case EET_GUI_EVENT:{
			gui::IGUIElement* caller = event.GUIEvent.Caller;
			}
			break;

		case EET_MOUSE_INPUT_EVENT:
			break;

		default:
			break;
	}

	//return Parent ? Parent->OnEvent(event) : false;
	//return Parent ? Parent->OnEvent(event) : IGUIElement::OnEvent(event);
	return IGUIElement::OnEvent(event);
}

//! draws the element and its children
void CGUIAudioEqualizer::draw()
{
	if (!IsVisible)
		return;

	// IGUISkin* skin = Environment->getSkin();

	IGUIFont* font = Environment->getSkin()->getFont( EGDF_DEFAULT );


	video::IVideoDriver* driver = Environment->getVideoDriver();

    const core::recti& r0 = AbsoluteRect;
    const core::recti& r1 = AbsoluteClippingRect;

	const s32 w = AbsoluteRect.getWidth();
	const s32 h = AbsoluteRect.getHeight();
//	// todo:	move sprite up and text down if the pressed state has a sprite
//	const core::position2di spritePos = AbsoluteRect.getCenter();
//
//	if (DrawBorder)
//        skin->draw3DButtonPaneStandard(this, AbsoluteRect, &AbsoluteClippingRect);
    driver->draw2DRectangle( video::SColor(64,0,0,255), r0 );

    driver->draw2DLine(
        r0.UpperLeftCorner,
        r0.LowerRightCorner,
        video::SColor(255,255,255,255) );

    driver->draw2DLine(
        core::position2di(r0.LowerRightCorner.X,r0.UpperLeftCorner.Y),
        core::position2di(r0.UpperLeftCorner.X,r0.LowerRightCorner.Y),
        video::SColor(255,255,255,255) );

	IGUIElement::draw();
}


//! get info as stringw
core::stringw CGUIAudioEqualizer::getInfoText() const
{
	core::stringw txt = L"sf::SoundBuffer\n";

//	s32 pos = Filename.findLastChar("/3\\",2);
//	if (pos == -1 )
//	{
//		txt += "filename   = "; txt += Filename.c_str(); txt += "\n";
//
//	}
//	else
//	{
//		u32 cc = Filename.size();
//		txt += "+ filename   = "; txt += Filename.subString(pos + 1,  cc - pos - 1); txt += "\n";
//		txt += "- filepath   = "; txt += Filename.subString(0, pos ); txt += "\n";
//
//	}
//
//    txt += "- samplerate   = "; txt += audio_samplerate; txt += "\n";
//    txt += "- channelCount = "; txt += audio_channels; txt += "\n";
//    txt += "- samplecount  = "; txt += audio_samplecount; txt += "\n";
//    txt += "- duration     = "; txt += audio_duration; txt += "\n";
//    txt += "- offset       = "; txt += audio_offset; txt += "\n";
//    txt += "- volume       = "; txt += audio_volume; txt += "\n";
//    txt += "- pitch        = "; txt += audio_pitch; txt += "\n";
//    txt += "= status       = "; txt += audio_status;
	return txt;
}


} // end namespace gui
} // end namespace irr



