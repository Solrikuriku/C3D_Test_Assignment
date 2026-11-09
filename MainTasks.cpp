#include <qt_vision.h>
#include <vsn_last.h>
#include <action_solid.h>
#include <action_mesh.h>
#include <mb_data.h>
#include <gce_types.h>
#include <gce_api.h>
#include <cur_line_segment3d.h>
#include "gce_callback.h"
#include <op_boolean_flags.h>
#include <tri_face.h>
#include <solid.h>
#include <wire_frame.h>
#include <mesh_provider.h>
#include <polymesh.h>
#include <op_polymesh_parameter.h>
#include <op_mesh_parameter.h>
#include <action_polymesh.h>
#include <mb_data.h>
#include <mip_solid_area_volume.h.>
#include <mesh.h>
#include <surf_spine.h>
#include <space_item.h>
#include <cur_curve_spiral.h>
#include <cur_contour3d.h>
#include <mb_placement3d.h>
#include <mb_vector3d.h>
#include <mb_cart_point3d.h>
#include <mb_nurbs_function.h>
#include <mb_point_mating.h>
#include <cur_nurbs3d.h>
#include <space_instance.h>
#include <vsn_pen.h>
#include <vsn_mathgeometry.h>
#include <op_shell_parameter.h>
#include <op_swept_parameter.h>
#include <vsn_scenesegment.h>
#include <vsn_feature.h>
#include <vsn_geometryrep.h>   
#include <vsn_geometryfactory.h>
#include <vsn_segmentfeatures.h>
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <cmath>

#define RADIANS(x) ((x) * (M_PI / 180.0f))
#define FULLTURN 2.0 * M_PI; //один оборот

using namespace VSN;
using namespace QtVision;

//базовый код на три точки
//сделано для удобства читаемости кода
//но учитывать, что не все объекты требуют ровно три точки
static c3d::SpacePointsVector BasePointsVector()
{
    return { { 0.0f, 0.0f, 0.0f }, { 20.0f, 0.0f, 0.0f }, { 20.0f, 10.0f, 0.0f } };
}

MbElementarySolidResults MakeElementarySolid(ElementaryShellType type, const c3d::SpacePointsVector& points, const MbSNameMaker& name = MbSNameMaker())
{
    MbElementarySolidResults result;
    MbElementarySolidParams params = MbElementarySolidParams(type, points, name);
    ::ElementarySolid(params, result);

    return result;
}

void AddItemToScene(const SPtr<MbItem>& object, SceneSegment* seg)
{
    MathRepresentation* resultMath = new MathRepresentation();
    resultMath->SetMathItem(object, 0.2);
    new SceneSegment(resultMath, seg);
};

//Здесь строим ромб через Solver
std::array<MbCartPoint, 4> CalculateRhombusPoints(const double angle, const double L = 40.0f)
{
    GCE_system system = GCE_CreateSystem();

    double radAngle = RADIANS(angle);

    //хардкодинг
    GCE_point A;
    A.x = 0.0;
    A.y = 0.0;

    GCE_point B;
    B.x = A.x + L;
    B.y = A.y;

    GCE_point D;
    D.x = A.x + L * std::cos(radAngle);
    D.y = A.y + L * std::sin(radAngle);

    GCE_point C;
    C.x = B.x + D.x - A.x;
    C.y = B.y + D.y - A.y;

    geom_item pA = GCE_AddPoint(system, A);
    geom_item pB = GCE_AddPoint(system, B);
    geom_item pC = GCE_AddPoint(system, C);
    geom_item pD = GCE_AddPoint(system, D);

    geom_item ab[2] = { pA, pB };
    geom_item bc[2] = { pB, pC };
    geom_item cd[2] = { pC, pD };
    geom_item ad[2] = { pA, pD };

    geom_item AB = GCE_AddLineSeg(system, ab);
    geom_item BC = GCE_AddLineSeg(system, bc);
    geom_item CD = GCE_AddLineSeg(system, cd);
    geom_item AD = GCE_AddLineSeg(system, ad);

    GCE_dim_pars lenPars{};
    lenPars.var = GCE_NULL_V;
    lenPars.dimValue = L;

    GCE_AddLength(system, AB, lenPars);

    ::GCE_AddEqualLength(system, AB, BC);
    ::GCE_AddEqualLength(system, BC, CD);
    ::GCE_AddEqualLength(system, CD, AD);

    GCE_adim_pars pars;
    pars.dPars.var = GCE_NULL_V;
    pars.dPars.dimValue = radAngle;                 
    pars.factor = 1; //некий коэффициент для умножения
    pars.adjacent = false; //смежный или нет         

    constraint_item angleA = ::GCE_AddAngle(system, AB, AD, pars);

    //попытка применить ограничения
    //на данный момент не работает и нуждается в пересмотре
    //выводит GCE_RESULT_None
    GCE_result res = ::GCE_Evaluate(system);

    GCE_point calcA = GCE_GetPointXY(system, pA);
    GCE_point calcB = GCE_GetPointXY(system, pB);
    GCE_point calcC = GCE_GetPointXY(system, pC);
    GCE_point calcD = GCE_GetPointXY(system, pD);

    GCE_RemoveSystem(system);

    MbCartPoint A2D(calcA.x, calcA.y);
    MbCartPoint B2D(calcB.x, calcB.y);
    MbCartPoint C2D(calcC.x, calcC.y);
    MbCartPoint D2D(calcD.x, calcD.y);

    std::array<MbCartPoint, 4> rhombusPoints{ A2D, B2D, C2D, D2D };

    return rhombusPoints;
}

void RhombusSolverExample(SceneSegment* topSegment, const double angle, const double L = 40.0f, const double countTurns = 3.0, const double radius = 20.0, const double step = 60.0)
{
    auto rhombusPoints = CalculateRhombusPoints(angle);

    SPtr<MbLineSegment> AB(new MbLineSegment(rhombusPoints[0], rhombusPoints[1]));
    SPtr<MbLineSegment> BC(new MbLineSegment(rhombusPoints[1], rhombusPoints[2]));
    SPtr<MbLineSegment> CD(new MbLineSegment(rhombusPoints[2], rhombusPoints[3]));
    SPtr<MbLineSegment> DA(new MbLineSegment(rhombusPoints[3], rhombusPoints[0]));

    SPtr<MbContour>rhombusContour2D(new MbContour());

    rhombusContour2D->AddSegment(AB);
    rhombusContour2D->AddSegment(BC);
    rhombusContour2D->AddSegment(CD);
    rhombusContour2D->AddSegment(DA);

    //const double radius = 5.0; //радиус цилиндрика
    //const double step = 60.0; //подъем за оборот
    double rise = step / FULLTURN; //подъем за один радиан

    MbCartPoint3D startPoint(radius, 0.0, 0.0); //стартовая точка
    MbVector3D tangent(0.0, radius, rise); //касательная
    double len = sqrt((radius * radius) + (rise * rise)); //длина
    tangent *= (1.0 / len); //нормирование касательной

    MbCartPoint3D origin(0.0, 0.0, 0.0);
    MbVector3D axisX(1.0, 0.0, 0.0);
    MbVector3D axisY(0.0, 1.0, 0.0); //вместо axisZ - так почему-то ровнее

    MbPlacement3D helixPlacement(origin, axisY, axisX); //система координат для винтовой
    MbPlacement3D sectionPlacement(startPoint, tangent, axisX); //для сечения

    //данные об образующей операции движения
    MbSweptData sweptData(sectionPlacement, *rhombusContour2D);
    /*SPtr<MbSweptData> sweptData (new MbSweptData(sectionPlacement, *rhombusContour2D));*/

    double start = 0.0; //стартовая точка
    double turns = countTurns * FULLTURN;
    MbCurveSpiral helixCylinder(helixPlacement, radius, step, start, turns); //цилиндрическая спиралька
    MbSpine* spine(MbSpine::Create(helixCylinder, true, axisY, false)); //направляющая для поверхности
    //риск утечки из-за сырого указателя (?)

    EvolutionValues values;

    //наименования для имен
    MbSNameMaker opNameMaker, contourNameMaker, solidNameMaker;
    PArray<MbSNameMaker> contoursNames(0, 1, false);
    contoursNames.Add(&contourNameMaker);

    //вычисляем заметание
    c3d::SolidSPtr resultSolid;
    ::MbEvolutionShellParams evShellParams(sweptData, *spine, values, opNameMaker, contoursNames, solidNameMaker);
    MbResultType result = ::EvolutionShell(evShellParams, resultSolid);

    if (result != rt_Success) return;

    AddItemToScene(resultSolid, topSegment);
}

//добавить типы объекта и типы вывода?
void BooleanExample(SceneSegment* topSegment, const ElementaryShellType typeA, const ElementaryShellType typeB, const OperationType op)
{
    //захардкожено под примеры с цилиндром и сферой
    //нужно учесть что другие типы могут требовать не три точки
    c3d::SpacePointsVector pointsA = { { 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, -5.0f }, { 10.0f, 0.0f, 0.0f } };
    c3d::SpacePointsVector pointsB = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 20.0f }, { 6.0f, 6.0f, 0.0f } };

    MbElementarySolidResults objectA = MakeElementarySolid(typeA, pointsA);
    MbElementarySolidResults objectB = MakeElementarySolid(typeB, pointsB);

    c3d::SolidSPtr solidObjectA = objectA.GetResultSolid();
    c3d::SolidSPtr solidObjectB = objectB.GetResultSolid();

    if (!solidObjectA || !solidObjectB) return;

    auto MakeBoolean = [](const c3d::SolidSPtr& objA, const c3d::SolidSPtr& objB, OperationType booleanOperation, MbeCopyMode copyMode1 = cm_Copy,
        MbeCopyMode copyMode2 = cm_Copy, const MbSNameMaker& name = MbSNameMaker(), const MbBooleanFlags& booleanFlags = MbBooleanFlags())
        {
            if (!objA || !objB) return c3d::SolidSPtr{};

            c3d::SolidSPtr solidA(objA);
            c3d::SolidSPtr solidB(objB);
            MbBooleanOperationParams params(booleanOperation, booleanFlags, name);
            c3d::SolidSPtr result;

            ::BooleanResult(solidA, copyMode1, solidB, copyMode2, params, result);

            return result;
        };

    c3d::SolidSPtr booleanSolid = MakeBoolean(solidObjectA, solidObjectB, op);

    if (!booleanSolid) return;

    //сдвигаем исходные объекты вместе для примера
    solidObjectA->Move(MbVector3D(15.0, 20.0, 0.0));
    solidObjectB->Move(MbVector3D(15.0, 20.0, 0.0));

    AddItemToScene(booleanSolid, topSegment);
    AddItemToScene(solidObjectA, topSegment);
    AddItemToScene(solidObjectB, topSegment);
}

void FilletExample(SceneSegment* topSegment, const ElementaryShellType type, const double fillet = 3.0f)
{
    auto points = BasePointsVector();
    MbElementarySolidResults cylinderResult = MakeElementarySolid(type, points);
    c3d::SolidSPtr cylinder = cylinderResult.GetResultSolid();
    c3d::EdgesSPtrVector edges;
    c3d::FacesSPtrVector faces;
    cylinder->GetEdges(edges);
    cylinder->GetFaces(faces);

    SmoothValues smooth;
    smooth.SetSmoothCorner(SmoothValues::ec_either); //пока хардкод
    smooth.InitEquidistantFillet(fillet);
    MbShellFilletValues filletParams = MbShellFilletValues(edges, faces, smooth, MbSNameMaker());

    c3d::SolidSPtr result;
    ::FilletSolid(*cylinder, cm_Same, filletParams, result);

    AddItemToScene(result, topSegment);
}

void TriangulationExample(SceneSegment* topSegment, const ElementaryShellType type, const MbeStepType stepType = MbeStepType::ist_SpaceStep, const double sag = 5.0f)
{
    auto points = BasePointsVector();
    MbElementarySolidResults solidResult = MakeElementarySolid(type, points);
    c3d::SolidSPtr solid = solidResult.GetResultSolid();
    MbStepData data(stepType, sag);
    SPtr<MbMesh> mesh(new MbMesh());
    solid->CalculateGrid(data, *mesh);

    mesh->ResetRefItems();
    AddItemToScene(mesh, topSegment);
}

void RemeshingExample(SceneSegment* topSegment, const ElementaryShellType type, const MbeStepType stepType = MbeStepType::ist_SpaceStep, const double sag = 1.0f)
{
    auto points = BasePointsVector();
    MbElementarySolidResults solidResult = MakeElementarySolid(type, points);

    c3d::SolidSPtr solid = solidResult.GetResultSolid();
    MbStepData data(stepType, sag);
    SPtr<MbMesh> mesh(new MbMesh());
    MbFormNote note;
    //экспериментальное задание параметров для полигонального объекта
    //note.SetGrid(true); note.SetWire(true); note.SetSeam(true);
    solid->CalculateMesh(data, note, *mesh);
    mesh->ResetRefItems();

    //риск утечки (?)
    IMeshProvider* meshProvider(IMeshProvider::Create(*mesh));
    MbRepairInconsistentMeshParams repairParams;
    SPtr<MbPolymesh> polymesh (new MbPolymesh(*meshProvider, repairParams));

    c3d::IndicesVector fixedEdges;
    MbRemeshParams remeshParams;
    remeshParams.InitIncrementalUniform(1.0, fixedEdges);
    ::Remesh(remeshParams, *polymesh);

    AddItemToScene(polymesh, topSegment);
}

void ElementarySolidExample(SceneSegment* topSegment, const ElementaryShellType type)
{
    auto points = BasePointsVector();
    MbElementarySolidResults objectResult = MakeElementarySolid(type, points);
    c3d::SolidSPtr elementaryObject = objectResult.GetResultSolid();
    AddItemToScene(elementaryObject, topSegment);
}

int main(int argc, char** argv)
{
    auto app = QtApplication::createApplication(argc, argv, "Examples");
    if (!app)
        return 1;

    QtSdiMainWindow mainWindow;
    QtOpenGLSceneWidget* sceneWidget = mainWindow.sceneWidget();
    mainWindow.processManager()->createProcessesCameraControls();
    SceneSegment* pTopSegment = sceneWidget->sceneContent()->GetRootSegment();
    Q_ASSERT(pTopSegment != nullptr);

    //создание элементарных тел
    //ElementarySolidExample(pTopSegment, et_Torus);
    //ElementarySolidExample(pTopSegment, et_Cylinder);

    //BooleanExample(pTopSegment, et_Sphere, et_Cylinder, bo_Difference); //вызов були
    //FilletExample(pTopSegment, et_Cylinder); //вызов скругления ребер

    //тела заметания с ромбиками 30 и 40 градусов
    //RhombusSolverExample(pTopSegment, 30.0f);
    //RhombusSolverExample(pTopSegment, 40.0f);

    //триангуляция полигонального тела
    //TriangulationExample(pTopSegment, et_Torus);
    //ремешинг полигонального тела
    //RemeshingExample(pTopSegment, et_Torus);

    mainWindow.show();
    GlobalFlags::SetFaceCulling(CullFaces::None);
    sceneWidget->mainLight()->SetType(Light::PointLight);
    sceneWidget->mainLight()->SetDoubleSided(true);
    sceneWidget->viewport()->GetCamera()->SetViewOrientation(Orientation::Front);
    sceneWidget->viewport()->ZoomToFit(sceneWidget->sceneContent()->GetBoundingBox());
    
    return app->exec();
}