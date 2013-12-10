// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_EXTENSION_C_GUI_AUDIO_PLAYER_H__
#define __IRR_EXTENSION_C_GUI_AUDIO_PLAYER_H__

/// Irrlicht-Engine to render this class to
#include <irrlicht.h>
#include <../source/Irrlicht/CImage.h>

#include <audio/irrAudio.h>

#define MY_DEFAULT_FONT_LARGE "media/fonts/courier22.png"

namespace irr
{
namespace gui
{
	class CGUIAudioPlayer : public IGUIElement
	{
	public:
		enum
		{
			GID_PlayButton=23000,
			GID_PauseButton,
			GID_StopButton,
			GID_RecordButton,
			GID_PrevButton,
			GID_NextButton,
			GID_AddTrackButton,
			GID_DelTrackButton,
			GID_EditTrackButton,
			GID_TrackInfoButton
		};


	private:
		ITimer*						Timer;
		video::ITexture*			BackgroundImage;
		gui::IGUIFont* 				TextFont;	// big font for drawing filename

		video::IColorGradient* 		WAV_Gradient;
		video::IColorGradient* 		FFT_Gradient;

		sf::SoundBuffer 			SoundBuffer;
		sf::Sound 					Sound;
		u32							PlayPosition;
		// u32						PlayDuration;

		/// FourierTransformation
		sfx::FourierTransformRtoC*	FFT_Calculator;		// fft
		core::array<s16> 			FFT_InputBuffer;	// store waveforms here
		core::array<f32> 			FFT_OutputBuffer;	// store spectrum here

		IGUIButton* PlayButton;		// press play
		IGUIButton* PauseButton;		// press pause
		IGUIButton* StopButton;		// press stop
		IGUIButton* RewindButton;		// press rewind
		IGUIButton* ForwardButton;		// press forward
		IGUIButton* PrevButton;		// press previous track
		IGUIButton* NextButton;		// press next track
		// IGUIButton* RecordButton;

		IGUISpinBox* MasterVolume;	// MasterVolume ( Loudness ) 0.0f ... 1.0f
		IGUISpinBox* MasterPan;		// MasterPan ( Centered Sound at 0.0f, -1.0f ... 1.0f
		IGUISpinBox* MasterPitch;		// MasterPitch ( PlaySpeed ) -100.0f ... 100.0f

		//IGUICheckBox* ChkMute;			// Mute ( MasterVolume = 0, panic button )
		//IGUICheckBox* ChkLoopMode;		// LoopMode	( playlist )
		//IGUICheckBox* ChkShuffleMode;	// ShuffleMode ( playlist )
		//
		//IGUIComboBox* AudioInDeviceList;	// list of audio input devices
		//IGUIComboBox* AudioOutDeviceList;	// list of audio output devices
		//
		//IGUIListBox* TrackList; // Playlist
		//IGUIButton* AddTrack;
		//IGUIButton* DelTrack;
		//
		core::stringc FileName; // current played file
		//IGUIComboBox* TrackChannelList;	// list of channels the current track has
		//IGUIStaticText* TrackName; // current played file
		//IGUIStaticText* TrackInfoText; // sample_rate + channel_count + sample_count + sample_count_per_channel
		//IGUIStaticText* TrackPosition; // in milliseconds
		//IGUIStaticText* TrackDuration; // in milliseconds



public:



/// Interface: IGUIElement

	//! value constructor
	CGUIAudioPlayer(
		u32 init_fft_size, /* 8192 */
		ITimer* timer,
		IGUIEnvironment* environment,
		IGUIElement* parent,
		s32 id,
		core::rect<s32> rectangle,
		f32 volume = 0.1f,
		f32 pitch = 1.0f,
		f32 pan = 0.0f );

	//! destructor
	virtual ~CGUIAudioPlayer();

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event);

	//! draws the element and its children
	virtual void draw();

//	//! Writes attributes of the element.
//	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
//
//	//! Reads attributes of the element
//	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);
//

	/// FFT

	virtual u32 getFFTSize() const;

	virtual sfx::FourierTransformRtoC* getFourierTransform();

	//virtual const sfx::FourierTransformRtoC& getFourierTransform() const { return FFT_Calculator; }


	virtual core::stringc getAudioAPIName( ) const
	{
		return core::stringc("SFML 2.0");
	}

	//virtual bool EnumerateDevices( ) { return true; }
	//
	//virtual u32 getAudioDeviceCount( ) const { return 1; }
	//
	//virtual u32 getDefaultAudioDevice( ) const { return 0; }
	//
	//virtual core::stringc getAudioDeviceName( u32 card = 0) const { return core::stringc("SoundBlaster-16 compatible"); }
	//
	//virtual bool setAudioDevice( u32 card = 0) { return true; }


//	virtual io::path createPreviewImageFromWaveforms( const core::dimension2du& img_size, bool bVertical = false ) const;
//
//	virtual io::path createPreviewImageFromPowerSpectrum( const core::dimension2du& img_size, bool bVertical = false ) const;

	///@brief draw wallpaper texture to Screen0
	virtual void drawWallpaper( video::ITexture* tex ) const;

	///@brief draw wallpaper texture to Screen0
	virtual video::ITexture* addTexture( video::IImage* img );

	///@brief create image from soundbuffer-waveform ( 16bit per sample * channel_count * sample_rate = bits per sec = 8*bytes per sec )
	virtual video::IImage* createImageFromSoundBufferWaveform( const core::dimension2du& img_size, const video::SColor& clearColor = video::SColor(64,0,0,0) ) const;

	/// create image from FFT-PowerSpectrum
	virtual video::IImage* createImageFromSoundBufferPowerSpectrum( const core::dimension2du& img_size ) const;



//		/// PreviewTex
//
//		///@return get preview texture that is as drawingsurface for fft-spectrum
//		virtual video::ITexture* getPreviewTexture() { return PreviewTex; }
//
//		///@brief set preview texture to be used by this class for fft-spectrum
//		virtual void setPreviewTexture( video::ITexture* tex = 0 )
//		{
//			video::IVideoDriver* driver = Environment->getVideoDriver();
//
//			if (driver && PreviewTex)
//			{
//				driver->removeTexture( PreviewTex );
//			}
//
//			PreviewTex = tex;
//		}


	/// manage BackGround Texture

	///@return get wallpaper texture that is as drawingsurface for fft-spectrum
	virtual video::ITexture* getBGImage() const;

	///@brief set wallpaper texture to be used by this class for fft-spectrum
	virtual void setBGImage( video::ITexture* tex = 0 );



	/// manage WAV ColorGradient

	///@return get ColorGradient used for drawing fft-spectrum
	virtual video::IColorGradient* getWAVColorGradient() {	return WAV_Gradient; }

	///@brief set ColorGradient used for drawing fft-spectrum
	virtual void setWAVColorGradient( video::IColorGradient* gradient = 0 )
	{
		if (WAV_Gradient)
		{
			if (WAV_Gradient!=gradient)
				WAV_Gradient->drop();
		}

		WAV_Gradient = gradient;
	}



	/// manage FFT ColorGradient

	///@return get ColorGradient used for drawing fft-spectrum
	virtual video::IColorGradient* getFFTColorGradient() {	return FFT_Gradient; }

	///@brief set ColorGradient used for drawing fft-spectrum
	virtual void setFFTColorGradient( video::IColorGradient* gradient = 0 )
	{
		if (FFT_Gradient)
		{
			if (FFT_Gradient!=gradient)
				FFT_Gradient->drop();
		}

		FFT_Gradient = gradient;
	}



	// SoundBuffer
	virtual sf::SoundBuffer& getSoundBuffer()
	{
		return SoundBuffer;
	}

	virtual const sf::SoundBuffer& getSoundBuffer() const
	{
		return SoundBuffer;
	}

	virtual sf::Sound& getSound()
	{
		return Sound;
	}

	virtual const sf::Sound& getSound() const
	{
		return Sound;
	}

	virtual const s16* getSamples()
	{
		return SoundBuffer.getSamples();
	}

	virtual u32 getPlayPosition() { PlayPosition = Sound.getPlayingOffset().asMilliseconds(); return PlayPosition; }

	virtual u32 getPlayDuration() const {	return sfx::getDurationFromSoundBufferInMillis( &SoundBuffer ); }

	virtual u32 getChannelCount( ) const { return SoundBuffer.getChannelCount(); }

	virtual u32 getSampleCount() const { return SoundBuffer.getSampleCount(); }

	virtual u32 getSampleCountPerChannel() const
	{
		if (SoundBuffer.getChannelCount() > 0)
			return SoundBuffer.getSampleCount() / SoundBuffer.getChannelCount();
		else
			return 0;
	}

	virtual u32 getSampleRate() const { return SoundBuffer.getSampleRate(); }

	virtual f32 getPitch() const { return Sound.getPitch(); }

	virtual f32 getVolume() const { return 0.01f*Sound.getVolume(); }

	virtual void setPosition( u32 time_in_ms )
	{
		Sound.setPlayingOffset( sf::milliseconds( time_in_ms ) );
		PlayPosition = Sound.getPlayingOffset().asMilliseconds();
	}

	virtual void setPitch( f32 value = 1.0f ) { Sound.setPitch( value ); }

	virtual void setVolume( f32 value = 0.5f ) { value = core::clamp( 100.0f*value, 0.0f, 100.0f); Sound.setVolume( value ); }




/// manipulate PlayList

	virtual void clearPlayList() {} // { PlayList->clear() }

	virtual u32 getPlayListItemCount() const { return 1; }

	// virtual core::stringc getPlayListItem( u32 i ) { return PlayList[i]; }

	virtual bool loadPlayList( const core::stringc& filename ) { return true; }

	virtual bool savePlayList( const core::stringc& filename ) { return true; }

/// manipulate current Track


	virtual core::stringw createTrackInfo() const;

	virtual bool loadFile( const core::stringc& filename );

	virtual bool saveFile( const core::stringc& filename ) const;

	virtual bool closeFile( ) { return true; }

	virtual void mute() { Sound.setVolume( 0.0f ); }

	virtual void panic() { mute(); stop(); }

	virtual void play( s32 loops = -1 ) { Sound.setLoop( (loops>0) ); Sound.play(); }

	virtual void pause() { Sound.pause(); }

	virtual void resume() { Sound.play(); }

	virtual void stop() { Sound.stop(); }

	virtual void next() { }

	virtual void prev() { }

// Sound-Effect Control
	virtual bool setReverbEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }
	virtual bool setEchoEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }
	virtual bool setFlangerEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }
	virtual bool setCompressorEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }
	virtual bool setDistortionEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }
	virtual bool setChorusEffect( bool bEnable = false, f32 value = 0.0f ) { return false; }


	protected:

};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_AUDIO_PLAYER_H_INCLUDED__
