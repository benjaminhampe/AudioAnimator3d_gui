// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXTENSION_C_GUI_AUDIO_EQUALIZER_H__
#define __IRR_EXTENSION_C_GUI_AUDIO_EQUALIZER_H__

/// Irrlicht-Engine to render this class to
#include <irrlicht.h>

/// uses BenjaminHampe@gmx.de extensions
#include <audio/irrAudio.h>

namespace irr
{
namespace gui
{
	class CGUIAudioEqualizer : public IGUIElement
	{
	public:
		enum {
			AudioEqualizer_PlayButton=23000,
			AudioEqualizer_PauseButton,
			AudioEqualizer_StopButton,
			AudioEqualizer_RecordButton,
			AudioEqualizer_PrevButton,
			AudioEqualizer_NextButton,
			AudioEqualizer_AddTrackButton,
			AudioEqualizer_DelTrackButton,
			AudioEqualizer_EditTrackButton,
			AudioEqualizer_TrackInfoButton
		};

		//! constructor
		CGUIAudioEqualizer( IGUIEnvironment* environment,
					IGUIElement* parent, s32 id, core::rect<s32> rectangle,
					f32 volume = 0.1f, f32 pitch = 1.0f, f32 pan = 0.0f );

		//! destructor
		virtual ~CGUIAudioEqualizer();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! draws the element and its children
		virtual void draw();

		//! getter
        core::stringw getInfoText() const;

	protected:

	private:

		IGUIFont* Font;
		s32 DrawMode;
		scene::E_PRIMITIVE_TYPE PrimType;

		bool LogarithmicXAxis;
		bool LogarithmicYAxis;

		bool DrawAxis;
		bool DrawAxisText;

		video::SColor Color;

		u32 FFTSize;
	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_AUDIO_PLAYER_H_INCLUDED__
