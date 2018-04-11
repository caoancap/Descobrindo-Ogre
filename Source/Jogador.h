#ifndef __Jogador_H__
# define __Jogador_H__

# include "NMundo.h"
# include <Ogre.h>
# include <limits>
# include <OISEvents.h>
# include <OISInputManager.h>
# include <OISKeyboard.h>
# include <OISMouse.h>

using namespace Ogre;
using namespace OIS;
using namespace std;

class Jogador {
public:
	Jogador(SceneManager* sm);
	~Jogador();
	void setYawPitchDist(Radian yaw, Radian pitch, Real dist);
	void injectMouseMove(const MouseEvent& evt);
	void injectKeyDown(const KeyEvent& evt);
	void injectKeyUp(const KeyEvent& evt);
	bool frameRenderingQueued(const FrameEvent& evt);
	void stabilizesCam(Real x = Real(0), Real y = Real(0), Real z = Real(0));
	const Ogre::Vector3 &getPosition();
	static Jogador* getJogador(SceneManager* sm);
	
protected:
	Camera* eCamera;
	SceneNode* nSuper;
	SceneNode* nPlayer;

	Ogre::Vector3 mVelocity;
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mFastMove;
};

#endif
