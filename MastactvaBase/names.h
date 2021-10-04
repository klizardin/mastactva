/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef NAMES_H
#define NAMES_H


#include <QChar>


Q_DECL_UNUSED static const int g_userId = 1;


Q_DECL_UNUSED static const char *g_englishLanguage = "English";
Q_DECL_UNUSED static const char *g_belarusLanguage = "Belarus";
Q_DECL_UNUSED static const char *g_shaderName = "shader";
Q_DECL_UNUSED static const char *g_scriptName = "script";
Q_DECL_UNUSED static const char *g_argumentName = "argument";
Q_DECL_UNUSED static const char *g_storageName = "storage";
Q_DECL_UNUSED static const char *g_precisionName = "precision";
Q_DECL_UNUSED static const char *g_typeName = "type";
Q_DECL_UNUSED static const char *g_nameName = "name";
Q_DECL_UNUSED static const char *g_defaultValueName = "defaultValue";
Q_DECL_UNUSED static const char *g_descriptionName = "description";
Q_DECL_UNUSED static const char *g_geometrySolidName = "geomentrySolid";
Q_DECL_UNUSED static const char *g_geometryFacedName = "geomentryFaced";
Q_DECL_UNUSED static const char *g_geometrySizeName = "geomentrySize";
Q_DECL_UNUSED static const char *g_facedGeometryCoefName = "facedGeometryCoef";
Q_DECL_UNUSED static const char *g_colorsAttributeName = "attributeColors";
Q_DECL_UNUSED static const char *g_artefactsRelPath = "artefacts";
Q_DECL_UNUSED static const char *g_imagesRelPath = "images";
Q_DECL_UNUSED static const char *g_artefactTypeVertex = "Vertex";
Q_DECL_UNUSED static const char *g_artefactTypeFragment = "Fragment";
Q_DECL_UNUSED static const char *g_allImagesOfGalleryModel = "AllImagesOfGallery";
Q_DECL_UNUSED static const char *g_sqlText = "TEXT";
Q_DECL_UNUSED static const char *g_sqlInt = "INTEGER";
Q_DECL_UNUSED static const char *g_sqlDouble = "REAL";
Q_DECL_UNUSED static const char *g_trueName = "TRUE";
Q_DECL_UNUSED static const char *g_dbNameRW = "mastactva_rw";
Q_DECL_UNUSED static const char *g_dbNameRO = "mastactva_ro";
Q_DECL_UNUSED static const char *g_dbNameBase = "mastactva";
Q_DECL_UNUSED static const char *g_dbNameP2rw = "rw";
Q_DECL_UNUSED static const char *g_dbNameP2ro = "r0";
Q_DECL_UNUSED static const char *g_dbNameExt = ".db3";
Q_DECL_UNUSED static const char *g_splitTableRef = "_by_";
Q_DECL_UNUSED static const char *g_insertFieldSpliter = " , ";
Q_DECL_UNUSED static const char *g_spaceName = " ";
Q_DECL_UNUSED static const char *g_refPrefix = "ref_";
Q_DECL_UNUSED static const char *g_bindPrefix = ":";
Q_DECL_UNUSED static const char *g_errorDetailTag = "detail";
Q_DECL_UNUSED static const char *g_noImage = ":/resources/no-image.png";
Q_DECL_UNUSED static const char *g_noImageQRC = "qrc:///resources/no-image.png";
Q_DECL_UNUSED static const char *g_createDefaultEffectObjectProcedureName = "create_default_object";
Q_DECL_UNUSED static const char *g_effectIDName = "id";
Q_DECL_UNUSED static const char *g_defaultObjectInfoNameName = "name";
Q_DECL_UNUSED static const char *g_defaultObjectInfoProgrammerNameName = "programmer_name";
Q_DECL_UNUSED static const char *g_jsonDataVariableNameObjectListName = "objectsToRun";
Q_DECL_UNUSED static const char *g_defaultObjectInfoDescriptionName = "description";
Q_DECL_UNUSED static const char *g_defaultObjectInfoCreatedName = "created";
Q_DECL_UNUSED static const char *g_defaultObjectInfoProgrammerName = "";
Q_DECL_UNUSED static const char *g_randArgumentValueName = "__rand";
Q_DECL_UNUSED static const char *g_currentSpecialwordName = "__current";
Q_DECL_UNUSED static const char *g_vertexSpecialwordName = "_vertex";
Q_DECL_UNUSED static const char *g_vertexTextureSpecialwordName = "_texture";
Q_DECL_UNUSED static const char *g_vertexNormalSpecialwordName = "_normal";
Q_DECL_UNUSED static const char *g_indexesSpecialwordName = "_indexes";
Q_DECL_UNUSED static const char *g_mainSpecialwordName = "__main";
Q_DECL_UNUSED static const char *g_defaultObjectNameName = "default";
Q_DECL_UNUSED static const char *g_firstSpecialwordName = "__first";
Q_DECL_UNUSED static const char *g_lastSpecialwordName = "__last";
Q_DECL_UNUSED static const char *g_renderFromImageName = "renderFromImage";
Q_DECL_UNUSED static const char *g_renderToImageName = "renderToImage";
Q_DECL_UNUSED static const char *g_renderMatrixName = "renderMatrix";
Q_DECL_UNUSED static const char *g_renderFromImageMatrixName = "renderFromImageMatrix";
Q_DECL_UNUSED static const char *g_renderToImageMatrixName = "renderToImageMatrix";
Q_DECL_UNUSED static const char *g_renderScreenRectName = "renderScreenRect";
Q_DECL_UNUSED static const char *g_renderWindowSizeName = "renderWindowSize";
Q_DECL_UNUSED static const char *g_renderOpacityName = "renderOpacity";
Q_DECL_UNUSED static const char *g_renderTName = "renderT";
Q_DECL_UNUSED static const char *g_renderFacedGeometryCoefsName = "renderFacedGeometryCoefs";
Q_DECL_UNUSED static const char *g_renderGeomertySizeName = "renderGeomertySize";
Q_DECL_UNUSED static const char *g_renderIsGeomertySolidName = "renderIsGeomertySolid";
Q_DECL_UNUSED static const char *g_renderVertexAttributeName = "renderVertexAttribute";
Q_DECL_UNUSED static const char *g_renderTextureAttributeName = "renderTextureAttribute";
Q_DECL_UNUSED static const char *g_renderIndexesName = "renderIndexes";
Q_DECL_UNUSED static const char *g_intTypeName = "int";
Q_DECL_UNUSED static const char *g_floatTypeName = "float";
Q_DECL_UNUSED static const char *g_vec2TypeName = "vec2";
Q_DECL_UNUSED static const char *g_vec3TypeName = "vec3";
Q_DECL_UNUSED static const char *g_vec4TypeName = "vec4";
Q_DECL_UNUSED static const char *g_ivec2TypeName = "ivec2";
Q_DECL_UNUSED static const char *g_ivec3TypeName = "ivec3";
Q_DECL_UNUSED static const char *g_ivec4TypeName = "ivec4";
Q_DECL_UNUSED static const char *g_mat2TypeName = "mat2";
Q_DECL_UNUSED static const char *g_mat3TypeName = "mat3";
Q_DECL_UNUSED static const char *g_mat4TypeName = "mat4";
Q_DECL_UNUSED static const char *g_stringsTypeName = "strings";
Q_DECL_UNUSED static const char *g_sampler1DTypeName = "sampler1D";
Q_DECL_UNUSED static const char *g_sampler2DTypeName = "sampler2D";
Q_DECL_UNUSED static const char *g_sampler3DTypeName = "sampler3D";
Q_DECL_UNUSED static const char *g_attributeStorageName = "attribute";
Q_DECL_UNUSED static const char *g_uniformStorageName = "uniform";
Q_DECL_UNUSED static const char *g_indexesStorageName = "indexes";


// db request info
Q_DECL_UNUSED static const char *g_procedureExtraFieldName = "";
Q_DECL_UNUSED static const char *g_procedureConditionName = "WHERE";
Q_DECL_UNUSED static const char *g_procedureLimitName = "LIMIT";
Q_DECL_UNUSED static const char *g_procedureOrderByName = "ORDER BY";
Q_DECL_UNUSED static const char *g_procedureArguments = "ARGS";
Q_DECL_UNUSED static const char *g_procedureArgFunctionName = "ARGFUNCTION";
Q_DECL_UNUSED static const char *g_procedureSelectFunctionName = "SELECTFUNCTION";
Q_DECL_UNUSED static const char *g_procedureFilterNamesName = "FILTERNAMES";
Q_DECL_UNUSED static const char *g_procedureFilterConditionsName = "FILTERCONDITIONS";
Q_DECL_UNUSED static const char *g_noCachAPI = "NOCACHE";
Q_DECL_UNUSED static const char *g_cachAPI = "CACHE";


// list model
Q_DECL_UNUSED static const char *g_randSort = "?";
Q_DECL_UNUSED static const QChar g_reverseSortOrder = QChar('-');


// QML Objects
Q_DECL_UNUSED static const char *g_quizUserModel = "QuizUserModel";
Q_DECL_UNUSED static const char *g_mastactvaAPI = "MastactvaAPI";
Q_DECL_UNUSED static const char *g_effectModel = "EffectModel";
Q_DECL_UNUSED static const char *g_artefactModel = "ArtefactModel";
Q_DECL_UNUSED static const char *g_artefactTypeModel = "ArtefactTypeModel";
Q_DECL_UNUSED static const char *g_artefactArgTypeModel = "ArtefactArgTypeModel";
Q_DECL_UNUSED static const char *g_artefactArgStorageModel = "ArtefactArgStorageModel";
Q_DECL_UNUSED static const char *g_easingTypeModel = "EasingTypeModel";
Q_DECL_UNUSED static const char *g_galleryModel = "GalleryModel";
Q_DECL_UNUSED static const char *g_effectObjectsModel = "EffectObjectsModel";
Q_DECL_UNUSED static const char *g_objectInfoModel = "ObjectInfoModel";


// server files
Q_DECL_UNUSED static const char *g_currentDir = "./";
Q_DECL_UNUSED static const char *g_assestsDir = "assets:/";


// json
Q_DECL_UNUSED static const char *g_argumentNameName = "name";
Q_DECL_UNUSED static const char *g_argumentStorageName = "storage";
Q_DECL_UNUSED static const char *g_argumentTypeName = "type";
Q_DECL_UNUSED static const char *g_argumentValueName = "value";
Q_DECL_UNUSED static const char *g_argumentDefaultValueName = "default_value";
Q_DECL_UNUSED static const char *g_textureArtefactFilenameName = "filename";
Q_DECL_UNUSED static const char *g_shaderArtefactFilenameName = "filename";
Q_DECL_UNUSED static const char *g_vertexShaderArtefactForStepName = "vertex";
Q_DECL_UNUSED static const char *g_fragmentShaderArtefactForStepName = "fragment";
Q_DECL_UNUSED static const char *g_argumentsArtefactForStepName = "arguments";
Q_DECL_UNUSED static const char *g_argumentArtefactForStepName = "argument";
Q_DECL_UNUSED static const char *g_textureArtefactForStepName = "texture";
Q_DECL_UNUSED static const char *g_stepsName = "steps";


// images
Q_DECL_UNUSED static const char *g_noImageName = ":/Images/Images/no-image.png";
Q_DECL_UNUSED static const char *g_noImage001Name = ":/Images/Images/no-image-001.png";
Q_DECL_UNUSED static const char *g_noImage002Name = ":/Images/Images/no-image-002.png";


// shaders
Q_DECL_UNUSED static const char *g_defaultVertexShaderName = ":/Shaders/Shaders/default.vsh";
Q_DECL_UNUSED static const char *g_defaultFragmentShaderName = ":/Shaders/Shaders/default.fsh";


// jsonData artefacts
Q_DECL_UNUSED static const char * g_jsonDataVariableValueName = "value";
Q_DECL_UNUSED static const char * g_jsonDataVariableObjectNameName = "objectName";
Q_DECL_UNUSED static const char * g_jsonDataVariableObjectStepIndexName = "objectStepIndex";
Q_DECL_UNUSED static const char * g_jsonDataVariableArtefactStepIndexName = "artefactStepIndex";
Q_DECL_UNUSED static const char * g_jsonDataVariablePositionName = "position";
Q_DECL_UNUSED static const char * g_jsonDataVariableNamesName = "names";
Q_DECL_UNUSED static const char * g_jsonDataVariablePositionsName = "positions";


// lua artefacts
Q_DECL_UNUSED static const char * g_luaArtefactMainFunctionName = "main";


// lua effect calculations
Q_DECL_UNUSED static const char * g_imageMatrixDefaultCalculationName = "imageMatrix";
Q_DECL_UNUSED static const char * g_geometryDefaultCalculationName = "geometry";

#endif // NAMES_H
