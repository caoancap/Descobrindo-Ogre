#ifndef __VistaMundo_H__
# define __VistaMundo_H__

# include "Jogador.h"
# include <Ogre.h>
# include <limits>
# include <OISEvents.h>
# include <OISInputManager.h>
# include <OISKeyboard.h>
# include <OISMouse.h>

using namespace Ogre;
using namespace OIS;
using namespace std;

class VistaMundo {
public:
	VistaMundo(SceneManager* sm);
	~VistaMundo();
	void setYawPitchDist(Radian yaw, Radian pitch, Real dist);
	void injectMouseMove(const MouseEvent& evt);
	void injectKeyDown(const KeyEvent& evt);
	void injectKeyUp(const KeyEvent& evt);
	bool frameRenderingQueued(const FrameEvent& evt);
	void stabilizesCam(Real x = Real(0), Real y = Real(0), Real z = Real(0));
	
protected:
	Camera* eCamera;
	SceneNode* nSuper;
	Jogador* nPlayer;

	Ogre::Vector3 mVelocity;
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mFastMove;
};
#endif
