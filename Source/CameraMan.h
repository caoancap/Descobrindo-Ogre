#ifndef __CameraMan_H__
# define __CameraMan_H__

# include <Ogre.h>
# include <limits>
# include <OISEvents.h>
# include <OISInputManager.h>
# include <OISKeyboard.h>
# include <OISMouse.h>

using namespace Ogre;
using namespace OIS;

class CameraMan {
public:
	CameraMan(Camera* cam);
	~CameraMan();
	void setCamera(Camera* cam);
	Camera* getCamera();
	void setTarget(SceneNode* target);
	SceneNode* getTarget();
	void setYawPitchDist(Radian yaw, Radian pitch, Real dist);
	void injectMouseMove(const MouseEvent& evt);
	
protected:

	Camera* mCamera;
	SceneNode* mTarget;
};
#endif
