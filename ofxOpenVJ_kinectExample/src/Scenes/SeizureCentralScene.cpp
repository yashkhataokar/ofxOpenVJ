//
//  SeizureCentralScene.cpp
//  KinectPerformanceV01
//
//  Created by Nick Hardeman on 11/6/12.
//
//

#include "SeizureCentralScene.h"

//--------------------------------------------------------------
SeizureCentralScene::SeizureCentralScene()
{
    lastAddTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
SeizureCentralScene::~SeizureCentralScene() {
    
}

//--------------------------------------------------------------
void SeizureCentralScene::setup() {
    hue     = ofRandom(1);
    hueDir  = 1;
    bOdd    = true;
    millisDelay = 2500 ;
    contourSoundThreshold = 0.25 ; 
}

//--------------------------------------------------------------
void SeizureCentralScene::setupGui(float a_x, float a_y) {
    // creates new gui and adds the name to it //
    BaseScene::setupGui(a_x, a_y);
    
    float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;

    /*
     BaseScene::setupGui(a_x, a_y);
     
     
     gui->addSlider("FORCE RADIUS", 0.0f , 1200.0f , forceRadius, width, height) ;
     gui->addSlider("FRICTION", 0.0f , 1.0f , friction, width, height) ;
     gui->addSlider("SPRING FACTOR", 0.0f , 1.0f , springFactor, width, height) ;
     gui->addSlider("FADE AMOUNT", 0.0f , 255.0f , fadeAlpha, width, height) ;
     gui->addSlider("PARTICLE ALPHA", 0.0f , 255.0f , particleAlpha, width, height) ;
     gui->addSlider("FORCE STRENGTH", 0.0f , 400.0f , forceStrength, width, height) ;
     gui->addSlider("SAMPLING", 0.0f , 16 , sampling, width, height) ;
     gui->addSlider("FRAMES UNTIL SWITCH", 0.0 , 200 , numFramesUntilTransition, width, height) ;
     
     gui->addSlider("POINT CLOUD OFFSET X" , -3000.0 , 3000.0f , pointCloudOffset.x , width , height ) ;
     gui->addSlider("POINT CLOUD OFFSET Y" , -3000.0 , 3000.0f , pointCloudOffset.y , width , height ) ;
     gui->addSlider("POINT CLOUD OFFSET Z" , -3000.0 , 3000.0f , pointCloudOffset.z , width , height ) ;
     
     gui->addSlider("FORCE TARGET X" , -3000.0 , 3000.0f , forceTarget.x , width , height ) ;
     gui->addSlider("FORCE TARGET Y" , -3000.0 , 3000.0f , forceTarget.y , width , height ) ;
     gui->addSlider("FORCE TARGET Z" , -3000.0 , 3000.0f , forceTarget.z , width , height ) ;
     
     
     gui->addToggle("SEEK", bSeekEnabled, height, height ) ;
     gui->addToggle("SPRING ENABLED", bSpringEnabled, height, height ) ;
*/
    int width = 300 ;
    int height = 25 ;
    gui->addSlider(  "CONTOUR DELAY" ,  50.0f , 2500.0f , millisDelay , width , height) ;
     gui->addSlider( "CONTOUR SOUND THRESHOLD" ,0.0f , 1.0f , contourSoundThreshold ,  width , height ) ;
   
    ofAddListener( gui->newGUIEvent, this, &SeizureCentralScene::guiEvent );
    
}

//--------------------------------------------------------------
void SeizureCentralScene::guiEvent(ofxUIEventArgs &e) {
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "CONTOUR DELAY"  )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        millisDelay = slider->getScaledValue() ;
	}
    
    if(name == "CONTOUR SOUND THRESHOLD" )
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
        contourSoundThreshold = slider->getScaledValue() ;
	}

}

//--------------------------------------------------------------
void SeizureCentralScene::activate() {
    
}

//--------------------------------------------------------------
void SeizureCentralScene::deactivate() {
    // turn off the gui //
    BaseScene::deactivate();
}

//--------------------------------------------------------------
void SeizureCentralScene::addContour() {
    ofRectangle srcRect( 0,0, kinectMan->contourFinder.getWidth(), kinectMan->contourFinder.getHeight() );
    ofRectangle destRect( 0,0, getWidth(), getHeight() );
    ofRectangle newBounds = ofFitRectProportionallyInsideRect( srcRect, destRect );
    
    float offx = (getWidth() - newBounds.width) *.5;
    offx -= getWidth()/2;
    float offy = (getHeight() - newBounds.height) * .5f;
    offy -= getHeight()/2;
    
    vector<ofPolyline> newLines = kinectMan->getScaledContourPolyline( offx, offy, newBounds.width, newBounds.height );
    ofColor color( ofRandom(0, 255), ofRandom(10, 100), ofRandom(30, 55) );
    //ofColor color(255, 10, 50);
    color.setHue( hue*255.f );
    
    //if(bOdd) color.set(255);
    //else color.set(0);
    
    for(int i = 0; i < newLines.size(); i++ ) {
        ofPolyline resampled = newLines[i].getResampledByCount(30);
        
        ofPath path;
        for( int j = 0; j < resampled.getVertices().size(); j++) {
            if(j == 0) {
                path.newSubPath();
                path.moveTo( resampled.getVertices()[j] );
            } else {
                path.lineTo( resampled.getVertices()[j] );
            }
        }
        
        path.close();
        path.simplify();
        path.setUseShapeColor(false);
        
        path.tessellate();
        ofMesh mesh = path.getTessellation();
        
        //lines.push_back(path);
        lines.push_back( mesh );
        
        ofxParticle2D particle;
        particle.color = color;
        particle.radius = .5f; // store the scale in here //
        particles.push_back( particle );
        
        bOdd = !bOdd;
    }
    
    lastAddTime = ofGetElapsedTimeMillis();

}

//--------------------------------------------------------------
void SeizureCentralScene::update() {
    kinectMan->update();
    if ( low > contourSoundThreshold ) 
        hue += (.3f * hueDir);
    
    if(hue <= 0) {
        hueDir = 1; hue = 0;
    }
    if(hue >= 1) {
        hueDir = -1; hue = 1;
    }
    
    //cout << "hue: " << hue << " dir = " << hueDir << endl;
    
    if(kinectMan->isFrameNew()) {
        kinectMan->calculateCVOperations();
        
        if( ( ofGetElapsedTimeMillis() - lastAddTime  ) > millisDelay ) {
            addContour();
            //cout << "lines.size() = " << lines.size() << " particles.size() = " << particles.size() << endl;
        }
    }
    
    int numTooMany = lines.size()-52;
    
    if(numTooMany > 0 ) {
        lines.erase( lines.begin(), lines.begin()+numTooMany );
        particles.erase( particles.begin(), particles.begin()+numTooMany);
    }
    
    for(int i = 0; i < particles.size(); i++) {
        ofxParticle2D& p = particles[i];
        p.radius += .03f;
        
    }
    
    for(int i = 0; i < particles.size(); i++) {
        if(particles[i].radius > getHeight() / 3) {
            particles.erase( particles.begin() + i );
            break;
        }
    }
    
}

//--------------------------------------------------------------
void SeizureCentralScene::draw() {
    

    if(particles.size()>0) {
        ofColor color = particles[0].color;
        color.setBrightness(.8f * 255);
        ofSetColor( color );
//        ofRect(0, 0, getWidth(), getHeight() );
    }
    
    float pct = 1.f;
    
    ofSetLineWidth(2);
    
    for(int i = 0; i < (int)lines.size(); i++) {
        
        //pct = ((float)i/(float)lines.size());
        pct = ofMap(i, 0, lines.size(), .8, 1);
        
        ofxParticle2D& p = particles[i];
        //ofSetColor( p.color );
        
        glPushMatrix(); {
            glTranslatef(getWidth()/2, getHeight()/2, 0);
            glScalef(p.radius, p.radius, p.radius);
            
            ofColor color = p.color;
            color.setBrightness(pct * 255);
            ofSetColor( color );
            lines[i].draw();
            
        } glPopMatrix();
        
    }
    ofSetLineWidth(1);
}

//--------------------------------------------------------------
void SeizureCentralScene::drawDebug() {
    
}






