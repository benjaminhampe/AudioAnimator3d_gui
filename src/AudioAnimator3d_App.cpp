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
, WindowTitle(MY_TITLE)
, WindowPosition( 10, 100)
, WindowDimension( 0, 0)
, WindowPadding( 50, 50)
, DesktopSize(0,0)
, ScreenSize(0,0)
//, ScreenRect(0,0,0,0)
, Wallpaper(0)
, SkyDomeTexture(0)
, SkyDome(0)
, FileName( MY_AUDIO_FILE )
, myFFTSize(4*8*128)
, myFFT(0)
, MatrixDimension( 4*64, 2*64 )
, MatrixData(0)
, DataDisplayBuffer(0)
, MatrixSize(1024,256,2*1024)
, MatrixPrimType( scene::EPT_TRIANGLES )
, MatrixNode0(0)
//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,100,100), video::SColorf(1,1,1,1), 400, -1);
//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(0,0,0), video::SColorf(1,1,0,1), 200, -1);
//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,0,0), video::SColorf(0,0,1,1), 200, -1);
, audioPlayerWindow(0)
, audioPlayer(0)
//, WaterNode(0)
, myTime(0)
, myRenderTime(0)
//	, myTimeLastWindowTitleUpate(0)
//	, myTimeWaitWindowTitleUpate(500)
, Font(0)
//, DefaultFont(0)
//, FontCaption(0)
//, FontBig(0)
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
	if (MatrixData)
		MatrixData->drop();
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

	ScreenSize = myDriver->getScreenSize();

	mySceneMgr = myDevice->getSceneManager();


	myGUI = myDevice->getGUIEnvironment();

	// ScreenRect = myGUI->getRootGUIElement()->getAbsolutePosition();

	Font = myGUI->getFont( MY_BIG_FONT_FILE );

	mySkin = myGUI->createSkin( gui::EGST_BURNING_SKIN );

	//  if (mySkin && BigFont)
	//	{
	//		mySkin->setFont( BigFont );
	//	}

	/// Wallpaper Texture

	myDriver->setTextureCreationFlag( video::ETCF_ALLOW_NON_POWER_2, true );

	myDriver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, false );

	Wallpaper = myDriver->getTexture( MY_LOAD_SCREEN_IMAGE );

	myDriver->setTextureCreationFlag( video::ETCF_CREATE_MIP_MAPS, true );

	/// render "Loading..." screen

	//drawLoadingScreen( myDriver, Wallpaper, L"loading...", BigFont, 0xffffffff);

	myDriver->beginScene( true, true, video::SColor(255,0,0,0) );

	video::drawLoadingScreen( myDriver, Wallpaper, L"Loading", Font, 0xffffffff );

	myDriver->endScene();

	myDriver->removeTexture( Wallpaper );

	/// create AudioPlayer Window

	audioPlayerWindow = myGUI->addWindow(

		core::recti( 	WindowPadding.Width,

						3*ScreenSize.Height/4,

						ScreenSize.Width-WindowPadding.Width,

						ScreenSize.Height-WindowPadding.Height),

		false, L"GUI AudioPlayer (SFML API)", myGUI->getRootGUIElement(), -1 );

	/// create AudioPlayer inside Window

	audioPlayer = new gui::CGUIAudioPlayer(

		myFFTSize, myTimer,

		myGUI, audioPlayerWindow, -1, audioPlayerWindow->getClientRect() );


	audioPlayer->loadFile( FileName );

	// audioPlayer->play();

	/// SampleBuffer ( SFML )

	printf( "get SoundBuffer\n" );

	sfmlSoundBuffer = const_cast<sf::SoundBuffer&>(audioPlayer->getSoundBuffer());


	/// FastFourierTransform ( fftw3 )

	printf( "get FourierTransform\n" );

	myFFT = audioPlayer->getFourierTransform();

	printf( "get FourierTransform - OK\n" );

	myFFTSize = myFFT->getFFTSize();

	myFFTInput.reallocate( myFFTSize );

	myFFTInput.set_used( myFFTSize );

	myFFTOutput.reallocate( MatrixDimension.Width );

	myFFTOutput.set_used( MatrixDimension.Width );

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
		myCamera->setPosition( core::vector3df(0.5f*MatrixSize.X,2.0f*MatrixSize.Y,-0.25f*MatrixSize.X) );
		// myCamera->setRotation( core::vector3df(0,528,0) );
		myCamera->setTarget( core::vector3df(0.5f*MatrixSize.X,0.0f,0.25f*MatrixSize.Z) );
		myCamera->setNearValue( 0.1f );
		myCamera->setFarValue( 10000.f );
		myCamera->setFOV( core::PI/2.1f );
	}

	printf( "create SkyDome\n" );

	/// skydome texture
	SkyDomeTexture = myDriver->getTexture( MY_SKYDOME_TEXTURE );

	/// skydome SceneNode
	SkyDome = mySceneMgr->addSkyDomeSceneNode( SkyDomeTexture, 32,16,
		0.99f, 2.0f, 1000.0f, mySceneMgr->getRootSceneNode(), -1);

	if (SkyDome)
	{
		SkyDome->setRotation( core::vector3df(0,180,0) );
	}

	/// lights
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,100,100), video::SColorf(1,1,1,1), 400, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(0,0,0), video::SColorf(1,1,0,1), 200, -1);
	//	mySceneMgr->addLightSceneNode( mySceneMgr->getRootSceneNode(), core::vector3df(100,0,0), video::SColorf(0,0,1,1), 200, -1);

	/// create DisplayBuffer

	printf( "create FrontMeshBuffer\n" );

	DataDisplayBuffer = new scene::SMeshBuffer();
	if (DataDisplayBuffer)
	{
		DataDisplayBuffer->Vertices.reallocate( 4*(myFFTOutput.size()-1) );
		DataDisplayBuffer->Indices.reallocate( 6*(myFFTOutput.size()-1) );
		DataDisplayBuffer->Vertices.set_used( 0 );
		DataDisplayBuffer->Indices.set_used( 0 );
		DataDisplayBuffer->Material.MaterialType = video::EMT_SOLID;
		DataDisplayBuffer->Material.Lighting = false;
		DataDisplayBuffer->Material.Wireframe = false;
		DataDisplayBuffer->Material.FogEnable = false;
	}

	/// Matrix 2D Array

	printf( "create 2D Matrix\n" );

	MatrixData = new core::CMatrix( MatrixDimension.Height, MatrixDimension.Width );
	if (MatrixData)
	{
		MatrixData->fill( 0.0f );
	}

	/// Matrix 3D SceneNode
	video::IColorGradient* colorGradient = audioPlayer->getFFTColorGradient();

	printf( "create 3D Matrix SceneNode\n" );

	MatrixNode0 = new scene::CMatrixSceneNode(
		MatrixPrimType,	MatrixSize, colorGradient, MatrixData,
		mySceneMgr, mySceneMgr->getRootSceneNode(), -1, core::vector3df(0,0,0) );

	if (!MatrixNode0)
	{
		printf("Could not create CMatrixSceneNode\n");
	}
	else
	{
		MatrixNode0->setMaterialFlag( video::EMF_LIGHTING, false );
		// MatrixNode0->setDebugDataVisible( scene::EDS_BBOX );
	}


	/// elvman
	//WaterNode = new CRealisticWaterSceneNode( mySceneMgr, 2000,2000 );

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

	audioPlayer->play();

	printf( "AudioAnimator3d::init( ) - OK\n" );

	return true;
}

gui::IGUIWindow* AudioAnimator3d::createHelpWindow()
{
	return 0;
}

gui::IGUIWindow* AudioAnimator3d::createParamWindow()
{
	if (!myDevice)
		return 0;

	gui::IGUIEnvironment* env = myDevice->getGUIEnvironment();

	if (!env)
		return 0; // error!

	gui::IGUIWindow* window = env->addWindow(
		core::recti( 0, 100, 200, 400),
		false, L"Parameter Control Window", env->getRootGUIElement(), -1 );

	const core::recti r_client = window->getClientRect();

	const s32 w = r_client.getWidth();

	const s32 h = r_client.getHeight();

	const s32 b = 5; // border / padding

	const s32 l = 30; // line-height

	s32 x = b;

	s32 y = b;

	s32 dx = w/2 - 2*b;

	s32 dy = l;

	env->addStaticText( L"FFT-Size",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Dimension X",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Dimension Y",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Size X",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Size Y",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Size Z",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

	y += dy + b;

	env->addStaticText( L"Matrix Render Type",
		core::recti( x, y, x + dx, y + dy), true, false, window, -1, true );

//	InputFFTSize = env->addComboBox(
//		core::recti( 0, 0, w, )
//
//, InputMatrixDimX(0)
//, InputMatrixDimY(0)
//, InputMatrixSizeX(0)
//, InputMatrixSizeY(0)
//, InputMatrixSizeZ(0)
//, InputMatrixPrimType(0)
	return window;
}

bool
AudioAnimator3d::run()
{
	if (!myDevice)
		return false;

	myRenderTime = myTimer->getRealTime() - myTime;

	myTime = myTimer->getRealTime();

	u32 ChannelCount = audioPlayer->getChannelCount();

	/// get current PlayPosition
	u32 PlayPosition = audioPlayer->getPlayPosition();

	/// decrease time to be sync with audio
	if (PlayPosition > myRenderTime * ChannelCount )
		PlayPosition -= myRenderTime * ChannelCount;

	/// myFFT

	sfx::fillSampleBuffer<s16>( myFFTInput, &audioPlayer->getSoundBuffer(), 0, PlayPosition, myFFTInput.size() );
	myFFT->setInputData<s16>( myFFTInput );
	myFFT->fft();
	myFFT->getPowerSpectrumAsDecibelsThreshold<f32>( myFFTOutput, 45.0f );
	// myFFT.getPowerSpectrum<f32>( myFFTOutput );
	// myFFT.getScaledPowerSpectrum<f32>( myFFTOutput, MatrixSize.Y );

	/// Fill Matrix with new values from myFFT
	MatrixData->shiftRow();
	// MatrixData.shiftRows( 1 );

	const u32 i_max = core::min_<u32>( myFFTOutput.size(), MatrixData->getCols() );

	for (u32 i=0; i<i_max; i++)
	{
		MatrixData->setElement( 0, i, myFFTOutput[i] );
	}

	/// update Matrix SceneNode Mesh
	MatrixNode0->createMesh();

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
		DataDisplayBuffer,
		core::dimension2df(MatrixSize.X, MatrixSize.Y),
		myFFTOutput,
		audioPlayer->getFFTColorGradient(),
		core::vector3df(0,0,0) );

	myDriver->setMaterial( DataDisplayBuffer->getMaterial() );

	video::drawMeshBufferEx( myDriver, DataDisplayBuffer, scene::EPT_TRIANGLES );

	/// draw CoordSystems
	video::drawXMeter( core::vector3df( 0.0f,0.0f,-1.0f), 0 , core::round32(MatrixSize.X) );
	video::drawZMeter( core::vector3df( -1.0f,0.0f,0.0f), 0 , core::round32(MatrixSize.Z) );
	video::drawYMeter( core::vector3df( 0.0f,0.0f,-1.0f), -1, core::round32(MatrixSize.Y) );
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

} // end namespace irr

