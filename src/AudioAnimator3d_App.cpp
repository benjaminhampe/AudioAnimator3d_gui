#include "AudioAnimator3d_App.h"


namespace irr
{

// class AudioAnimator3d
//
// : public IEventReceiver, public IRunable, public ICustomRenderer

AudioAnimator3d::AudioAnimator3d( IrrlichtDevice* device )
: myDevice(device)
, myLogger(0)
, myTimer(0)
, myDriver(0)
, mySceneMgr(0)
, myCamera(0)
, myGUI(0)
, mySkin(0)
, myTime(0)
, myRenderTime(0)
//, myTimeLastWindowTitleUpate(0)
//, myTimeWaitWindowTitleUpate(500)
, myFont(0)
//, myDefaultFont(0)
//, myFontCaption(0)
//, myFontBig(0)
, myWindowTitle(MY_TITLE)
, myWindowPosition( 10, 100)
, myWindowDimension( 0, 0)
, myWindowPadding( 50, 50)
, myDesktopSize(0,0)
, myScreenSize(0,0)
, myWallpaper(0)
, mySkyDomeTexture(0)
, mySkyDome(0)
, myFileName( MY_AUDIO_FILE )
, myFFTSize(4*8*128)
, myFFT(0)
, myMatrixRows( 2*64 )
, myMatrixCols( 4*64 )
, myMatrixData(0)
, myDataDisplayBuffer(0)
, myMatrixSize(1024,256,2*1024)
, myMatrixPrimType( scene::EPT_TRIANGLES )
, myMatrixNode(0)
, myAudioPlayerWindow(0)
, myAudioPlayer(0)
//, myWaterNode(0)


, ToggleButtonHelpWindow(0)
, ToggleButtonParamWindow(0)
, ToggleButtonWireframe(0)
, ToggleButtonLighting(0)
, HelpWindow(0)
, ParamWindow(0)
, InputFFTSize(0)
, InputMatrixDimX(0)
, InputMatrixDimY(0)
, InputMatrixSizeX(0)
, InputMatrixSizeY(0)
, InputMatrixSizeZ(0)
, InputMatrixPrimType(0)
{
	if ( !init() )
	{
		fprintf( stdout, "Could not start AudioAnimator3d Application / StateMachine\n");
		fprintf( stderr, "Could not start AudioAnimator3d Application / StateMachine\n");
		exit(0);
	}

}

AudioAnimator3d::~AudioAnimator3d( )
{
	if (myMatrixData)
		myMatrixData->drop();
}

bool
AudioAnimator3d::init( )
{
	if (!myDevice)
		return false;

	myDevice->setEventReceiver(this);

	/// Logger

	myLogger = myDevice->getLogger();

	myLogger->log( "AudioAnimator3d attached to IrrlichtDevice" );


	myTimer = myDevice->getTimer();

	myTime = myTimer->getRealTime();

	srand( myTime );


	myDriver = myDevice->getVideoDriver();

	myScreenSize = myDriver->getScreenSize();

	mySceneMgr = myDevice->getSceneManager();


	myGUI = myDevice->getGUIEnvironment();

	// myScreenRect = myGUI->getRootGUIElement()->getAbsolutePosition();

	myFont = myGUI->getFont( MY_BIG_FONT_FILE );

	mySkin = myGUI->createSkin( gui::EGST_BURNING_SKIN );

	//  if (mySkin && myBigFont)
	//	{
	//		mySkin->setFont( myBigFont );
	//	}

	/// Wallpaper Texture

	myDriver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, true );

	myDriver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );

	myWallpaper = myDriver->getTexture( MY_LOAD_SCREEN_IMAGE );

	myDriver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, true );

	/// render "Loading..." screen

	//drawLoadingScreen( myDriver, Wallpaper, L"loading...", BigFont, 0xffffffff);

	myDriver->beginScene( true, true, video::SColor(255,0,0,0) );

	video::drawLoadingScreen( myDriver, myWallpaper, L"Loading", myFont, 0xffffffff );

	myDriver->endScene();

	myDriver->removeTexture( myWallpaper );

	/// create AudioPlayer Window

	myAudioPlayerWindow = myGUI->addWindow(

		core::recti( 	myWindowPadding.Width,

						3*myScreenSize.Height/4,

						myScreenSize.Width-myWindowPadding.Width,

						myScreenSize.Height-myWindowPadding.Height),

		false, L"GUI AudioPlayer (SFML API)", myGUI->getRootGUIElement(), -1 );

	/// create AudioPlayer inside Window

	myAudioPlayer = new gui::CGUIAudioPlayer(

		myFFTSize, myTimer,

		myGUI, myAudioPlayerWindow, -1, myAudioPlayerWindow->getClientRect() );


	myAudioPlayer->loadFile( myFileName );


	/// SampleBuffer ( SFML )

	printf( "get SoundBuffer\n" );

	mySoundBuffer = myAudioPlayer->getSoundBuffer();


	/// FastFourierTransform ( fftw3 )

	printf( "get FourierTransform\n" );

	myFFT = myAudioPlayer->getFourierTransform();

	printf( "get FourierTransform - OK\n" );

	myFFTSize = myFFT->getFFTSize();

	myFFTInput.reallocate( myFFTSize );

	myFFTInput.set_used( myFFTSize );

	myFFTOutput.reallocate( myMatrixCols );

	myFFTOutput.set_used( myMatrixCols );

	printf( "fft-size = %d\n", myFFTSize );

	printf( "%s\n", myFFT->getPlanInfo().c_str());

	//	getFFTSpectrumMinMaxValue(

	//		audioPlayer->getFourierTransform(),
	//		audioPlayer->getSoundBuffer(),
	//		10000, /* u32 TimeStamps */
	//		0 /* s32 channel_index */ );

	/// Camera
	printf( "create Camera\n" );

	myCamera = mySceneMgr->getActiveCamera();
	if (!myCamera)
	{
		myCamera = Camera::createFPS( mySceneMgr, 0.1f, 100.f );
	}

	if (myCamera)
	{
		myCamera->setPosition( core::vector3df(0.5f*myMatrixSize.X,2.0f*myMatrixSize.Y,-0.25f*myMatrixSize.X) );
		// myCamera->setRotation( core::vector3df(0,528,0) );
		myCamera->setTarget( core::vector3df(0.5f*myMatrixSize.X,0.0f,0.25f*myMatrixSize.Z) );
		myCamera->setNearValue( 0.1f );
		myCamera->setFarValue( 10000.f );
		myCamera->setFOV( core::PI/2.1f );
	}

	printf( "create SkyDome\n" );

	/// skydome texture
	mySkyDomeTexture = myDriver->getTexture( MY_SKYDOME_TEXTURE );

	/// skydome SceneNode
	mySkyDome = mySceneMgr->addSkyDomeSceneNode( mySkyDomeTexture, 32,16,
		0.99f, 2.0f, 1000.0f, mySceneMgr->getRootSceneNode(), -1);

	if (mySkyDome)
	{
		mySkyDome->setRotation( core::vector3df(0,180,0) );
	}

	/// lights
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,100,100), video::SColorf(1,1,1,1), 400, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(0,0,0), video::SColorf(1,1,0,1), 200, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,0,0), video::SColorf(0,0,1,1), 200, -1);

	/// create DisplayBuffer

	printf( "create FrontMeshBuffer\n" );

	myDataDisplayBuffer = new scene::SMeshBuffer();
	if (myDataDisplayBuffer)
	{
		myDataDisplayBuffer->Vertices.reallocate( 4*(myFFTOutput.size()-1) );
		myDataDisplayBuffer->Indices.reallocate( 6*(myFFTOutput.size()-1) );
		myDataDisplayBuffer->Vertices.set_used( 0 );
		myDataDisplayBuffer->Indices.set_used( 0 );
		myDataDisplayBuffer->Material.MaterialType = video::EMT_SOLID;
		myDataDisplayBuffer->Material.Lighting = false;
		myDataDisplayBuffer->Material.Wireframe = false;
		myDataDisplayBuffer->Material.FogEnable = false;
	}

	/// Matrix 2D Array

	printf( "create 2D Matrix\n" );

	myMatrixData = new core::CMatrix( myMatrixRows, myMatrixCols );
	if (myMatrixData)
	{
		myMatrixData->fill( 0.0f );
	}

	/// Matrix 3D SceneNode
	video::IColorGradient* colorGradient = myAudioPlayer->getFFTColorGradient();

	printf( "create 3D Matrix SceneNode\n" );

	myMatrixNode = new scene::CMatrixSceneNode(
		myMatrixPrimType,
		myMatrixSize,
		colorGradient,
		myMatrixData,
		mySceneMgr, mySceneMgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!myMatrixNode)
	{
		printf("Could not create CMatrixSceneNode\n");
	}
	else
	{
		myMatrixNode->setMaterialFlag( video::EMF_LIGHTING, false );
		// myMatrixNode->setDebugDataVisible( scene::EDS_BBOX );
	}


	/// elvman
	// myWaterNode = new CRealisticWaterSceneNode( mySceneMgr, 2000,2000 );

	//	core::array<s16> samples_for_goertzel;
	//
	//	samples_for_goertzel.reallocate(myFFTSize, true);
	//	samples_for_goertzel.set_used(myFFTSize);
	//
	//	myTime = myTimer->getRealTime();
	//	sfx::fillSampleBuffer(samples_for_goertzel, &sfmlSoundBuffer, 0, 0, samples_for_goertzel.size() );
	//	printf("sfx::fillSampleBuffer() = %d ms\n", myTimer->getRealTime() - myTime );
	//
	//	myTime = myTimer->getRealTime();
	//
	//	f64 result = goertzel<s16>( samples_for_goertzel, sfmlSoundBuffer.getSampleRate(), 440.0 ); // search tone A4
	//	printf("sfx::goertzel_fft() = %d ms\n", myTimer->getRealTime() - myTime );
	//
	//	f64 goertzel<s16>( samples_for_goertzel, sfmlSoundBuffer->getSampleRate(), 440.0 ) // A4
	//	f64 goertzel<s16>( samples_for_goertzel, sfmlSoundBuffer->getSampleRate(), 440.0 ) // A4
	//	f64 goertzel<s16>( samples_for_goertzel, sfmlSoundBuffer->getSampleRate(), 440.0 ) // A4



	//	/// create image from FFT-PowerSpectrum
	//	video::IImage* img_spectrum = audioPlayer->createImageFromSoundBufferPowerSpectrum( ScreenSize );
	//
	//	video::ITexture* tex_spectrum = audioPlayer->addTexture( img_spectrum );
	//
	//	if (img_spectrum)
	//		img_spectrum->drop();

	/// GUI-ImageViewer
	//gui::IGUIWindow* image_viewer_window = myGUI->addWindow( core::recti( 10,10,10+640,10+480 ), false, L"ImageViewer", myGUI->getRootGUIElement(), -1 );
	//gui::CGUIImageViewer* image_viewer_panel = new gui::CGUIImageViewer( myGUI, image_viewer_window, -1, image_viewer_window->getClientRect() );
	// image_viewer_panel->setImage( audioPlayer->getWallpaperTexture() );

//	s32 x = 0;
//	s32 y = 0;
//	s32 padd = 5;
//	s32 btn_w = 48;
//	s32 btn_h = 48;
//
//	ToggleButtonHelpWindow = myGUI->addButton(
//		core::recti( x+padd,y+padd, x+padd+btn_w,y+padd+btn_h),
//		myGUI->getRootGUIElement(), -1, L"Help", L"toggle visibility of help window");
//
//	y += btn_h + 2*padd;
//
//	ToggleButtonParamWindow = myGUI->addButton(
//		core::recti( x+padd,y+padd, x+padd+btn_w,y+padd+btn_h),
//		myGUI->getRootGUIElement(), -1, L"Parameters", L"toggle visibility of param window");
//
//	y += btn_h + 2*padd;
//
//	ToggleButtonWireframe = myGUI->addButton(
//		core::recti( x+padd,y+padd, x+padd+btn_w,y+padd+btn_h),
//		myGUI->getRootGUIElement(), -1, L"Wireframe", L"toggle wireframe");
//
//	y += btn_h + 2*padd;

//	HelpWindow = createHelpWindow();
//
//	ParamWindow = createParamWindow();

	printf( "play()\n" );

	myAudioPlayer->play();

	printf( "AudioAnimator3d::init( ) - OK\n" );

	return true;
}

bool
AudioAnimator3d::run()
{
	if (!myDevice)
		return false;

	myRenderTime = myTimer->getRealTime() - myTime;

	myTime = myTimer->getRealTime();

	u32 ChannelCount = myAudioPlayer->getChannelCount();

	/// get current PlayPosition
	u32 PlayPosition = myAudioPlayer->getPlayPosition();

	/// decrease time to be sync with audio
	if (PlayPosition > myRenderTime * ChannelCount )
		PlayPosition -= myRenderTime * ChannelCount;

	/// myFFT

	sfx::fillSampleBuffer<s16>( myFFTInput, myAudioPlayer->getSoundBuffer(), 0, PlayPosition, myFFTInput.size() );
	myFFT->setInputData<s16>( myFFTInput );
	myFFT->fft();
	myFFT->getPowerSpectrumAsDecibelsThreshold<f32>( myFFTOutput, 45.0f );
	// myFFT.getPowerSpectrum<f32>( myFFTOutput );
	// myFFT.getScaledPowerSpectrum<f32>( myFFTOutput, MatrixSize.Y );

	/// Fill Matrix with new values from myFFT
	myMatrixData->shiftRow();
	// MatrixData.shiftRows( 1 );

	const u32 i_max = core::min_<u32>( myFFTOutput.size(), myMatrixData->getCols() );

	for (u32 i=0; i<i_max; i++)
	{
		myMatrixData->setElement( 0, i, myFFTOutput[i] );
	}

	/// update Matrix SceneNode Mesh
	myMatrixNode->createMesh();

	return true;
}

void
AudioAnimator3d::render()
{

	/// draw FFT output as filled path between x-axis and output-data
	myDriver->setTransform( video::ETS_WORLD, core::IdentityMatrix );
	myDriver->setTransform( video::ETS_VIEW, myCamera->getViewMatrix() );
	myDriver->setTransform( video::ETS_PROJECTION, myCamera->getProjectionMatrix() );

	sfx::createFilledPath(
		myDataDisplayBuffer,
		core::dimension2df(myMatrixSize.X, myMatrixSize.Y),
		myFFTOutput,
		myAudioPlayer->getFFTColorGradient(),
		core::vector3df(0,0,0) );

	myDriver->setMaterial( myDataDisplayBuffer->getMaterial() );

	video::drawMeshBufferEx( myDriver, myDataDisplayBuffer, scene::EPT_TRIANGLES );

	/// draw CoordSystems
	video::drawXMeter( core::vector3df( 0.0f,0.0f,-1.0f), 0 , core::round32(myMatrixSize.X) );
	video::drawZMeter( core::vector3df( -1.0f,0.0f,0.0f), 0 , core::round32(myMatrixSize.Z) );
	video::drawYMeter( core::vector3df( 0.0f,0.0f,-1.0f), -1, core::round32(myMatrixSize.Y) );
}

bool
AudioAnimator3d::OnEvent (const SEvent &event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED)
		{
			gui::IGUIElement* caller = event.GUIEvent.Caller;
			if (caller)
			{
				/// toggle button - wireframe
				if (caller == ToggleButtonWireframe)
				{
					if (myDevice)
					{
						const core::list<scene::ISceneNode*>& children = myDevice->getSceneManager()->getRootSceneNode()->getChildren();
						core::list<scene::ISceneNode*>::ConstIterator it = children.begin();
						while ( it != children.end() )
						{
							scene::ISceneNode* node = *it;
							if (node)
							{
								u32 matCount = node->getMaterialCount();
								for (u32 i=0; i<matCount; i++)
								{
									video::SMaterial& matRef = node->getMaterial(i);
									matRef.Wireframe = !matRef.Wireframe;
								}
							}
							it++;
						}
					}
					return true;
				}

				/// light toggle button

				/// frontface-culling toggle button

				/// backface-culling toggle button

				/// show-help toggle button
			}
		}
	}

	if(event.EventType == EET_KEY_INPUT_EVENT)
	{
		/// escape key - exit program
		if(event.KeyInput.Key == KEY_ESCAPE)
		{
			exit(0); // we exit
		}

		/// print key - create screenshot image with unique name ( myTimer based )
		if ( (event.KeyInput.Key == KEY_PRINT || event.KeyInput.Key == KEY_SNAPSHOT) && !event.KeyInput.PressedDown)
		{
			if (myDevice)
			{
				ITimer* myTimer = myDevice->getTimer();
				video::IVideoDriver* myDriver = myDevice->getVideoDriver();
				if (myDriver && myTimer)
				{
					video::IImage* screenshot = myDriver->createScreenShot();
					if (screenshot)
					{
						/// create filename
						io::path name = "./screenshot_render_audio_spectrum_";
						name += currentDateTimeString( myTimer );
						name += ".png";

						/// save screenshot
						myDriver->writeImageToFile( screenshot, name);

						/// delete
						screenshot->drop();
					}
				}
			}
			return true;
		}

		/// space key - enable/disable myCamera eventreceiver
		if (event.KeyInput.Key == KEY_SPACE && !event.KeyInput.PressedDown)
		{
			if (myDevice)
			{
				scene::ISceneManager* mySceneMgr = myDevice->getSceneManager();
				scene::ICameraSceneNode* myCamera = mySceneMgr->getActiveCamera();
				if (myCamera)
				{
					myCamera->setInputReceiverEnabled( !myCamera->isInputReceiverEnabled() );
				}
			}
			return true;
		}
	}
	return false;
}

//
//gui::IGUIWindow* AudioAnimator3d::createHelpWindow()
//{
//	return 0;
//}
//
//gui::IGUIWindow* AudioAnimator3d::createParamWindow()
//{
//	if (!myDevice)
//		return 0;
//
//	gui::IGUIEnvironment* env = myDevice->getGUIEnvironment();
//
//	if (!env)
//		return 0; // error!
//
//	gui::IGUIWindow* window = env->addWindow(
//		core::recti( 0, 100, 200, 400),
//		false, L"Parameter Control Window", env->getRootGUIElement(), -1 );
//
//	const core::recti r_client = window->getClientRect();
//
//	const s32 w = r_client.getWidth();
//
//	const s32 h = r_client.getHeight();
//
//	const s32 b = 5; // border / padding
//
//	const s32 l = 30; // line-height
//
//	s32 x = b;
//
//	s32 y = b;
//
//	s32 dx = w/2 - 2*b;
//
//	s32 dy = l;
//
//	env->addStaticText( L"FFT-Size",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Dimension X",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Dimension Y",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Size X",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Size Y",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Size Z",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
//	y += dy + b;
//
//	env->addStaticText( L"Matrix Render Type",
//		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );
//
////	InputFFTSize = env->addComboBox(
////		core::recti( 0, 0, w, )
////
////, InputMatrixDimX(0)
////, InputMatrixDimY(0)
////, InputMatrixSizeX(0)
////, InputMatrixSizeY(0)
////, InputMatrixSizeZ(0)
////, InputMatrixPrimType(0)
//	return window;
//}

} // end namespace irr

