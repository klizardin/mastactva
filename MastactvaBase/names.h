#ifndef NAMES_H
#define NAMES_H


#include <QChar>


static const int g_userId = 1;


static const char *g_englishLanguage = "English";
static const char *g_belarusLanguage = "Belarus";
static const char *g_shaderName = "shader";
static const char *g_argumentName = "argument";
static const char *g_storageName = "storage";
static const char *g_precisionName = "precision";
static const char *g_typeName = "type";
static const char *g_nameName = "name";
static const char *g_defaultValueName = "defaultValue";
static const char *g_descriptionName = "description";
static const char *g_geometrySolidName = "geomentrySolid";
static const char *g_geometryFacedName = "geomentryFaced";
static const char *g_geometrySizeName = "geomentrySize";
static const char *g_facedGeometryCoefName = "facedGeometryCoef";
static const char *g_colorsAttributeName = "attributeColors";
static const char *g_artefactsRelPath = "artefacts";
static const char *g_imagesRelPath = "images";
static const char *g_artefactTypeVertex = "Vertex";
static const char *g_artefactTypeFragment = "Fragment";
static const char *g_allImagesOfGalleryModel = "AllImagesOfGallery";
static const char *g_sqlText = "TEXT";
static const char *g_sqlInt = "INTEGER";
static const char *g_sqlDouble = "REAL";
static const char *g_trueName = "TRUE";
static const char *g_dbNameRW = "mastactva_rw";
static const char *g_dbNameRO = "mastactva_ro";
static const char *g_dbNameExt = ".db3";
static const char *g_splitTableRef = "_by_";
static const char *g_insertFieldSpliter = " , ";
static const char *g_spaceName = " ";
static const char *g_refPrefix = "ref_";
static const char *g_bindPrefix = ":";
static const char *g_errorDetailTag = "detail";
static const char *g_noImage = ":/resources/no-image.png";
static const char *g_noImageQRC = "qrc:///resources/no-image.png";
static const char *g_createDefaultEffectObjectProcedureName = "create_default_object";
static const char *g_effectIDName = "id";
static const char *g_defaultObjectInfoNameName = "name";
static const char *g_defaultObjectInfoProgrammerNameName = "programmer_name";
static const char *g_jsonDataVariableNameObjectListName = "objectsToRun";
static const char *g_defaultObjectInfoDescriptionName = "description";
static const char *g_defaultObjectInfoCreatedName = "created";
static const char *g_defaultObjectInfoProgrammerName = "";
static const char *g_randArgumentValueName = "__rand";
static const char *g_currentSpecialwordName = "__current";
static const char *g_vertexSpecialwordName = "_vertex";
static const char *g_vertexTextureSpecialwordName = "_texture";
static const char *g_vertexNormalSpecialwordName = "_normal";
static const char *g_indexesSpecialwordName = "_indexes";
static const char *g_mainSpecialwordName = "__main";
static const char *g_defaultObjectNameName = "default";
static const char *g_firstSpecialwordName = "__first";
static const char *g_lastSpecialwordName = "__last";
static const char *g_renderFromImageName = "renderFromImage";
static const char *g_renderToImageName = "renderToImage";
static const char *g_renderMatrixName = "renderMatrix";
static const char *g_renderFromImageMatrixName = "renderFromImageMatrix";
static const char *g_renderToImageMatrixName = "renderToImageMatrix";
static const char *g_renderScreenRectName = "renderScreenRect";
static const char *g_renderOpacityName = "renderOpacity";
static const char *g_renderTName = "renderT";
static const char *g_renderFacedGeometryCoefsName = "renderFacedGeometryCoefs";
static const char *g_renderGeomertySizeName = "renderGeomertySize";
static const char *g_renderIsGeomertySolidName = "renderIsGeomertySolid";
static const char *g_renderVertexAttributeName = "renderVertexAttribute";
static const char *g_renderTextureAttributeName = "renderTextureAttribute";
static const char *g_renderIndexesName = "renderIndexes";
static const char *g_intTypeName = "int";
static const char *g_floatTypeName = "float";
static const char *g_vec2TypeName = "vec2";
static const char *g_vec3TypeName = "vec3";
static const char *g_vec4TypeName = "vec4";
static const char *g_ivec2TypeName = "ivec2";
static const char *g_ivec3TypeName = "ivec3";
static const char *g_ivec4TypeName = "ivec4";
static const char *g_mat2TypeName = "mat2";
static const char *g_mat3TypeName = "mat3";
static const char *g_mat4TypeName = "mat4";
static const char *g_stringsTypeName = "strings";
static const char *g_sampler1DTypeName = "sampler1D";
static const char *g_sampler2DTypeName = "sampler2D";
static const char *g_sampler3DTypeName = "sampler3D";
static const char *g_attributeStorageName = "attribute";
static const char *g_uniformStorageName = "uniform";
static const char *g_indexesStorageName = "indexes";


// db request info
static const char *g_procedureExtraFieldName = "";
static const char *g_procedureConditionName = "WHERE";
static const char *g_procedureLimitName = "LIMIT";
static const char *g_procedureOrderByName = "ORDER BY";
static const char *g_procedureArguments = "ARGS";
static const char *g_procedureArgFunctionName = "ARGFUNCTION";
static const char *g_procedureSelectFunctionName = "SELECTFUNCTION";
static const char *g_procedureFilterNamesName = "FILTERNAMES";
static const char *g_procedureFilterConditionsName = "FILTERCONDITIONS";
static const char *g_noCachAPI = "NOCACHE";
static const char *g_cachAPI = "CACHE";


// list model
static const char *g_randSort = "?";
static const QChar g_reverseSortOrder = QChar('-');


// QML Objects
static const char *g_quizUserModel = "QuizUserModel";
static const char *g_mastactvaAPI = "MastactvaAPI";
static const char *g_effectModel = "EffectModel";
static const char *g_artefactModel = "ArtefactModel";
static const char *g_artefactTypeModel = "ArtefactTypeModel";
static const char *g_artefactArgTypeModel = "ArtefactArgTypeModel";
static const char *g_artefactArgStorageModel = "ArtefactArgStorageModel";
static const char *g_easingTypeModel = "EasingTypeModel";
static const char *g_galleryModel = "GalleryModel";


// server files
static const char *g_currentDir = "./";
static const char *g_assestsDir = "assets:/";


// json
static const char *g_argumentNameName = "name";
static const char *g_argumentStorageName = "storage";
static const char *g_argumentTypeName = "type";
static const char *g_argumentValueName = "value";
static const char *g_argumentDefaultValueName = "default_value";
static const char *g_textureArtefactFilenameName = "filename";
static const char *g_shaderArtefactFilenameName = "filename";
static const char *g_vertexShaderArtefactForStepName = "vertex";
static const char *g_fragmentShaderArtefactForStepName = "fragment";
static const char *g_argumentsArtefactForStepName = "arguments";
static const char *g_argumentArtefactForStepName = "argument";
static const char *g_textureArtefactForStepName = "texture";
static const char *g_stepsName = "steps";


// images
static const char *g_noImageName = ":/Images/Images/no-image.png";
static const char *g_noImage001Name = ":/Images/Images/no-image-001.png";
static const char *g_noImage002Name = ":/Images/Images/no-image-002.png";


// shaders
static const char *g_defaultVertexShaderName = ":/Shaders/Shaders/default.vsh";
static const char *g_defaultFragmentShaderName = ":/Shaders/Shaders/default.fsh";


// jsonData artefacts
static const char * g_jsonDataVariableValueName = "value";
static const char * g_jsonDataVariableObjectNameName = "objectName";
static const char * g_jsonDataVariableObjectStepIndexName = "objectStepIndex";
static const char * g_jsonDataVariableArtefactStepIndexName = "artefactStepIndex";
static const char * g_jsonDataVariablePositionName = "position";


#endif // NAMES_H
