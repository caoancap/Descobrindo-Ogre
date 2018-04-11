#include "BaseApplication.h"

BaseApplication::BaseApplication(void)
	: mRoot(0)
	, mCamera(0)
	, mSceneMgr(0)
	, mWindow(0)
	, mResourcesCfg(StringUtil::BLANK)
	, mPluginsCfg(StringUtil::BLANK)
	, mTrayMgr(0)
	, mVistaMundo(0)
	, mDetailsPanel(0)
	, mCursorWasVisible(false)
	, mShutDown(false)
	, mInputManager(0)
	, mMouse(0)
	, mKeyboard(0)
	, mOverlaySystem(0)
{}

BaseApplication::~BaseApplication(void) {
	if(mTrayMgr)
		delete mTrayMgr;
	if(mVistaMundo)
		delete mVistaMundo;
	if(mOverlaySystem)
		delete mOverlaySystem;

	// Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

bool BaseApplication::configure(void) {
	// Show the configuration dialog and initialise the system.
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg.
	if(mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise.
		// Here we choose to let the system create a default rendering window by passing 'true'.
		mWindow = mRoot->initialise(true, "TutorialApplication Render Window");
		return true;
	}
	else return false;
}

void BaseApplication::chooseSceneManager(void) {
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC);
	// Initialize the OverlaySystem (changed for Ogre 1.9)
	mOverlaySystem = new OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
}

void BaseApplication::colocaJogador(void) {
	mVistaMundo = new VistaMundo(mSceneMgr);
}

void BaseApplication::createCamera(void) {
    mCamera = mSceneMgr->createCamera(Mundo::camera);
    mCamera->setPosition(Ogre::Vector3(0,0,80));
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);
}

void BaseApplication::createFrameListener(void) {
	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	ParamList pl;
	size_t windowHnd = 0;
	ostringstream windowHndStr;
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(make_pair(string("WINDOW"), windowHndStr.str()));
	mInputManager = InputManager::createInputSystem(pl);
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject(OISKeyboard, true));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject(OISMouse, true));
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
	
	// Set initial mouse clipping size
	windowResized(mWindow);
	
	// Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mInputContext.mKeyboard = mKeyboard;
	mInputContext.mMouse = mMouse;
	mTrayMgr = new SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
	mTrayMgr->showFrameStats(TL_BOTTOMLEFT);
	mTrayMgr->showLogo(TL_BOTTOMRIGHT);
	mTrayMgr->hideCursor();
	
	// Create a params panel for displaying sample details
	StringVector items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("");
	items.push_back("Filtering");
	items.push_back("Poly Mode");
	
	mDetailsPanel = mTrayMgr->createParamsPanel(TL_NONE, "DetailsPanel", 200, items);
	mDetailsPanel->setParamValue(9, "Bilinear");
	mDetailsPanel->setParamValue(10, "Solid");
	mDetailsPanel->hide();
	mRoot->addFrameListener(this);
}

void BaseApplication::destroyScene(void) {
}

void BaseApplication::createViewports(void) {
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

void BaseApplication::setupResources(void) {
	// Load resource paths from config file
	ConfigFile cf;
	cf.load(mResourcesCfg);
	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	String secName, typeName, archName;

	while(seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;

		for(i = settings->begin(); i != settings->end(); i++) {
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

void BaseApplication::createResourceListener(void) {
}

void BaseApplication::loadResources(void) {
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void BaseApplication::go(void) {
#ifdef _DEBUG
	mPluginsCfg = ".\\Config\\plugins_d.cfg";
#else
	mPluginsCfg = ".\\Config\\plugins.cfg";
#endif
	mResourcesCfg = ".\\Config\\resources.cfg";
	
	if(!setup()) return;
	
	mRoot->startRendering();
	// Clean up
	destroyScene();
}

bool BaseApplication::setup(void) {
	mRoot = new Root(mPluginsCfg);
	setupResources();
	bool carryOn = configure();
	
	if(!carryOn) return false;
	
    chooseSceneManager();
    createCamera();
    createViewports();
	
	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	
	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();

	colocaJogador();
	
	// Create the scene
	createScene();
	
	createFrameListener();
	return true;
}

bool BaseApplication::frameRenderingQueued(const FrameEvent& evt) {
	if(mWindow->isClosed())
		return false;
	if(mShutDown)
		return false;
	
	// Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	
	mTrayMgr->frameRenderingQueued(evt);
	
	if(!mTrayMgr->isDialogVisible()) {
		mVistaMundo->frameRenderingQueued(evt); // If dialog isn't up, then update the camera
		if(mDetailsPanel->isVisible()) { // If details panel is visible, then update its contents
			mDetailsPanel->setParamValue(0, StringConverter::toString(mCamera->getDerivedPosition().x));
			mDetailsPanel->setParamValue(1, StringConverter::toString(mCamera->getDerivedPosition().y));
			mDetailsPanel->setParamValue(2, StringConverter::toString(mCamera->getDerivedPosition().z));
			mDetailsPanel->setParamValue(4, StringConverter::toString(mCamera->getDerivedOrientation().w));
			mDetailsPanel->setParamValue(5, StringConverter::toString(mCamera->getDerivedOrientation().x));
			mDetailsPanel->setParamValue(6, StringConverter::toString(mCamera->getDerivedOrientation().y));
			mDetailsPanel->setParamValue(7, StringConverter::toString(mCamera->getDerivedOrientation().z));
		}
	}

	return true;
}

bool BaseApplication::keyPressed(const KeyEvent &arg) {
	if(mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up
	
	String newVal;

	switch(arg.key) {
	case KC_F: // toggle visibility of advanced frame stats
		mTrayMgr->toggleAdvancedFrameStats();
		break;
	case KC_G: // toggle visibility of even rarer debugging details
		if(mDetailsPanel->getTrayLocation() == TL_NONE) {
			mTrayMgr->moveWidgetToTray(mDetailsPanel, TL_TOPRIGHT, 0);
			mDetailsPanel->show();
		} else {
			mTrayMgr->removeWidgetFromTray(mDetailsPanel);
			mDetailsPanel->hide();
		}
		break;
	case KC_T: // cycle polygon rendering mode
		TextureFilterOptions tfo;
		unsigned int aniso;
		
		switch(mDetailsPanel->getParamValue(9).asUTF8()[0]) {
		case 'B':
			newVal = "Trilinear";
			tfo = TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = TFO_BILINEAR;
			aniso = 1;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mDetailsPanel->setParamValue(9, newVal);
		break;
	case KC_R: // cycle polygon rendering mode
		PolygonMode pm;
		
		switch (mCamera->getPolygonMode()) {
		case PM_SOLID:
			newVal = "Wireframe";
			pm = PM_WIREFRAME;
			break;
		case PM_WIREFRAME:
			newVal = "Points";
			pm = PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = PM_SOLID;
		}
		mCamera->setPolygonMode(pm);
		mDetailsPanel->setParamValue(10, newVal);
		break;
	case KC_F5: // refresh all textures
		TextureManager::getSingleton().reloadAll();
		break;
	case KC_SYSRQ: // take a screenshot
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
		break;
	case KC_ESCAPE:
		mShutDown = true;
	}
	mVistaMundo->injectKeyDown(arg);

	return true;
}

bool BaseApplication::keyReleased(const KeyEvent &arg) {
	mVistaMundo->injectKeyUp(arg);

	return true;
}

bool BaseApplication::mouseMoved(const MouseEvent &arg) {
	if(mTrayMgr->injectMouseMove(arg)) return true;
	
	mVistaMundo->injectMouseMove(arg);
	return true;
}

bool BaseApplication::mousePressed(const MouseEvent &arg, MouseButtonID id) {
	if(mTrayMgr->injectMouseDown(arg, id)) return true;
	
	return true;
}

bool BaseApplication::mouseReleased(const MouseEvent &arg, MouseButtonID id) {
	if(mTrayMgr->injectMouseUp(arg, id)) return true;

	return true;
}

// Adjust mouse clipping area
void BaseApplication::windowResized(RenderWindow* rw) {
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);
	const MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(RenderWindow* rw) {
	// Only close for window that created OIS (the main window in these demos)
	if(rw == mWindow) {
		if(mInputManager) {
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);
			InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}