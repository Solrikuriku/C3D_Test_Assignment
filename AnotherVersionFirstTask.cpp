#include <qt_vision.h>
#include <vsn_rectanglegeometry.h>
#include <vsn_cylindergeometry.h>
#include <vsn_torusgeometry.h>
#include <vsn_colorarray.h>
#include <vsn_polygongeometry.h>
#include <qt_vision.h>
#include <vsn_last.h>
#include <action_solid.h>
#include <action_mesh.h>
#include <mb_data.h>
#include <solid.h>
#include <mesh.h>
#include <vsn_scenesegment.h>     
#include <vsn_geometryrep.h>   

#include <vsn_last.h>

using namespace VSN;
using namespace QtVision;

static NodeKey createShapeSegment(GeometryRep* pShapeRep, const Vector3DF& vecMove, const Color& color, SceneSegment* pParent)
{
    SceneSegment* pSegment = new SceneSegment(pShapeRep, pParent);
    if (color.IsValid())
        pSegment->SetColorPresentationMaterial(color);
    pSegment->GetTransform().Move(vecMove);
    return pSegment->GetUniqueKey();
}

class ElementarySolidGenerator
{
public:
    //Удаляем конструктор чтобы не создать пустышку
    ElementarySolidGenerator() = delete;

    //Генерация тора: большой радиус, малый радиус, количество колец и долек
    static GeometryRep* GenerateRepTorus(const int majorRadius = 10, const int minorRadius = 5, const int rings = 30, const int slices = 30)
    {
        GeometryRep* shape = new GeometryRep();
        TorusGeometry* torus = new TorusGeometry();

        torus->SetMajorRadius(majorRadius);
        torus->SetMinorRadius(minorRadius);
        torus->SetRingStep(rings);
        torus->SetSliceStep(slices);
        shape->AddGeometry(torus);

        return shape;
    }

    //Генерация цилиндра: радиус и высота
    static GeometryRep* GenerateRepCylinder(const int radius = 10, const int height = 10)
    {
        GeometryRep* shape = new GeometryRep();
        CylinderGeometry* cylinder = new CylinderGeometry();

        cylinder->SetRadius(radius);
        cylinder->SetHeight(height);
        shape->AddGeometry(cylinder);

        return shape;
    }
};

int main(int argc, char **argv) 
{
    auto app = QtVision::QtApplication::createApplication(argc, argv, "Basics geometries");
    if (app == nullptr)
        return 0;

    QtVision::QtSdiMainWindow mainWindow;
    mainWindow.processManager()->createProcessesCameraControls();
    SceneSegment* pTopSegment = mainWindow.sceneWidget()->sceneContent()->GetRootSegment();
    Q_ASSERT(pTopSegment != nullptr);

    Vector3DF move = Vector3DF(0.f, 0.f, 0.f);
    Color color = Color(128, 128, 128);

    //GeometryRep* pShapeRep = ElementarySolidGenerator::GenerateRepTorus(); //Тор
    //GeometryRep* pShapeRep = ElementarySolidGenerator::GenerateRepCylinder(); //Цилиндр

    //if (!pShapeRep) return 0;

    //::createShapeSegment(pShapeRep, move, color, pTopSegment);    


    mainWindow.show();
    GlobalFlags::SetFaceCulling(CullFaces::None);
    mainWindow.sceneWidget()->mainLight()->SetDoubleSided(true);
    mainWindow.sceneWidget()->viewport()->GetCamera()->SetViewOrientation(Orientation::Front);
    mainWindow.sceneWidget()->viewport()->ZoomToFit(mainWindow.sceneWidget()->sceneContent()->GetBoundingBox());

    return app->exec();
}