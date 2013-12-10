
/*! \mainpage AudioAnimator3d help & irrAudioFFT API 1.0 docs
 *
 * <div align="center"><img src="irrAudio_logo.png" ></div>
 *
 * \section Content-Listing
 * Sections:<br/>
 * @ref start <br/>
 * @ref features <br/>
 * @ref links <br/>
 * @ref tipsandtricks <br/>
 * @ref example <br/>
 * <br/>
 /**
 * \section start This program creates animated 3d powerspectrum from audio-files
 *
 * idea found at http://www.youtube.com/watch?v=gbuZVcw3ZiM
 * no copyright infringement intended i just like working with audio-data
 *
 * to compile this program u need fftw3, SFML2.0 Audio, freetype, OpenGL, Irrlicht
 *
 * tested so far only on Windows7 32-bit professional
 * on Core2Duo E6600 2x 2,4 GHz, 2GiB RAM, Ati HD5450 512MiByte VRAM
 * i got 30 FPS so far at FFT-Size 8192 and 8192/16 = 512 Tiles per MatrixRow ( == 2 Triangles )

 *
 * Welcome to the irrAudio API documentation. This page should give you a short overview
 * over irrAudio, the high level audio library wrapper.
 * In this documentation files you'll find any information you'll need to develop applications with
 * irrKlang using C++. If you are looking for a tutorial on how to start, you'll
 * find some on the homepage of irrAudio at
 * <A HREF="/irrAudio" >http://irrAudio</A>
 * or inside the SDK in the directory \examples.
 *
 * The irrAudio library is intended to be an free easy-to-use 3d and 2d sound engine, so
 * this documentation is an important part of it. If you have any questions or
 * suggestions, please take a look into the ambiera.com forum or just send a mail.
 * <br/>
 *
 *
 * \section features load audio-file using SFML 2.0 Audio Library
 * calculate its PowerSpectrum (0...160 dB) using fftw3 FastFourierTransform
 * create image from wavform and power-spectrum using CLinearColorGradient
 * create 2D and 3D animated meshes from power-spectrum
 * each loop:
 * get N samples at myTime - lastRenderTime -> can lead to fluctuations when paused
 * do some windowing, filtering stuff
 * calculate power-spectrum with N samples
 * use only first N/16 frequencies to create meshes ( much faster )
 * shift matrix one row
 * set matrix data at first row( y = row-index = 0 ) from power-spectrum
 * create front mesh from power-spectrum
 * create matrix mesh
 * render power-spectrum as background-texture
 * render matrix-mesh
 * render front-mesh
 * render x,y,z meter ( currently pure OpenGL )
 * render GUI
 * set WindowCaption with new information
 * loop until ESCAPE-Key
 *
 *	@todo Speed improvements! ( pls give advices )
 * Use assembler to speed up sin/cos ( both can be computed with one instruction )
 * Use Goertzel's algorithm to speed up FFT at specific frequencies ( tones )
 * WindowFunctions ( Cos, Gauss, Hamming, Hann, Blackman )
 * impulse-response functions -> each create different responses in freq-domain
 * 2D Plot Window ( linear and logarithmic scales ) for current data
 * 3D Plot Window ( insert CMatrixSceneNode in a IGUIElement )
 * Equalizer window with user interactive curve-manipulation ( 1-8 point polynom )
 * do chromatic-tone-pitch estimation
 * build piano-tuner using microphone
 *
 * \section links further Web-Links
 * \section tipsandtricks
 *
 * irrAudio is collection of tools to load, draw waveforms, fft-power-sprectrum to images
 * uses cross platform sound engine SFML-Audio API for most stuff.
 * It has a very simply c-style set of tool-functions that operate on images, strings, etc...
 * irrAudio can load/play several file formats such as
 * <ul>
 *               <li>RIFF WAVE (*.wav)</li>
 *               <li>Ogg Vorbis (*.ogg)</li>
 *               <li>MPEG-1 Audio Layer 3 (*.mp3)</li>
 *               <li>Free Lossless Audio Codec (*.flac)</li>
 * </ul>


 * \section example
 *
 * <ul>
 *               <li>If you can choose which audio file format is the primary one for your application,
 *					 use .OGG files, instead of for example .MP3 files. irrKlang uses a lot less memory
 *                   and CPU power when playing .OGGs.</li>
 *               <li>To keep your application simple, each time you play a sound, you can use for example
 *                   play2D("filename.mp3") and let irrKlang handle the rest. There is no need to implement
 *                   a preloading/caching/file management system for the audio playback. irrKlang will handle
 *                   all this by itself and will never load a file twice. </li>
 *               <li>irrKlang is crashing in your application? This should not happen, irrKlang is pretty stable,
 *                   and in most cases, this is a problem in your code: In a lot of cases the reason is simply
 *                   a wrong call to irrklang::IReferenceCounted::drop(). Be sure you are doing it correctly. (If you are unsure,
 *                   tempoarily remove all calls to irrklang::IReferenceCounted::drop() and see if this helps.)</li>
 * </ul>
 *
 * <br/>

 * \code

 * \endcode
 */
