#include "CameraMan.h"

CameraMan::CameraMan(Camera* cam)
	: mCamera(0)
	, mTarget(0)
{
	setCamera(cam);
	setTarget(mTarget ? mTarget : mCamera->getSceneManager()->getRootSceneNode());
	mCamera->setFixedYawAxis(true);
	setYawPitchDist(Degree(0), Degree(15), 150);
}

CameraMan::~CameraMan() {}

void CameraMan::setCamera(Camera* cam) {
	mCamera = cam;
}

Camera* CameraMan::getCamera() {
	return mCamera;
}

void CameraMan::setTarget(SceneNode* target) {
	if (target != mTarget) {
		mTarget = target;
		if(target) {
			setYawPitchDist(Degree(0), Degree(15), 150);
			mCamera->setAutoTracking(true, mTarget);
		} else {
			mCamera->setAutoTracking(false);
		}
	}
}

SceneNode* CameraMan::getTarget() {
	return mTarget;
}

void CameraMan::setYawPitchDist(Radian yaw, Radian pitch, Real dist) {
	mCamera->setPosition(mTarget->_getDerivedPosition());
	mCamera->setOrientation(mTarget->_getDerivedOrientation());
	mCamera->yaw(yaw);
	mCamera->pitch(-pitch);
	mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
}

void CameraMan::injectMouseMove(const MouseEvent& evt) {
	Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();

	mCamera->setPosition(mTarget->_getDerivedPosition());

	mCamera->yaw(Degree(-evt.state.X.rel * 0.25f));
	mCamera->pitch(Degree(-evt.state.Y.rel * 0.25f));
	mTarget->yaw(Degree(-evt.state.X.rel * 0.25f));

	mCamera->moveRelative(Ogre::Vector3(0, 0, dist));

	if(evt.state.Z.rel != 0) {
		mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
	}
}
