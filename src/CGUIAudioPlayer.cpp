// Copyright (C) 2002-2013 Benjamin Hampe
// This file is part of the "irrlicht-engine"
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIAudioPlayer.h"

#include <video/CLinearColorGradient.h>

namespace irr
{
namespace gui
{

///@brief draw wallpaper texture to Screen
void CGUIAudioPlayer::drawWallpaper( video::ITexture* tex ) const
{
	if (!tex)
		return;

	video::IVideoDriver* driver = Environment->getVideoDriver();

	if (!driver)
		return;

	const core::dimension2du& scr_size = driver->getScreenSize();
	const core::dimension2du& tex_size = tex->getSize();

	const f32 aspect = (f32)tex_size.Height / (f32)tex_size.Width;
	const s32 height = aspect * scr_size.Width;
	const s32 y = ( (s32)scr_size.Height - height ) / 2;

	const core::recti r_dst( 0, y, scr_size.Width - 1, y + height );
	const core::recti r_src( core::position2di(0,0), tex_size );

	driver->draw2DImage( tex, r_dst, r_src );
}

//! constructor
CGUIAudioPlayer::CGUIAudioPlayer(
	u32 init_fft_size, /* 8192 */
	ITimer* timer,
	IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle,
	f32 volume, f32 pitch, f32 pan )

: IGUIElement(EGUIET_ELEMENT, environment, parent, id, rectangle)
, Timer(timer)
, BackgroundImage(0)
, WAV_Gradient(0)
, FFT_Gradient(0)
, TextFont(0)
, PlayPosition(0)
, FFT_Calculator(0)
, MasterVolume(0)
, MasterPan(0)
, MasterPitch(0)
//, AudioOutDeviceList(0),
, PlayButton(0), PauseButton(0), StopButton(0), RewindButton(0), ForwardButton(0), PrevButton(0), NextButton(0)
, FileName("")
//, TrackList(0), AddTrack(0), DelTrack(0)
//, TrackChannelList(0), TrackName(0), TrackInfoText(0), TrackPosition(0), TrackDuration(0)
//, ChkMute(0), ChkLoopMode(0), ChkShuffleMode(0)
//, IsEchoEffect(0), IsCompressorEffect(0), IsReverbEffect(0), IsChorusEffect(0), IsDistortionEffect(0), IsFlangerEffect(0)
{
	setName("CGUIAudioPlayer");
	setTabStop(true);
	setTabOrder(-1);
	// setNotClipped(noclip);

	const s32 w = AbsoluteRect.getWidth();
	const s32 h = AbsoluteRect.getHeight();
    const s32 Border = 2;
	const s32 line_height = 24;

    core::stringw txt;
    core::dimension2du txt_size;

	s32 ex = 0;
    s32 ey = 0;
	s32 x = Border;
	s32 y = Border;
	s32 dx = 0;
	s32 dy = line_height;

	// #ifdef DEBUG
	printf( "CGUIAudioPlayer::create fft colorgradients()\n" );
	// #endif // DEBUG

	/// ColorGradient for waveforms
	WAV_Gradient = new video::CLinearColorGradient();
	if (WAV_Gradient)
	{
//		WAV_Gradient->addColor( video::SColor(255,255,255,255), 0.0f );
//		WAV_Gradient->addColor( video::SColor(255,255,255,255), 1.0f );
		WAV_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
		WAV_Gradient->addColor( video::SColor(255,0,0,255), 0.25f );
		WAV_Gradient->addColor( video::SColor(255,0,255,0), 0.50f );
		WAV_Gradient->addColor( video::SColor(255,255,255,0), 0.75f );
		WAV_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
	}

	// #ifdef DEBUG
	// printf( "CGUIAudioPlayer::create fft colorgradients()\n" );
	// #endif // DEBUG

	/// ColorGradient for waveforms
	FFT_Gradient = new video::CLinearColorGradient();
	if (FFT_Gradient)
	{
	/// black
//		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
//		FFT_Gradient->addColor( video::SColor(255,25,25,25), 0.25f );
//		FFT_Gradient->addColor( video::SColor(255,75,75,75), 0.50f );
//		FFT_Gradient->addColor( video::SColor(255,95,95,95), 0.60f );
//		FFT_Gradient->addColor( video::SColor(255,115,115,115), 0.70f );
//		FFT_Gradient->addColor( video::SColor(255,155,155,155), 0.80f );
//		FFT_Gradient->addColor( video::SColor(255,255,255,255), 0.90f );
//		FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.95f );
//		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
		FFT_Gradient->addColor( video::SColor(255,25,25,25), 0.10f );
		FFT_Gradient->addColor( video::SColor(255,50,50,50), 0.20f );
		FFT_Gradient->addColor( video::SColor(255,75,75,75), 0.30f );
		FFT_Gradient->addColor( video::SColor(255,100,100,100), 0.40f );
		FFT_Gradient->addColor( video::SColor(255,125,125,125), 0.45f );
		//FFT_Gradient->addColor( video::SColor(255,0,0,100), 0.5f );
		FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.50f );
		FFT_Gradient->addColor( video::SColor(255,0,200,0), .60f );
		FFT_Gradient->addColor( video::SColor(255,255,255,0), .80f );
		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.0f );
		//FFT_Gradient->addColor( video::SColor(255,255,255,255), 1.0f );

	/// blue

	//	FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.00f );
	//	FFT_Gradient->addColor( video::SColor(255,25,25,50), 0.25f );
	//	FFT_Gradient->addColor( video::SColor(255,50,50,75), 0.50f );
	//	FFT_Gradient->addColor( video::SColor(255,75,75,95), 0.60f );
	//	FFT_Gradient->addColor( video::SColor(255,95,95,155), 0.70f );
	//	FFT_Gradient->addColor( video::SColor(255,105,105,255), 0.80f );
	//	FFT_Gradient->addColor( video::SColor(255,255,255,255), 0.90f );
	//	FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.95f );
	//	FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );

	/// blue1
//		FFT_Gradient->addColor( video::SColor(255,0,0,0), 0.00f );
//		FFT_Gradient->addColor( video::SColor(255,0,0,255), 0.25f );
//		FFT_Gradient->addColor( video::SColor(255,0,255,0), 0.50f );
//		FFT_Gradient->addColor( video::SColor(255,255,255,0), 0.75f );
//		FFT_Gradient->addColor( video::SColor(255,255,0,0), 1.00f );
	}

	// #ifdef DEBUG
	// printf( "load fonts.\n" );
	// #endif // DEBUG

	video::IVideoDriver* driver = Environment->getVideoDriver();

	TextFont = Environment->getBuiltInFont();
	if (!TextFont)
	{
		// #ifdef DEBUG
		printf( "no default font available.\n" );
		// #endif // DEBUG
	}


	/// Buttons

	// #ifdef DEBUG
	// printf( "add buttons\n" );
	// #endif // DEBUG

	dx = 32;
	dy = dx;
	y = Border;
	x = core::floor32( 0.5f*(f32)w-3.5f*dx );

	PlayButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"|>");	x+=dx;
	PauseButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"||");x+=dx;
    StopButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"[]"); x+=dx;
	RewindButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<<"); x+=dx;
	ForwardButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">>"); x+=dx;
	PrevButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"<"); x+=dx;
	NextButton = Environment->addButton( core::recti(x,y,x+dx-1,y+dy-1), this, -1, L">"); x+=dx;


	/// Volume
	//	txt = L"Volume"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	MasterVolume = Environment->addSpinBox( L"0.10", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterVolume)
	{
		MasterVolume->setDecimalPlaces(2);
		MasterVolume->setRange(0,1);
		MasterVolume->setStepSize(0.01f);
		MasterVolume->setValue( volume );
	}
	x+=100; // y+=dy;

	/// Pan
	// txt = L"Panning"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	MasterPan = Environment->addSpinBox( L"0.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPan)
	{
		MasterPan->setDecimalPlaces(2);
		MasterPan->setRange(-1.f,1.f);
		MasterPan->setStepSize(.05f);
		MasterPan->setValue(0.f);
	}
	x+=100; // y+=dy;

	/// Speed / Pitch
	//    txt = L"Speed"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//    Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x+ex,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	MasterPitch = Environment->addSpinBox( L"1.00", core::recti(x,y,x+dx-1,y+dy-1), true, this, -1);
	if (MasterPitch)
	{
		MasterPitch->setDecimalPlaces(3);
		MasterPitch->setRange(0.001f,1000.0f);
		MasterPitch->setStepSize(0.1f);
		MasterPitch->setValue(1.0f);
	}
	x+=100; // y+=dy;

	/// AudioDevices
	//    txt = L"ChannelList"; txt_size = font->getDimension( txt.c_str() );
	//    ey = (dy-(s32)txt_size.Height)/2;
	//	dx = txt_size.Width;
	//	Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x,y+ey), txt_size), false, false, this, -1);
	//	y+=dy;

	//COLUMN 2
	//	y = Border;
	//	x = 200;
	//	dx = 200;
	//
	/// Mute
	//	ChkMute = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Mute"); y+=dy;
	/// ShuffleMode
	//	ChkShuffleMode = Environment->addCheckBox( false, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Shuffle"); y+=dy;
	/// LoopMode
	//	ChkLoopMode = Environment->addCheckBox( true, core::recti(x,y,x+dx-1,y+dy-1), this, -1, L"Loop"); y+=dy;

	/// TrackInfo
	//
	//	TrackName = Environment->addStaticText( txt.c_str(), core::recti(core::position2di(x,y+ey), txt_size), false, false, this, -1);
	//  ChannelList = Environment->addComboBox( core::recti(x,y,x+dx-1,y+dy-1), this, -1 ); y+=dy;

	// STATIC LABEL - Audio Information Text
	//	txt = getSoundInfoText();
	//
	//	txt_size = arial->getDimension( txt.c_str() );
	//	txt_size = font->getDimension( txt.c_str() );
	//
	//	x = w - txt_size.Width - Border;
	//	y = h - txt_size.Height - Border;
	//
	//	TrackInfo = Environment->addStaticText( txt.c_str(), core::recti( core::position2di(x,y), txt_size), true, false, this, -1, false );
	//
	//	if (TrackInfo)
	//	{
	////		TrackInfo->setOverrideFont( arial );
	//		TrackInfo->setOverrideFont( font );
	//		TrackInfo->setOverrideColor( video::SColor(255,255,255,255) );
	//	}

	/// FFT
	FFT_Calculator = new sfx::FourierTransformRtoC( init_fft_size );
	if (!FFT_Calculator)
	{
		printf( "Could not create FFT_Calculator\n" );
	}

}

//! destructor
CGUIAudioPlayer::~CGUIAudioPlayer()
{
	if (FFT_Calculator)
	{
		FFT_Calculator->drop();
		FFT_Calculator = 0;
	}
}

//! FFT
u32 CGUIAudioPlayer::getFFTSize() const
{
	return (FFT_Calculator)?(FFT_Calculator->getFFTSize()):0;
}

//! FFT
sfx::FourierTransformRtoC* CGUIAudioPlayer::getFourierTransform()
{
	return FFT_Calculator;
}

//! get info text from currently played/active Track
core::stringw CGUIAudioPlayer::createTrackInfo() const
{
	core::stringw txt = L"";

	#ifdef DEBUG
	printf( "CGUIAudioPlayer::createTrackInfo()\n" );
	#endif // DEBUG

    const u32 sample_rate = SoundBuffer.getSampleRate();
    const u32 channel_count = SoundBuffer.getChannelCount();
    const f32 play_duration = sfx::getDurationFromSoundBufferInMillis( &SoundBuffer );
    const u32 sample_count = SoundBuffer.getSampleCount();
    const bool isLooped = Sound.getLoop();
	const u32 play_position = Sound.getPlayingOffset().asMilliseconds();
    const sf::Sound::Status play_status = Sound.getStatus();

	s32 pos = FileName.findLastChar("/3\\",2);
	if (pos == -1 )
	{
		txt += "filename   = "; txt += FileName.c_str(); txt += "\n";

	}
	else
	{
		u32 cc = FileName.size();
		txt += "+ filename   = "; txt += FileName.subString(pos + 1,  cc - pos - 1); txt += "\n";
		txt += "- filepath   = "; txt += FileName.subString(0, pos ); txt += "\n";

	}

    txt += "- samplerate   = "; txt += sample_rate; txt += "\n";
    txt += "- channelCount = "; txt += channel_count; txt += "\n";
    txt += "- samplecount  = "; txt += sample_count; txt += "\n";
    txt += "- duration     = "; txt += play_duration; txt += "\n";
    txt += "- offset       = "; txt += play_position; txt += "\n";
    txt += "= status       = "; txt += play_status;
	return txt;
}



/// Const function operation on const class internal textures
///@return get wallpaper texture that is as drawingsurface for fft-spectrum
video::ITexture* CGUIAudioPlayer::getBGImage() const
{
	return BackgroundImage;
}

///@brief set wallpaper texture to be used by this class for fft-spectrum
void CGUIAudioPlayer::setBGImage( video::ITexture* tex )
{
//	video::IVideoDriver* driver = Environment->getVideoDriver();
//
//	if (driver && BackgroundImage)
//	{
//		driver->removeTexture( BackgroundImage );
//	}
//
//	BackgroundImage = tex;
}

///@brief draw wallpaper texture to Screen0
video::ITexture* CGUIAudioPlayer::addTexture( video::IImage* img )
{
	if (!img)
	{
		#ifdef _DEBUG
		printf( "No valid pointer to IImage\n");
		#endif // _DEBUG
		return 0;
	}

//	if (!Timer)
//	{
//		#ifdef _DEBUG
//		printf( "No valid pointer to ITimer\n");
//		#endif // _DEBUG
//		return 0;
//	}
//
	video::IVideoDriver* driver = Environment->getVideoDriver();

//	if (!driver)
//	{
//		#ifdef _DEBUG
//		printf( "No valid pointer to IVideoDriver\n");
//		#endif // _DEBUG
//		return 0;
//	}
//
//	const core::dimension2du& img_size = img->getDimension();
//
//	const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
//
//	io::path name = _IRR_TEXT("tex_");
//	name += img_size.Width;	name += _IRR_TEXT("x");
//	name += img_size.Height;name += _IRR_TEXT("x");
//	name += img_bpp;
//	if (Timer)
//	{
//		name += currentDateString( Timer );	name += _IRR_TEXT("_");
//		name += Timer->getRealTime(); name += _IRR_TEXT("_");
//	}
//	name += _IRR_TEXT(".png");

	driver->setTextureCreationFlag( video::ETCF_OPTIMIZED_FOR_QUALITY, true );
	driver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, true );
	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );

	video::ITexture* tex = driver->addTexture( "", img );
	if (!tex)
	{
		#ifdef _DEBUG
		printf( "No valid pointer to ITexture\n");
		#endif // _DEBUG
	}

	driver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, true );

	return tex;
}

///@brief create image from soundbuffer-waveform ( 16bit per sample * channel_count * sample_rate = bits per sec = 8*bytes per sec )
video::IImage* CGUIAudioPlayer::createImageFromSoundBufferWaveform( const core::dimension2du& img_size, const video::SColor& clearColor ) const
{
	#ifdef DEBUG
	printf( "CGUIAudioPlayer::createImageFromSoundBufferWaveform( %d, %d )\n", img_size.Width, img_size.Height );
	#endif // DEBUG

	video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
	if (!img)
	{
		return 0;
	}
	#ifdef _DEBUG
		const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
		printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
	#endif // _DEBUG

	//! fill image pixels to fully transparent ( alpha == 0 )
	img->fill( clearColor );

	//! make place for a change in drawing positions
	const u32 d = core::round32( (f32)img_size.Height / (f32)SoundBuffer.getChannelCount() );
	const core::dimension2du small_size( img_size.Width, d );
	const s32 x = 0;
	s32 y = 0;

	/// loop channels
	for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
	{
		sfx::drawAudioWaveformFast( img, core::recti( core::position2di(x,y), small_size ), WAV_Gradient, &SoundBuffer, channel_index, false );
		y += d;
	}


	return img;
}

/// create image from FFT-PowerSpectrum
video::IImage* CGUIAudioPlayer::createImageFromSoundBufferPowerSpectrum( const core::dimension2du& img_size ) const
{
	#ifdef DEBUG
	printf( "CGUIAudioPlayer::createImageFromSoundBufferPowerSpectrum( %d, %d )\n", img_size.Width, img_size.Height );
	#endif // DEBUG

	video::CImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
	if (!img)
	{
		return 0;
	}

	#ifdef _DEBUG
		const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
		printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
	#endif // _DEBUG

	//! fill image pixels to fully transparent ( alpha == 0 )
	img->fill( video::SColor(200,0,0,0) );

	//! make place for a change in drawing positions

	// const u32 d = core::round32( (f32)img_size.Height / (f32)my_channel_count );
	// const core::dimension2du small_size( img_size.Width, d );
	s32 x = 0;
	s32 y = 0;

	/// loop channels
	// for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
	// {

		/// draw spectrum
		sfx::drawAudioSpectrum( img, core::recti(core::position2di(x,y),img_size), FFT_Gradient, &SoundBuffer, 0);
	//	y += d;
	// }

	return img;
}




bool CGUIAudioPlayer::loadFile( const core::stringc& filename )
{
	// #ifdef DEBUG
	printf( "CGUIAudioPlayer::loadFile()\n" );
	// #endif // DEBUG

	const core::recti& r0 = AbsoluteRect;
	const core::recti& r1 = AbsoluteClippingRect;

	video::IVideoDriver* driver = Environment->getVideoDriver();

	/// SFML Audio
	FileName = filename;
	if (!SoundBuffer.loadFromFile( FileName.c_str() ) )
	{
		printf("Could not load SoundBuffer from file %s\n", FileName.c_str() );
		return false;
	}

	const u32 my_sample_rate = SoundBuffer.getSampleRate();
	const u32 my_sample_count = SoundBuffer.getSampleCount();
	const u32 my_channel_count = SoundBuffer.getChannelCount();
	const u32 my_duration = sfx::getDurationFromSoundBufferInMillis( &SoundBuffer );
	u32 my_position = 0;
	f32 my_volume = 10.0f;		// 10 %
	f32 my_pitch = 1.0f; 		// normal play-speed
	f32 my_pan = 0.0f; 		// normal pan ( middle )

	Sound.setBuffer( SoundBuffer );
	Sound.setVolume( my_volume );
	Sound.setPitch( my_pitch );
	Sound.setPlayingOffset( sf::milliseconds( my_position ) );
	//	s16* my_ptr = 0;
	//	u32 my_sample_delta = 0;
	//	f32 my_time_delta = 0.0f;	// in seconds
//
//	//! create waveform preview image
//	//! load fft-power-spectrum background-texture
//	video::IImage* wavImage = createImageFromSoundBufferWaveform( core::dimension2du(640,480) );
//	//	video::IImage* fftImage = createImageFromSoundBufferPowerSpectrum( core::dimension2du(2*640,2*480) );
//
//	if (!wavImage)	{
//		printf( "ELL_ERROR - Could not create waveform image\n" );
//	}
//
//	//	if (!fftImage)	{
//	//		printf( "ELL_ERROR - Could not create fft-powerspectrum image\n" );
//	//	}
//
//	video::ITexture* bgTexture = addTexture( wavImage );
//
//	if (!bgTexture)	{
//		printf( "ELL_ERROR - Could not create bg-texture\n" );
//	}
//
//	this->setBGImage( bgTexture );
//
//	if (wavImage)
//		wavImage->drop();

	//	Sound.play();
	printf( "CGUIAudioPlayer::loadFile() - OK\n" );
	return true;
}




bool CGUIAudioPlayer::saveFile( const core::stringc& filename ) const
{
	// #ifdef DEBUG
	printf( "CGUIAudioPlayer::saveFile()\n" );
	// #endif // DEBUG

	SoundBuffer.saveToFile( filename.c_str() );
	return true;
}



//! draws the element and its children
void CGUIAudioPlayer::draw()
{
	// #ifdef DEBUG
	// printf( "CGUIAudioPlayer::draw()\n" );
	// #endif // DEBUG

	if (!IsVisible)
		return;
//
//	const s32 w = AbsoluteRect.getWidth();
//	const s32 h = AbsoluteRect.getHeight();
//
//	PlayPosition = Sound.getPlayingOffset().asMilliseconds();
//
//	const u32 PlayDuration = sfx::getDurationFromSoundBufferInMillis( &SoundBuffer );
//
//	video::IVideoDriver* driver = Environment->getVideoDriver();
//
//	const core::dimension2du& screenSize = driver->getScreenSize();
//
////	if (DrawBorder)
////        skin->draw3DButtonPaneStandard(this, AbsoluteRect, &AbsoluteClippingRect);
//
//
//	if (BackgroundImage)
//	{
//		driver->draw2DImage( BackgroundImage, AbsoluteRect, core::recti( core::position2di(0,0), BackgroundImage->getSize() ) );
//	}
//	else
//	{
//		/// draw BoundingRect r0
//		driver->draw2DRectangle( video::SColor(64,0,0,255), AbsoluteRect );
//
//		/// draw white test-line
//		driver->draw2DLine(
//			AbsoluteRect.UpperLeftCorner,
//			AbsoluteRect.LowerRightCorner,
//			video::SColor(255,255,255,255) );
//
//		/// draw white test-line
//		driver->draw2DLine(
//			core::position2di(AbsoluteRect.LowerRightCorner.X,AbsoluteRect.UpperLeftCorner.Y),
//			core::position2di(AbsoluteRect.UpperLeftCorner.X,AbsoluteRect.LowerRightCorner.Y),
//			video::SColor(255,255,255,255) );
//	}
//
////	u32 frameCount = r0.getWidth();
////
////
///// draw red line as time_cursor
//
//	const s32 d = core::round32( (f32)w * (f32)PlayPosition / (f32)PlayDuration );
//
//	s32 x = core::s32_clamp( d, 0, w );
//
//	x += AbsoluteRect.UpperLeftCorner.X;
//
//	s32 y0 = AbsoluteRect.UpperLeftCorner.Y;
//	s32 y1 = AbsoluteRect.LowerRightCorner.Y;
//
//	const video::SColor red(255,255,0,0);
//
//	driver->draw2DLine( core::position2di( x-1, y0 ), core::position2di( x-1,y1 ), red);
//	driver->draw2DLine( core::position2di( x,   y0 ), core::position2di( x,  y1 ), red);
//	driver->draw2DLine( core::position2di( x+1, y0 ), core::position2di( x+1,y1 ), red);
//
//	IGUIElement::draw();
//
//	if (TextFont)
//	{
//		video::SColor TextColor(255,255,0,0);
//		core::stringw txt;
//		core::dimension2du txt_size;
//		core::recti txt_rect;
//
//		s32 x = AbsoluteRect.UpperLeftCorner.X+10;
//		s32 y = AbsoluteRect.UpperLeftCorner.Y+10;
//		s32 dy = 20;
//
//		txt = L""; txt += FileName;
//		txt_size = TextFont->getDimension( txt.c_str() );
//		txt_rect = core::recti( core::position2di(x,y), txt_size );
//		txt_rect.clipAgainst( AbsoluteClippingRect );
//		TextFont->draw( txt, txt_rect, TextColor, false, false);
//		y += dy;
//
//		txt = L"ChannelCount = "; txt += getChannelCount(); txt += L"\n";
//		txt += L"SampleRate = "; txt += getSampleRate(); txt += L"\n";
//		txt += L"SampleCount = "; txt += getSampleCount(); txt += L"\n";
//		txt_size = TextFont->getDimension( txt.c_str() );
//		txt_rect = core::recti( core::position2di(x,y), txt_size );
//		txt_rect.clipAgainst( AbsoluteClippingRect );
//		TextFont->draw( txt, txt_rect, TextColor, false, false);
//		y += dy;
//
//		txt = L"Position = "; txt += getHMS_fromMillis( PlayPosition );
//		txt_size = TextFont->getDimension( txt.c_str() );
//		y = AbsoluteRect.LowerRightCorner.Y - 10 - (s32)txt_size.Height;
//		txt_rect = core::recti( core::position2di(x,y), txt_size );
//		txt_rect.clipAgainst( AbsoluteClippingRect );
//		TextFont->draw( txt, txt_rect, TextColor, false, false);
//
//		txt = L"Duration = "; txt += getHMS_fromMillis( getPlayDuration() );
//		txt_size = TextFont->getDimension( txt.c_str() );
//		x = AbsoluteRect.LowerRightCorner.X - 10 - (s32)txt_size.Width;
//		y = AbsoluteRect.LowerRightCorner.Y - 10 - (s32)txt_size.Height;
//		txt_rect = core::recti( core::position2di(x,y), txt_size );
//		txt_rect.clipAgainst( AbsoluteClippingRect );
//		TextFont->draw( txt, txt_rect, TextColor, false, false);
//	}


//	if (LargeFont)
//	{
//		core::stringw txt(L"AudioFile: "); txt += FileName;
//		core::dimension2du txt_size = font->getDimension( txt.c_str() );
//		s32 x = 10;
//		s32 y = ScreenSize.Height - txt_size.Height - 20 - core::round32( (f32)ScreenSize.Height / ( 2.0f * my_channel_count ) );
//		// driver->draw2DRectangle( video::SColor(127,0,0,0), )
//		big_font->draw( txt, core::recti( core::position2di(x-2, y+2), txt_size), 0xffffffff, false, false);
//		big_font->draw( txt, core::recti( core::position2di(x+2, y-2), txt_size), 0xffffffff, false, false);
//		big_font->draw( txt, core::recti( core::position2di(x-2, y-2), txt_size), 0xffffffff, false, false);
//		big_font->draw( txt, core::recti( core::position2di(x+2, y+2), txt_size), 0xffffffff, false, false);
//		big_font->draw( txt, core::recti( core::position2di(x, y), txt_size), video::SColor(255,0,0,0), false, false);
//	}
	IGUIElement::draw();
}

//
////! Writes attributes of the element.
//void CGUIAudioPlayer::serializeAttributes( io::IAttributes* out, io::SAttributeReadWriteOptions* options ) const
//{
//	IGUIElement::serializeAttributes(out,options);
//
//	//	out->addBool	("PushButton",		IsPushButton );
//	//	if (IsPushButton)
//	//		out->addBool("Pressed",		Pressed);
//	//
//	//	out->addTexture ("Image",		Image);
//	//	out->addRect	("ImageRect",		ImageRect);
//	//	out->addTexture	("PressedImage",	PressedImage);
//	//	out->addRect	("PressedImageRect",	PressedImageRect);
//	//	out->addBool	("UseAlphaChannel",	isAlphaChannelUsed());
//	//	out->addBool	("Border",		isDrawingBorder());
//	//	out->addBool	("ScaleImage",		isScalingImage());
//	//  out->addString  ("OverrideFont",	OverrideFont);
//}
//
////! Reads attributes of the element
//void CGUIAudioPlayer::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
//{
//	IGUIElement::deserializeAttributes(in,options);
//
//	//	IsPushButton	= in->getAttributeAsBool("PushButton");
//	//	Pressed		= IsPushButton ? in->getAttributeAsBool("Pressed") : false;
//	//
//	//	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
//	//	if (rec.isValid())
//	//		setImage( in->getAttributeAsTexture("Image"), rec);
//	//	else
//	//		setImage( in->getAttributeAsTexture("Image") );
//	//
//	//	rec = in->getAttributeAsRect("PressedImageRect");
//	//	if (rec.isValid())
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage"), rec);
//	//	else
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage") );
//	//
//	//	setDrawBorder(in->getAttributeAsBool("Border"));
//	//	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));
//	//	setScaleImage(in->getAttributeAsBool("ScaleImage"));
//	//  setOverrideFont(in->getAttributeAsString("OverrideFont"));
//
//	updateAbsolutePosition();
//}

//! called if an event happened.
bool CGUIAudioPlayer::OnEvent(const SEvent& event)
{
////	if (!isEnabled())
//	return IGUIElement::OnEvent(event);
//
//	switch(event.EventType)
//	{
//		case EET_KEY_INPUT_EVENT:
//		{
////		if (event.KeyInput.PressedDown &&
////			(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
////		{
////			return true;
////		}
////
////		else
////		if (!event.KeyInput.PressedDown && (event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
////		{
////			if (Parent)
////			{
////				SEvent newEvent;
////				newEvent.EventType = EET_GUI_EVENT;
////				newEvent.GUIEvent.Caller = this;
////				newEvent.GUIEvent.Element = 0;
////				newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
////				Parent->OnEvent(newEvent);
////			}
////			return true;
////		}
//	}
//		break;
//
//	case EET_GUI_EVENT:
//	{
//        gui::IGUIElement* caller = event.GUIEvent.Caller;
//
////		if ( this->isMyChild(caller) )
////		{
//			if ( event.GUIEvent.EventType == EGET_BUTTON_CLICKED )
//			{
//				if (caller == PlayButton)
//				{
//					Sound.play();
////					if (Player)
////                        Player->play();
//					return true;
//				}
//
//				else if (caller == PauseButton)
//				{
//					Sound.pause();
////                    if (Player)
////                        Player->pause();
//					return true;
//				}
//
//				else if (caller == StopButton)
//				{
//					Sound.stop();
////					if (Player)
////                        Player->stop();
//					return true;
//				}
//
////				else if (caller == ButtonResume)
////				{
////                    if (Player)
////                        Player->resume();
////					return true;
////				}
////				else if (caller == ButtonRecord)
////				{
////					if (Player)
////                        Player->record();
////					return true;
////				}
//
//				else if (caller == RewindButton)
//				{
//					Sound.stop();
//					Sound.setPlayingOffset( sf::milliseconds( 0 ) );
////					if (Player)
////                        Player->setPlaySpeed( Player->getPlaySpeed() / 1.5f );
//					return true;
//				}
//
//				else if (caller == ForwardButton)
//				{
//					Sound.stop();
//					Sound.setPlayingOffset( sf::milliseconds( 0 ) );
////					if (Player)
////                        Player->setPlaySpeed( Player->getPlaySpeed() * 1.5f );
//					return true;
//				}
//
//				else if (caller == PrevButton)
//				{
//					Sound.stop();
//					Sound.setPlayingOffset( sf::milliseconds( 0 ) );
////					if (Player)
////                        Player->prev();
//					return true;
//				}
//
//				else if (caller == NextButton)
//				{
//					Sound.stop();
//					Sound.setPlayingOffset( sf::milliseconds( 0 ) );
////                    if (Player)
////                        Player->next();
//					return true;
//				}
//			}
//
//			if ( event.GUIEvent.EventType == EGET_SPINBOX_CHANGED )
//			{
//				if (caller == MasterVolume)
//				{
//					f32 value = core::clamp( 100.0f * MasterVolume->getValue(), 0.0f, 100.0f );
//					Sound.setVolume( value );
//					return true;
//				}
//
//				if (caller == MasterPan)
//				{
//					f32 value = core::clamp( MasterPan->getValue(), -1.0f, 1.0f );
////					Sound.set( value );
//					return true;
//				}
//				if (caller == MasterPitch)
//				{
//					f32 value = core::clamp( MasterPitch->getValue(), 0.0f, 100.0f );
//					Sound.setPitch( value );
//					return true;
//				}
//			}
//
//			if ( event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED )
//			{
////				gui::IGUIElement* caller = event.GUIEvent.Caller;
////				if (caller == ChkMute )
////				{
////                    Sound.setVolume( 0.0f );
////                    MasterVolume->setValue( 0.0f );
////					return true;
////				}
////				if (caller == ChkLoopMode )
////				{
////					return true;
////				}
////				if (caller == ChkShuffleMode )
////				{
////					return true;
////				}
////				if (caller == IsCompressorEffect)
////				{
////					return true;
////				}
////				if (caller == IsDistortionEffect)
////				{
////					return true;
////				}
////				if (caller == IsEchoEffect)
////				{
////					return true;
////				}
////				if (caller == IsFlangerEffect)
////				{
////					return true;
////				}
////				if (caller == IsReverbEffect)
////				{
////					return true;
////				}
//			}
//	} break;
//
//	case EET_MOUSE_INPUT_EVENT:
////		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
////		{
////			if (Environment->hasFocus(this) && !AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
////			{
////				Environment->removeFocus(this);
////				return false;
////			}
////			Environment->setFocus(this);
////			return true;
////		}
////		else
////		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
////		{
////			if ( !AbsoluteClippingRect.isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y ) ) )
////			{
////				return true;
////			}
////
////			return true;
////		}
//		break;
//	default:
//		break;
//	}

	//return Parent ? Parent->OnEvent(event) : false;
	//return Parent ? Parent->OnEvent(event) : IGUIElement::OnEvent(event);
	return IGUIElement::OnEvent(event);
}



////! Sets if the button should use the skin to draw its border
//void CGUIAudioPlayer::setDrawBorder(bool border)
//{
//	DrawBorder = border;
//}
//
////! Sets if the alpha channel should be used for drawing images on the button (default is false)
//void CGUIAudioPlayer::setUseAlphaChannel(bool useAlphaChannel)
//{
//	UseAlphaChannel = useAlphaChannel;
//}
//
////! Returns if the alpha channel should be used for drawing images on the button
//bool CGUIAudioPlayer::isAlphaChannelUsed() const
//{
//	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
//	return UseAlphaChannel;
//}
//
//bool CGUIAudioPlayer::isDrawingBorder() const
//{
//	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
//	return DrawBorder;
//}







//io::path CGUIAudioPlayer::createPreviewImageFromWaveforms( const core::dimension2du& img_size, bool bVertical ) const
//{
//	// #ifdef DEBUG
//	printf( "CGUIAudioPlayer::createPreviewImageFromWaveforms()\n" );
//	// #endif // DEBUG
//
//
//	io::path unique_name = _IRR_TEXT("");
//
//	/// create 1 image for all waveforms
//
//	video::IImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
//	if (!img)
//	{
//		return unique_name;
//	}
//
//	const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
//
//	#ifdef _DEBUG
//		printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
//	#endif // _DEBUG
//
//	unique_name = _IRR_TEXT("wav_");
//	if (Timer)
//	{
//		unique_name += currentDateString( Timer );
//		unique_name += _IRR_TEXT("_");
//		unique_name += Timer->getRealTime();
//		unique_name += _IRR_TEXT("_");
//	}
//
//	unique_name += img_size.Width;
//	unique_name += _IRR_TEXT("x");
//	unique_name += img_size.Height;
//	unique_name += _IRR_TEXT("x");
//	unique_name += img_bpp;
//	unique_name += _IRR_TEXT("_cc");
//	unique_name += SoundBuffer.getChannelCount();
//	unique_name += _IRR_TEXT(".png");
//
//	//! fill image pixels to fully transparent ( alpha == 0 )
//	img->fill( video::SColor(200,0,0,0) );
//
//	//! make place for a change in drawing positions
//
//	const u32 dy = core::round32( (f32)img_size.Height / (f32)SoundBuffer.getChannelCount() );
//
//	const core::dimension2du small_size( img_size.Width, dy );
//
//	core::position2di pos_xy( 0, 0);
//
//	/// loop channels
//	for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
//	{
//		sfx::drawAudioWaveformFast( img, core::recti( pos_xy, small_size ), WAV_Gradient, &SoundBuffer, channel_index, false );
//
//		pos_xy.Y += dy;
//	}
//
//	video::IVideoDriver* driver = Environment->getVideoDriver();
//	if ( !(driver->writeImageToFile( img, unique_name )) )
//	{
//		printf( "ELL_ERROR - Could not save image: %s\n", core::stringc(unique_name).c_str() );
//	}
//
//	//! delete image
//	img->drop();
//	img = 0;
//
//	return unique_name;
//}
//
//
///// create & save image from FFT-PowerSpectrum
//
//io::path CGUIAudioPlayer::createPreviewImageFromPowerSpectrum( const core::dimension2du& img_size, bool bVertical ) const
//{
//	// #ifdef DEBUG
//	printf( "CGUIAudioPlayer::createPreviewImageFromPowerSpectrum()\n" );
//	// #endif // DEBUG
//
//	io::path unique_name = _IRR_TEXT("");
//
//	/// create 1 image for all powerspectren
//
//	video::IImage* img = new video::CImage( video::ECF_A8R8G8B8, img_size );
//	if (!img)
//	{
//		return unique_name;
//	}
//
//	const u32 img_bpp = video::IImage::getBitsPerPixelFromFormat( img->getColorFormat() );
//
//	#ifdef _DEBUG
//		printf("Created image(%d,%d,%d);\n", img_size.Width, img_size.Height, img_bpp );
//	#endif // _DEBUG
//
//	unique_name = _IRR_TEXT("fft_");
//	if (Timer)
//	{
//		unique_name += currentDateString( Timer );
//		unique_name += _IRR_TEXT("_");
//		unique_name += Timer->getRealTime();
//		unique_name += _IRR_TEXT("_");
//	}
//
//	unique_name += img_size.Width;
//	unique_name += _IRR_TEXT("x");
//	unique_name += img_size.Height;
//	unique_name += _IRR_TEXT("x");
//	unique_name += img_bpp;
//	unique_name += _IRR_TEXT("_cc");
//	unique_name += SoundBuffer.getChannelCount();
//	unique_name += _IRR_TEXT(".png");
//
//	//! fill image pixels to fully transparent ( alpha == 0 )
//	img->fill( video::SColor(200,0,0,0) );
//
//	//! make place for a change in drawing positions
//
//	// const u32 dy = core::round32( (f32)img_size.Height / (f32)my_channel_count );
//
//	// const core::dimension2du small_size( img_size.Width, dy );
//
//	// core::position2di pos_xy( 0, 0);
//
//	/// loop channels
////		for ( u32 channel_index = 0; channel_index < SoundBuffer.getChannelCount(); channel_index++)
////		{
//
//	/// draw spectrum
//	sfx::drawAudioSpectrum( img, core::recti(), FFT_Gradient, &SoundBuffer, 0);
//		// sfx::drawAudioSpectrum( img, core::recti( pos_xy, small_size ), &my_soundbuffer, 0);
//
////			pos_xy.Y += dy;
////		}
//
//	video::IVideoDriver* driver = Environment->getVideoDriver();
//	if ( !(driver->writeImageToFile( img, unique_name )) )
//	{
//		printf( "ELL_ERROR - Could not save image: %s\n", core::stringc(unique_name).c_str() );
//	}
//
//	//! delete image
//	img->drop();
//	img = 0;
//
//	return unique_name;
//}
//



////! Writes attributes of the element.
//void CGUIAudioPlayer::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
//{
//	IGUIElement::serializeAttributes(out,options);
//
//	//	out->addBool	("PushButton",		IsPushButton );
//	//	if (IsPushButton)
//	//		out->addBool("Pressed",		Pressed);
//	//
//	//	out->addTexture ("Image",		Image);
//	//	out->addRect	("ImageRect",		ImageRect);
//	//	out->addTexture	("PressedImage",	PressedImage);
//	//	out->addRect	("PressedImageRect",	PressedImageRect);
//	//	out->addBool	("UseAlphaChannel",	isAlphaChannelUsed());
//	//	out->addBool	("Border",		isDrawingBorder());
//	//	out->addBool	("ScaleImage",		isScalingImage());
//	//  out->addString  ("OverrideFont",	OverrideFont);
//}
//
////! Reads attributes of the element
//void CGUIAudioPlayer::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
//{
//	IGUIElement::deserializeAttributes(in,options);
//
//	//	IsPushButton	= in->getAttributeAsBool("PushButton");
//	//	Pressed		= IsPushButton ? in->getAttributeAsBool("Pressed") : false;
//	//
//	//	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
//	//	if (rec.isValid())
//	//		setImage( in->getAttributeAsTexture("Image"), rec);
//	//	else
//	//		setImage( in->getAttributeAsTexture("Image") );
//	//
//	//	rec = in->getAttributeAsRect("PressedImageRect");
//	//	if (rec.isValid())
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage"), rec);
//	//	else
//	//		setPressedImage( in->getAttributeAsTexture("PressedImage") );
//	//
//	//	setDrawBorder(in->getAttributeAsBool("Border"));
//	//	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));
//	//	setScaleImage(in->getAttributeAsBool("ScaleImage"));
//	//  setOverrideFont(in->getAttributeAsString("OverrideFont"));
//
//	updateAbsolutePosition();
//}


} // end namespace gui
} // end namespace irr

// #endif // _IRR_COMPILE_WITH_GUI_


