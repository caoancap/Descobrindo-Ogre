#include "TutorialApplication.h"

TutorialApplication::TutorialApplication(void) {
}

TutorialApplication::~TutorialApplication(void) {
}

void TutorialApplication::createScene(void) {
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));
	Entity* ogreHead1 = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
	Entity* ogreHead2 = mSceneMgr->createEntity("Head2", "ogrehead.mesh");
	SceneNode* headNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode1", Ogre::Vector3(-50, 0, -100));
	SceneNode* headNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode2", Ogre::Vector3(+50, 0, -100));
	headNode1->attachObject(ogreHead1);
	headNode2->attachObject(ogreHead2);
	Light* light = mSceneMgr->createLight("MainLight");
	light->setSpecularColour(ColourValue());
	light->setPosition(0.0f, 50.0f, 100.0f);
}

# define WIN32_LEAN_AND_MEAN
# include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) {
	// Create application object
	TutorialApplication app;
	
	try {
	    app.go();
	} catch(Ogre::Exception& e) {
	    MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
	return 0;
}
