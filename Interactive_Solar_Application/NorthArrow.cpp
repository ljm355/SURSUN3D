#include "NorthArrow.h"
#include "osg\StateSet"
#include "osg\NodeCallback"
#include "osgUtil\CullVisitor"
#include "osgViewer\Viewer"
#include <string>
#include "osgDB\ReadFile"
#include "osg\Texture2D"
#include "osg\BlendFunc"
#include "osg\AlphaFunc"
#include "osg\MatrixTransform"
#include "osg\Program"

osg::StateSet* g_pNorthArrowStateSet;
struct NorthArrowCallback : public osg::NodeCallback
{
public:
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		if( nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
		{ 
			osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(nv);
			osg::Vec3 eye,center,up;
			cv->getCurrentCamera()->getViewMatrixAsLookAt(eye,center,up);

			osg::Vec3 look = center - eye;
			look.z() = 0;
			look.normalize();
			osg::Vec3 north(0,1,0);
			osg::Matrix mat = osg::Matrix::rotate(look,north);
			g_pNorthArrowStateSet->getUniform("matBearing")->set(mat);
		}
		traverse(node,nv);
	}
};


osg::Node* createNorthArrow( osgViewer::Viewer* viewer, std::string filename )

{
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	unsigned int swidth, sheight;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), swidth, sheight);
	osg::Projection * HUDProjectionMatrix = new osg::Projection;

	HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,swidth,0,sheight));
	osg::MatrixTransform* HUDModelViewMatrix = new osg::MatrixTransform;
	osg::Geode* HUDGeode = new osg::Geode;
	osg::Geometry* HUDBackgroundGeometry = new osg::Geometry();
	osg::Image* hudImage;
	hudImage = osgDB::readImageFile(filename);
	hudImage->scaleImage(120,120,1);
	osg::Vec3Array* HUDBackgroundVertices = new osg::Vec3Array;
	HUDBackgroundVertices->push_back( osg::Vec3(-hudImage->s() * 0.5, -hudImage->t()* 0.5,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(hudImage->s() * 0.5,  -hudImage->t()* 0.5,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(hudImage->s() * 0.5,hudImage->t()* 0.5,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(-hudImage->s() * 0.5,hudImage->t()* 0.5,-1) );

	osg::DrawElementsUInt* HUDBackgroundIndices =
		new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
	HUDBackgroundIndices->push_back(0);
	HUDBackgroundIndices->push_back(1);
	HUDBackgroundIndices->push_back(2);
	HUDBackgroundIndices->push_back(3);

	//osg::Vec4Array* HUDcolors = new osg::Vec4Array;
	//HUDcolors->push_back(osg::Vec4(0.8f,0.8f,0.8f,0.8f));

	osg::Vec2Array* texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f,0.0f);
	(*texcoords)[1].set(1.0f,0.0f);
	(*texcoords)[2].set(1.0f,1.0f);
	(*texcoords)[3].set(0.0f,1.0f);

	HUDBackgroundGeometry->setTexCoordArray(0,texcoords);
	osg::Texture2D* HUDTexture = new osg::Texture2D;
	HUDTexture->setDataVariance(osg::Object::DYNAMIC);

	HUDTexture->setImage(hudImage);
	HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices);
	HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices);

	HUDGeode->addDrawable(HUDBackgroundGeometry);
	osg::StateSet* stateset = HUDProjectionMatrix->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	//stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	//stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	osg::BlendFunc* bf = new osg::BlendFunc(
		osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
	stateset->setAttributeAndModes( bf );
	// Turn on alpha testing
	osg::AlphaFunc* af = new osg::AlphaFunc(
		osg::AlphaFunc::GREATER, 0.05f );
	stateset->setAttributeAndModes( af );
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setTextureAttributeAndModes(0,HUDTexture,osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	osg::Program* program = new osg::Program;
	stateset->setAttribute(program,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	char vertexShaderSource[] = 
		"uniform mat4 matBearing;\n"
		"void main(void)\n"
		"{\n"
		"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"   gl_Position    = gl_ModelViewProjectionMatrix * matBearing *  gl_Vertex;\n"
		"}\n";


	char fragmentShaderSource[] = 
		"uniform sampler2D tex; \n"
		"\n"
		"void main(void) \n"
		"{\n"
		"       vec4 color = texture2D(tex, gl_TexCoord[0].xy);\n"
		"       gl_FragColor = color;\n"
		"}\n";

	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSource));

	stateset->setAttribute(program,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	stateset->addUniform(new osg::Uniform("tex",0));
	stateset->addUniform(new osg::Uniform("matBearing",osg::Matrix::identity()));

	HUDModelViewMatrix->setMatrix(osg::Matrix::translate(swidth-hudImage->s() * 0.5,sheight-hudImage->t() * 1,0));
	// above it in the scene graph:
	HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	HUDModelViewMatrix->addChild( HUDGeode );
	HUDProjectionMatrix->addChild(HUDModelViewMatrix);
	g_pNorthArrowStateSet = stateset;

	HUDProjectionMatrix->setCullCallback(new NorthArrowCallback);
	return HUDProjectionMatrix;

}

