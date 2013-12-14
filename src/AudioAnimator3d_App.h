#ifndef AUDIOANIMATOR3D_APP_H
#define AUDIOANIMATOR3D_APP_H

// Irrlicht Engine
#include <irrlicht.h>

// hampe
#include <libHampe.h>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <fftw3/fftw3.h>
#include "CGUIAudioPlayer.h"

// elvman
//#include <CRealisticWater.h>

/// Definitions

#define MY_TITLE "CGUIDataPlot (c) 2013 by BenjaminHampe@gmx.de"

#define MY_AUDIO_FILE 				"../../media/music/razor.ogg"

#define MY_BIG_FONT_FILE 			"../../media/fonts/courier22.png"

#define MY_SKYDOME_TEXTURE 			"../../media/skydome/skydome.jpg"

#define MY_LOAD_SCREEN_IMAGE 		"../../media/background/load_2.jpg"

namespace irr
{

/// @class interface IRunable
class IRunable
{
public:
	virtual bool run() = 0;
};

/// @class interface ICustomRenderer
class ICustomRenderer
{
public:
	virtual void render() = 0;
};

/// @class AudioAnimator3d Application class
class AudioAnimator3d : public IEventReceiver, public IRunable, public ICustomRenderer
{
public:
	/// @brief Constructor, set all default values and pointers to zero
	AudioAnimator3d( IrrlichtDevice* device );

	/// @brief Destructor, deletes the matrix data if exist
	virtual ~AudioAnimator3d( );

	/// @brief Start the application, loads ressources
	bool init( );

	/// @brief implement interface IEventReceiver
	virtual bool OnEvent (const SEvent &event);

	/// @brief implement interface IRunable
	virtual bool run();

	/// @brief implement interface ICustomRenderer
	/// here is everything rendered that is not registered within SceneManager
	/// i.e. native OpenGL Code, pure MeshBuffers, etc...
	virtual void render();

protected:
	gui::IGUIWindow* createHelpWindow();

	gui::IGUIWindow* createParamWindow();

private:

	///! irrlicht pointer
	IrrlichtDevice* 			myDevice;
	ILogger* 					myLogger;
	ITimer* 					myTimer;
	video::IVideoDriver* 	myDriver;
	scene::ISceneManager* 	mySceneMgr;
	scene::ICameraSceneNode* myCamera;
	gui::IGUIEnvironment* 	myGUI;
	gui::IGUISkin* 			mySkin;
	u32 						myTime; 			// time measurement
	u32 						myRenderTime;
//	u32 						myTimeLastWindowTitleUpate;
//	u32 						myTimeWaitWindowTitleUpate;
	gui::IGUIFont* 			myFont; 				// current used font
	//gui::IGUIFont* 			myDefaultFont;		// GUI Fonts
	//gui::IGUIFont* 			myFontCaption;		// GUI Fonts
	//gui::IGUIFont* 			myFontBig;			// GUI Fonts
	core::stringw 			myWindowTitle;
	core::position2di 			myWindowPosition;
	core::dimension2du 		myWindowDimension;
	core::dimension2di 		myWindowPadding;
	core::dimension2du 		myDesktopSize;
	core::dimension2du 		myScreenSize;
	//core::recti 				myScreenRect;
	video::ITexture* 			myWallpaper; 			// for "loading..." screen
	video::ITexture* 			mySkyDomeTexture;		// SkyDome Texture
	scene::ISceneNode* 		mySkyDome; 			// SkyDome SceneNode

	/// Light SceneNodes
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,100,100), video::SColorf(1,1,1,1), 400, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(0,0,0), video::SColorf(1,1,0,1), 200, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,0,0), video::SColorf(0,0,1,1), 200, -1);

	/// SFML Audio 2.0 stuff
	core::stringc				myFileName; 			// current used FileName
	sf::SoundBuffer* 			mySoundBuffer; 	// SFML Audio 2.0 API stuff

	/// FastFourierTransform containers and calculator
	u32 						myFFTSize;
	core::array<s16> 			myFFTInput;
	core::array<f32> 			myFFTOutput;
	sfx::FourierTransformRtoC* myFFT;

	/// 2D Matrix data
	u32 						myMatrixRows;
	u32 						myMatrixCols;
	core::CMatrix* 			myMatrixData;

	/// used to display the current FFT DataSet
	scene::SMeshBuffer* 		myDataDisplayBuffer;

	/// 3D Matrix SceneNode
	core::vector3df 			myMatrixSize;
	scene::E_PRIMITIVE_TYPE 	myMatrixPrimType;
	scene::CMatrixSceneNode* 	myMatrixNode;

	/// GUI AudioPlayer
	gui::IGUIWindow* 			myAudioPlayerWindow;
	gui::CGUIAudioPlayer* 		myAudioPlayer;

	/// common GUI Elements
	gui::IGUIButton* 			ToggleButtonHelpWindow;
	gui::IGUIButton* 			ToggleButtonParamWindow;
	gui::IGUIButton* 			ToggleButtonWireframe;
	gui::IGUIButton* 			ToggleButtonLighting;
	gui::IGUIWindow* 			HelpWindow;
	gui::IGUIWindow* 			ParamWindow;
	gui::IGUIComboBox*			InputFFTSize;
	gui::IGUIEditBox*			InputMatrixDimX;
	gui::IGUIEditBox*			InputMatrixDimY;
	gui::IGUIEditBox*			InputMatrixSizeX;
	gui::IGUIEditBox*			InputMatrixSizeY;
	gui::IGUIEditBox*			InputMatrixSizeZ;
	gui::IGUIComboBox*			InputMatrixPrimType;
};

} // end namespace irr


#endif // AUDIOANIMATOR3D_APP_H
