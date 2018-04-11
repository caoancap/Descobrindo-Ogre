#include "Jogador.h"

Jogador::Jogador(SceneManager* sm)
	: nPlayer(0)
	, mVelocity(Ogre::Vector3::ZERO)
	, mGoingForward(false)
	, mGoingBack(false)
	, mGoingLeft(false)
	, mGoingRight(false)
	, mFastMove(false)
{
	SceneManager* sm = cam->getSceneManager();
	Entity* player = sm->createEntity(Mundo::e_jogador, "ogrehead.mesh");
	eCamera = cam;

	nSuper  = sm->getRootSceneNode()->createChildSceneNode(Mundo::sn_jogador, Ogre::Vector3::ZERO);
	nPlayer = nSuper->createChildSceneNode(Mundo::n_jogador, Ogre::Vector3::ZERO);

	nSuper->attachObject(eCamera);
	nPlayer->attachObject(player);

	nPlayer->yaw(Degree(180));
	setYawPitchDist(Degree(0), Degree(15), 500);
}

Jogador::~Jogador() {
}

void Jogador::setYawPitchDist(Radian yaw, Radian pitch, Real dist) {
	eCamera->setPosition(nSuper->_getDerivedPosition());
	eCamera->setOrientation(nSuper->_getDerivedOrientation());
	eCamera->yaw(yaw);
	eCamera->pitch(-pitch);
	eCamera->moveRelative(Ogre::Vector3(0, 0, dist));
}

void Jogador::stabilizesCam(Real x, Real y, Real z) {
	Real dist = (eCamera->getPosition() - nPlayer->getPosition()).length();

	eCamera->setPosition(nPlayer->getPosition());
	if(z != 0) {
		dist += z * -0.0008f * dist;
		if(dist > 2000) {
			dist = 2000;
		} else if(dist < 200) {
			dist = 200;
		}
	}
	Degree pitch = Degree(y * -0.2f);
	Real rota = eCamera->getOrientation().getPitch().valueDegrees() + pitch.valueDegrees();
	if(rota < 80 && rota > -80)
		eCamera->pitch(pitch);
	
	nSuper->yaw(Degree(x * -0.2f));
	eCamera->moveRelative(Ogre::Vector3(0, 0, dist));

	//LogManager::getSingletonPtr()->logMessage("1 " + StringConverter::toString(eCamera->getOrientation().getPitch().valueRadians()));
}

void Jogador::injectMouseMove(const MouseEvent& evt) {
	stabilizesCam(Real(evt.state.X.rel), Real(evt.state.Y.rel), Real(evt.state.Z.rel));
}

void Jogador::injectKeyDown(const KeyEvent& evt) {
	switch(evt.key) {
	case KC_W:
	case KC_UP:
		mGoingForward = true;
		break;
	case KC_S:
	case KC_DOWN:
		mGoingBack = true;
		break;
	case KC_A:
	case KC_LEFT:
		mGoingLeft = true;
		break;
	case KC_D:
	case KC_RIGHT:
		mGoingRight = true;
		break;
	case KC_LSHIFT:
		mFastMove = true;
	}
}

void Jogador::injectKeyUp(const KeyEvent& evt)
{
	switch(evt.key) {
	case KC_W:
	case KC_UP:
		mGoingForward = false;
		break;
	case KC_S:
	case KC_DOWN:
		mGoingBack = false;
		break;
	case KC_A:
	case KC_LEFT:
		mGoingLeft = false;
		break;
	case KC_D:
	case KC_RIGHT:
		mGoingRight = false;
		break;
	case KC_LSHIFT:
		mFastMove = false;
	}
}


bool Jogador::frameRenderingQueued(const FrameEvent& evt) {
	Ogre::Vector3 accel = Ogre::Vector3::ZERO;
	if(mGoingForward)
		accel += nSuper->getOrientation() * Ogre::Vector3(+000, +000, -100);
	if(mGoingBack)
		accel += nSuper->getOrientation() * Ogre::Vector3(+000, +000, +100);
	if(mGoingRight)
		accel += nSuper->getOrientation() * Ogre::Vector3(+100, +000, +000);
	if(mGoingLeft)
		accel += nSuper->getOrientation() * Ogre::Vector3(-100, +000, +000);
	
	Real topSpeed = mFastMove ? Real(1000) : Real(300);
	if (accel.squaredLength() != 0) {
		accel.normalise();
		mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
	} else
		mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;
	
	Real tooSmall = numeric_limits<Real>::epsilon();
	if (mVelocity.squaredLength() > topSpeed * topSpeed) {
		mVelocity.normalise();
		mVelocity *= topSpeed;
	} else if(mVelocity.squaredLength() < tooSmall * tooSmall)
		mVelocity = Ogre::Vector3::ZERO;
	
	if(mVelocity != Ogre::Vector3::ZERO) {
		Ogre::Vector3 aux = mVelocity * evt.timeSinceLastFrame;
		nSuper->translate(aux);
	}
	return true;
}