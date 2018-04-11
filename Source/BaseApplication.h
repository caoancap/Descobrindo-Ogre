#ifndef __BaseApplication_h_
# define __BaseApplication_h_

# include <OgreCamera.h>
# include <OgreEntity.h>
# include <OgreLogManager.h>
# include <OgreRoot.h>
# include <OgreViewport.h>
# include <OgreSceneManager.h>
# include <OgreRenderWindow.h>
# include <OgreConfigFile.h>
# include <OISEvents.h>
# include <OISInputManager.h>
# include <OISKeyboard.h>
# include <OISMouse.h>
# include <SdkTrays.h>
# include "VistaMundo.h"

using namespace Ogre;
using namespace OgreBites;
using namespace OIS;
using namespace std;

class BaseApplication : public FrameListener, public WindowEventListener, public KeyListener, public MouseListener, SdkTrayListener {
public:
	BaseApplication(void);
	virtual ~BaseApplication(void);
	virtual void go(void);

protected:
	virtual bool setup();
	virtual bool configure(void);
	virtual void chooseSceneManager(void);
	virtual void colocaJogador(void);
	virtual void createCamera(void);
	virtual void createFrameListener(void);
	virtual void createScene(void) = 0;
	virtual void destroyScene(void);
	virtual void createViewports(void);
	virtual void setupResources(void);
	virtual void createResourceListener(void);
	virtual void loadResources(void);
	virtual bool frameRenderingQueued(const FrameEvent& evt);
	virtual bool keyPressed(const KeyEvent &arg);
	virtual bool keyReleased(const KeyEvent &arg);
	virtual bool mouseMoved(const MouseEvent &arg);
	virtual bool mousePressed(const MouseEvent &arg, MouseButtonID id);
	virtual bool mouseReleased(const MouseEvent &arg, MouseButtonID id);
	virtual void windowResized(RenderWindow* rw);
	virtual void windowClosed(RenderWindow* rw);
	
	Root* mRoot;
	Camera* mCamera;
	SceneManager* mSceneMgr;
	RenderWindow* mWindow;
	String mResourcesCfg;
	String mPluginsCfg;
	OverlaySystem* mOverlaySystem;
	
	// OgreBites
	InputContext mInputContext;
	SdkTrayManager* mTrayMgr;
	ParamsPanel* mDetailsPanel; // Sample details panel
	bool mCursorWasVisible;	// Was cursor visible before dialog appeared?
	bool mShutDown;
	
	//OIS Input devices
	InputManager* mInputManager;
	Mouse* mMouse;
	Keyboard* mKeyboard;

	VistaMundo* mVistaMundo; // Basic camera controller
};

#endif